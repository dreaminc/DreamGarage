#include "DreamOS.h"

//#include "DreamLogger/DreamLogger.h"
#include "DreamAppManager.h"

#include "Primitives/font.h"
#include "Core/Utilities.h"

#include "Cloud/Environment/PeerConnection.h"
#include "DreamMessage.h"

//#include "DreamGarage/DreamSoundSystem.h"
#include "Sound/AudioPacket.h"
#include "DreamGarage/AudioDataMessage.h"

#include "DreamAppMessage.h"
#include "DreamGarage/DreamSettingsApp.h"

// Messages
#include "PeerHandshakeMessage.h"
#include "PeerAckMessage.h"
#include "PeerStayAliveMessage.h"

#include "HAL/Pipeline/ProgramNode.h"

DreamOS::DreamOS() :
	m_versionDreamOS(DREAM_OS_VERSION_MAJOR, DREAM_OS_VERSION_MINOR, DREAM_OS_VERSION_MINOR_MINOR),
	m_pSandbox(nullptr)
{
	RESULT r = R_PASS;

	//Success:
	Validate();
	return;

	//Error:
	//	Invalidate();
	//	return;
}

DreamOS::~DreamOS() {
	// empty
}

// This will construct and initialize all of the parts of the DreamOS client
RESULT DreamOS::Initialize(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	// TODO: This should be put into time a manager / utility 
	srand(static_cast <unsigned> (time(0)));

	// Create the Sandbox
	m_pSandbox = SandboxFactory::MakeSandbox(CORE_CONFIG_SANDBOX_PLATFORM, this);
	CNM(m_pSandbox, "Failed to create sandbox");
	CVM(m_pSandbox, "Sandbox is Invalid!");
	CRM(m_pSandbox->SetDreamOSHandle(this), "Failed to set DreamOS handle");

	// Initialize logger
	auto pDreamLogger = DreamLogger::instance();
	CN(pDreamLogger);
	pDreamLogger->Log(DreamLogger::Level::INFO, "DreamOS Starting ...");

	// This gives our DreamOS app instance a chance to configure the
	// sandbox prior to it getting initialized 
	CRM(ConfigureSandbox(), "Failed to configure sandbox");

	// Check if Dream is launching from a web browser url.
	// a url command from a web page, to trigger the launch of Dream, would start with 'dreamos:run' command line.
	// The following code splits the white space of a single command line param in that case, into a list of command line arguments.
	if ((argc > 1) &&
		((std::string(argv[1]).substr(0, 11).compare("dreamos:run") == 0) ||
		(std::string(argv[1]).substr(0, 14).compare("dreamosdev:run") == 0))) {
		//  Dream is launching from a web page

		DOSLOG(INFO, "[DreamOS] Dream launched from web");

		// Decide if to split args or not
		if ((std::string(argv[1]).compare("dreamos:run") != 0) &&
			(std::string(argv[1]).compare("dreamosdev:run") != 0)) {
			std::vector<std::string> args{ argv[0] };
			int new_argc = 1;

			std::string strCMDInput = std::string(argv[1]); // The .exe location is the first argument

			std::string stdArgument;
			std::stringstream ssCMDInput(strCMDInput);

			while (ssCMDInput >> stdArgument) {
				args.push_back(stdArgument);
				new_argc++;
			}

			char** new_argv = new char*[new_argc];

			for (int i = 0; i < new_argc; i++) {
				new_argv[i] = new char;
				new_argv[i] = (char*)args[i].c_str();
			}

			CRM(m_pSandbox->Initialize(new_argc, (const char**)new_argv), "Failed to initialize Sandbox");
		}
		else {
			CRM(m_pSandbox->Initialize(argc, argv), "Failed to initialize Sandbox");
		}
	}
	else {
		DOSLOG(INFO, "[DreamOS] Dream runs from EXE");

		// Initialize the sandbox
		CRM(m_pSandbox->Initialize(argc, argv), "Failed to initialize Sandbox");
	}

	// Cloud Controller
	// TODO: This code is getting way too specialized 
	// TODO: need that module pattern
	if (GetSandboxConfiguration().fInitCloud) {
		CRM(RegisterPeerConnectionObserver(this), "Failed to register Peer Connection Observer");
		CRM(RegisterEnvironmentObserver(this), "Failed to register environment controller observer");
		CRM(RegisterUserObserver(this), "Failed to register user controller observer");
	}

	// Give the Client a chance to set up the pipeline
	CRM(SetupPipeline(GetRenderPipeline()), "Failed to set up pipeline");

	// Load the scene
	CRM(LoadScene(), "Failed to load scene");

	//m_pKeyboard = LaunchDreamApp<UIKeyboard>(this);
	//CN(m_pKeyboard);

	// TODO: Move log in to here (avoid loading ordering issues since cloud is async and other things may
	// be as well)

	// Auto Login Handling
	/*
	auto pCommandLineManager = CommandLineManager::instance();
	CN(pCommandLineManager);

	if (pCommandLineManager->GetParameterValue("login").compare("auto") == 0) {
		// auto login
		CloudController* pCloudController = GetCloudController();
		if (pCloudController != nullptr) {
			pCloudController->Start();
		}
	}
	//*/

	// Audio System
	if (m_pSandbox->m_SandboxConfiguration.fInitSound) {
		CRM(InitializeDreamSoundSystem(), "Failed to initialize the Dream Sound System");
		CRM(RegisterSoundSystemObserver(this), "Failed to register this as sound system observer");
	}

	if (m_pSandbox->m_SandboxConfiguration.fInitUserApp) {
		CRM(InitializeDreamUserApp(), "Failed to initalize user app");
	}

	CR(InitializeKeyboard());

	CRM(DidFinishLoading(), "Failed to run DidFinishLoading");

	// Register the update callback
	CRM(RegisterUpdateCallback(std::bind(&DreamOS::Update, this)), "Failed to register DreamOS update callback");

Error:
	return r;
}

// Cloud
RESULT DreamOS::OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) {
	RESULT r = R_PASS;

	CN(pPeerConnection);

	DEBUG_LINEOUT("DataString: %s", strDataChannelMessage.c_str());
	DOSLOG(INFO, "[DreamOS] DataString: %v", strDataChannelMessage);

Error:
	return r;
}

RESULT DreamOS::OnDataChannel(PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	CR(pDreamPeer->OnDataChannel());

	{
		long userID = GetUserID();
		long peerUserID = pPeerConnection->GetPeerUserID();

		DOSLOG(INFO, "[DreamOS] SEND_PEER_HANDSHAKE user: %v peer: %v", userID, peerUserID);
		// Initialize handshake - only add user when peer connection stabilized 
		PeerHandshakeMessage peerHandshakeMessage(userID, peerUserID);
		CR(SendDataMessage(peerUserID, &peerHandshakeMessage));

		pDreamPeer->SentHandshakeRequest();
	}

Error:
	return r;
}

// Create a catch for incomplete handshakes
RESULT DreamOS::CheckDreamPeerAppStates() {
	RESULT r = R_PASS;

	// use a copy to prevent iterator issues
	auto dreamPeerApps = m_dreamPeerApps;
	DOSLOG(INFO, "[DreamOS] check peer app states");

	for (auto &dreamPeerAppPair : dreamPeerApps) {
		auto pDreamPeerApp = dreamPeerAppPair.second;
		CN(pDreamPeerApp);

		// Detect handshake request hung
		if (pDreamPeerApp->IsDataChannel() &&
			pDreamPeerApp->GetState() != DreamPeerApp::state::ESTABLISHED)
		{
			//if (pDreamPeerApp->IsHandshakeRequestHung()) {
			long userID = GetUserID();
			long peerUserID = pDreamPeerApp->GetPeerUserID();

			DOSLOG(INFO, "[DreamOS] HANDSHAKE_REQUEST_HUNG userid:%v peerid:%v", userID, peerUserID);

			// Initialize handshake - only add user when peer connection stabilized 
			PeerHandshakeMessage peerHandshakeMessage(userID, peerUserID);
			CR(SendDataMessage(peerUserID, &peerHandshakeMessage));
		}

	}

Error:
	return r;
}

RESULT DreamOS::OnAudioChannel(PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	CR(pDreamPeer->OnAudioChannel());

Error:
	return r;
}

RESULT DreamOS::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	// Create a new peer
	auto pDreamPeer = CreateNewPeer(pPeerConnection);
	CN(pDreamPeer);
	
	CR(pDreamPeer->RegisterDreamPeerObserver(this));

Error:
	return r;
}

RESULT DreamOS::OnPeerConnectionClosed(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	auto pDreamPeerApp = FindPeer(pPeerConnection);
	CN(pDreamPeerApp);

	// First give client layer to do something 
	CR(OnDreamPeerConnectionClosed(pDreamPeerApp));

	// Delete the dream peer
	CR(RemovePeer(pDreamPeerApp));

Error:
	return r;
}

WebRTCPeerConnectionProxy* DreamOS::GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;
	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy = nullptr;

	WebRTCImpProxy* pWebRTCProxy = (WebRTCImpProxy*)(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::WEBRTC));
	CN(pWebRTCProxy);

	pWebRTCPeerConnectionProxy = pWebRTCProxy->GetWebRTCPeerConnectionProxy(pPeerConnection);
	CN(pWebRTCPeerConnectionProxy);

	return pWebRTCPeerConnectionProxy;

Error:
	return nullptr;
}

RESULT DreamOS::OnDreamPeerStateChange(DreamPeerApp* pDreamPeer) {
	RESULT r = R_PASS;

	switch (pDreamPeer->GetState()) {
		case DreamPeerApp::state::ESTABLISHED: {
			CR(OnNewDreamPeer(pDreamPeer));
		} break;
	}

Error:
	return r;
}

RESULT DreamOS::RegisterDOSObserver(DOSObserver *pDOSObserver) {
	RESULT r = R_PASS;

	CNM((pDOSObserver), "Observer cannot be nullptr");
	CBM((m_pDOSObserver == nullptr), "Can't overwrite DOS observer");

	m_pDOSObserver = pDOSObserver;

Error:
	return r;
}

RESULT DreamOS::UnregisterDOSObserver(DOSObserver *pDOSObserver) {
	RESULT r = R_PASS;

	CN(pDOSObserver);
	CBM((m_pDOSObserver == pDOSObserver), "DOS Observer is not set to this object");

	m_pDOSObserver = nullptr;

Error:
	return r;
}

RESULT DreamOS::SendDOSMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	CNR(m_pDOSObserver, R_SKIPPED);

	CR(m_pDOSObserver->HandleDOSMessage(strMessage));

Error:
	return r;
}

RESULT DreamOS::RegisterWindows64Observer(Windows64Observer *pWindows64Observer) {
	RESULT r = R_PASS;

	CNM((pWindows64Observer), "Observer cannot be nullptr");
	CBM((m_pWindows64Observer == nullptr), "Can't overwrite DOS observer");

	m_pWindows64Observer = pWindows64Observer;

Error:
	return r;
}

RESULT DreamOS::UnregisterWindows64Observer(Windows64Observer *pWindows64Observer) {
	RESULT r = R_PASS;

	CN(pWindows64Observer);
	CBM((m_pWindows64Observer == pWindows64Observer), "DOS Observer is not set to this object");

	m_pWindows64Observer = nullptr;

Error:
	return r;
}

RESULT DreamOS::OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) {
	RESULT r = R_PASS;
	m_pWindows64Observer->HandleWindows64CopyData(messageSize, pMessageData, pxHeight, pxWidth);
	return r;
}

RESULT DreamOS::OnDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	// Send the dream app message to the dream app manager
	CR(m_pSandbox->HandleDreamAppMessage(pPeerConnection, pDreamAppMessage));

Error:
	return r;
}

RESULT DreamOS::OnDataMessage(PeerConnection* pPeerConnection, Message *pDataMessage) {
	RESULT r = R_PASS;

	DreamMessage::type dreamMsgType = (DreamMessage::type)(pDataMessage->GetType());

	// Route the message to the right place

	if (dreamMsgType < DreamMessage::type::CLIENT) {
		// DREAM OS Messages
		switch (dreamMsgType) {
		case DreamMessage::type::PEER_HANDSHAKE: {
			DOSLOG(INFO, "[DreamOS] PEER_HANDSHAKE user: %v peer: %v", pPeerConnection->GetUserID(), pPeerConnection->GetPeerUserID());
			PeerHandshakeMessage *pPeerHandshakeMessage = reinterpret_cast<PeerHandshakeMessage*>(pDataMessage);
			CR(HandlePeerHandshakeMessage(pPeerConnection, pPeerHandshakeMessage));
		} break;

		case DreamMessage::type::PEER_STAYALIVE: {
			PeerStayAliveMessage *pPeerStayAliveMessage = reinterpret_cast<PeerStayAliveMessage*>(pDataMessage);
			CR(HandlePeerStayAliveMessage(pPeerConnection, pPeerStayAliveMessage));
		} break;

		case DreamMessage::type::PEER_ACK: {
			PeerAckMessage *pPeerAckMessage = reinterpret_cast<PeerAckMessage*>(pDataMessage);
			CR(HandlePeerAckMessage(pPeerConnection, pPeerAckMessage));
		} break;

		default: {
			DEBUG_LINEOUT("Unhandled Dream OS Message of Type 0x%I64x", dreamMsgType);
		} break;
		}
	}
	else if (dreamMsgType >= DreamMessage::type::CLIENT && dreamMsgType < DreamMessage::type::APP) {
		// Dream Client Messages
		CR(OnDreamMessage(pPeerConnection, (DreamMessage*)(pDataMessage)));
	}
	else if (dreamMsgType >= DreamMessage::type::APP) {
		CR(OnDreamAppMessage(pPeerConnection, (DreamAppMessage*)(pDataMessage)));
	}
	else {
		DEBUG_LINEOUT("Unhandled Dream Message of Type 0x%I64x", dreamMsgType);
	}

Error:
	return r;
}

// Environment

// Peers
RESULT DreamOS::HandlePeerHandshakeMessage(PeerConnection* pPeerConnection, PeerHandshakeMessage *pPeerHandshakeMessage) {
	RESULT r = R_PASS;

	// Retrieve peer
	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	long userID = GetUserID();
	long peerUserID = pPeerConnection->GetPeerUserID();

	{
		// ACK the handshake
		PeerAckMessage peerHandshakeMessageAck(userID, peerUserID, PeerAckMessage::type::PEER_HANDSHAKE);
		CR(SendDataMessage(peerUserID, &peerHandshakeMessageAck));

		pDreamPeer->SentHandshakeACK();

		/*
		if (pDreamPeer->IsPeerReady()) {
		int a = 5;
		}
		*/
	}

Error:
	return r;
}

RESULT DreamOS::HandlePeerStayAliveMessage(PeerConnection* pPeerConnection, PeerStayAliveMessage *pPeerStayAliveMessage) {
	RESULT r = R_PASS;

	// Retrieve peer
	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	CB((pDreamPeer->IsPeerReady()));

	{
		long userID = GetUserID();
		long peerUserID = pPeerConnection->GetPeerUserID();

		// Initialize handshake - only add user when peer connection stabilized 
		PeerAckMessage peerHandshakeMessageAck(userID, peerUserID, PeerAckMessage::type::PEER_STAY_ALIVE);
		CR(SendDataMessage(peerUserID, &peerHandshakeMessageAck));
	}

Error:
	return r;
}

RESULT DreamOS::HandlePeerAckMessage(PeerConnection* pPeerConnection, PeerAckMessage *pPeerAckMessage) {
	RESULT r = R_PASS;

	// Retrieve peer
	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	long userID = GetUserID();
	long peerUserID = pPeerConnection->GetPeerUserID();

	switch (pPeerAckMessage->GetACKType()) {
	case PeerAckMessage::type::PEER_HANDSHAKE: {
		pDreamPeer->ReceivedHandshakeACK();
		DOSLOG(INFO, "[DreamOS] PEER_HANDSHAKE_ACK, user: %v, peer: %v", userID, peerUserID);

		/*
		if (pDreamPeer->IsPeerReady()) {
		int a = 5;
		}
		*/
	} break;

	case PeerAckMessage::type::PEER_STAY_ALIVE: {
		// TODO: update the stay alive
	} break;

	default: {
		// TODO: ?
	} break;
	}

Error:
	return r;
}

std::shared_ptr<DreamPeerApp> DreamOS::CreateNewPeer(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;
	std::shared_ptr<DreamPeerApp> pDreamPeerApp = nullptr;

	long peerUserID = 0;

	CNM(pPeerConnection, "Peer Connection invalid");
	peerUserID = pPeerConnection->GetPeerUserID();

	CBM((m_dreamPeerApps.find(peerUserID) == m_dreamPeerApps.end()), "Error: Peer user ID %d already exists", peerUserID);

	//pDreamPeerApp = LaunchDreamApp<DreamPeerApp>(this, true);
	pDreamPeerApp = LaunchDreamApp<DreamPeerApp>(this, true);
	CNM(pDreamPeerApp, "Failed to create dream peer app");

	CR(pDreamPeerApp->SetPeerConnection(pPeerConnection));

	// Set map
	m_dreamPeerApps[peerUserID] = pDreamPeerApp;
	DOSLOG(INFO, "DreamPeerApp added to connections");

	return pDreamPeerApp;

Error:
	if (pDreamPeerApp != nullptr) {
		pDreamPeerApp = nullptr;
	}

	return nullptr;
}

std::shared_ptr<DreamPeerApp> DreamOS::FindPeer(PeerConnection *pPeerConnection) {
	return FindPeer(pPeerConnection->GetPeerUserID());
}

std::shared_ptr<DreamPeerApp> DreamOS::FindPeer(long peerUserID) {
	std::map<long, std::shared_ptr<DreamPeerApp>>::iterator it;

	if ((it = m_dreamPeerApps.find(peerUserID)) != m_dreamPeerApps.end()) {
		return (*it).second;
	}

	return nullptr;
}

RESULT DreamOS::RemovePeer(long peerUserID) {
	std::map<long, std::shared_ptr<DreamPeerApp>>::iterator it;

	if ((it = m_dreamPeerApps.find(peerUserID)) != m_dreamPeerApps.end()) {
		m_dreamPeerApps.erase(it);
		return R_PASS;
	}

	return R_NOT_FOUND;
}

RESULT DreamOS::RemovePeer(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	RESULT r = R_PASS;

	std::map<long, std::shared_ptr<DreamPeerApp>>::iterator it;

	for (auto &pairDreamPeer : m_dreamPeerApps) {
		if (pairDreamPeer.second == pDreamPeer) {
			it = m_dreamPeerApps.find(pairDreamPeer.first);
			auto pDreamPeerApp = (*it).second;

			m_dreamPeerApps.erase(it);

			CRM(ShutdownDreamApp<DreamPeerApp>(pDreamPeerApp), "Failed to shut down dream peer app");

			return R_PASS;
		}
	}

	return R_NOT_FOUND;

Error:
	return r;
}

RESULT DreamOS::ClearPeers() {
	RESULT r = R_PASS;

	std::map<long, std::shared_ptr<DreamPeerApp>>::iterator it;

	for (auto &pairDreamPeer : m_dreamPeerApps) {

		std::shared_ptr<DreamPeerApp> pDreamPeerApp = pairDreamPeer.second;
		
		CRM(ShutdownDreamApp<DreamPeerApp>(pDreamPeerApp), "Failed to shut down dream peer app");
	}

Error:
	//m_dreamPeerApps.clear();

	return r;
}

bool DreamOS::HasPeerApps() {
	RESULT r = R_PASS;

	CN(m_pSandbox);
	CN(m_pSandbox->m_pDreamAppManager);


	return m_pSandbox->m_pDreamAppManager->GetDreamApp("DreamPeerApp").size() > 0;

Error:
	return false;
}

DreamPeerApp::state DreamOS::GetPeerState(long peerUserID) {
	std::shared_ptr<DreamPeerApp> pDreamPeer = nullptr;

	if ((pDreamPeer = FindPeer(peerUserID)) != nullptr) {
		return pDreamPeer->GetState();
	}

	return DreamPeerApp::state::INVALID;
}


stereocamera* DreamOS::GetCamera() {
	return m_pSandbox->GetCamera();
}

Pipeline *DreamOS::GetRenderPipeline() {
	return m_pSandbox->m_pHALImp->GetRenderPipelineHandle();
}

point DreamOS::GetCameraPosition() {
	return m_pSandbox->GetCameraPosition();
}

hand *DreamOS::GetHand(HAND_TYPE handType) {
	return m_pSandbox->GetHand(handType);
}

ProgramNode* DreamOS::MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags) {
	RESULT r = R_PASS;

	ProgramNode *pProgramNode = nullptr;

	pProgramNode = m_pSandbox->MakeProgramNode(strNodeName, optFlags);
	CN(pProgramNode);

	return pProgramNode;

Error:
	if (pProgramNode != nullptr) {
		delete pProgramNode;
		pProgramNode = nullptr;
	}

	return nullptr;
}

quaternion DreamOS::GetCameraOrientation() {
	return m_pSandbox->GetCameraOrientation();
}

RESULT DreamOS::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DREAM OS %s Starting ...", m_versionDreamOS.GetString().c_str());

	// This will start the application
	CRM(m_pSandbox->Show(), "Failed to show sandbox window");
	CR(m_pSandbox->RunAppLoop());

Error:
	DreamLogger::instance()->Flush();
	return r;
}

RESULT DreamOS::Exit(RESULT exitcode) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", m_versionDreamOS.GetString().c_str(), exitcode);

	CR(m_pSandbox->PendShutdown());
	CR(exitcode);

Error:
	return r;
}

InteractionEngineProxy *DreamOS::GetInteractionEngineProxy() {
	return m_pSandbox->GetInteractionEngineProxy();
}

RESULT DreamOS::GetMouseRay(ray &rCast, double t) {
	return m_pSandbox->GetMouseRay(rCast, t);
}

HMD *DreamOS::GetHMD() {
	return m_pSandbox->m_pHMD;
}

HWND DreamOS::GetDreamHWND() {
	return m_pSandbox->GetWindowHandle();
}

RESULT DreamOS::RecenterHMD() {
	return m_pSandbox->m_pHMD->RecenterHMD();
}

RESULT DreamOS::SetHALConfiguration(HALImp::HALConfiguration halconf) {
	return m_pSandbox->SetHALConfiguration(halconf);
}

const HALImp::HALConfiguration& DreamOS::GetHALConfiguration() {
	return m_pSandbox->GetHALConfiguration();
}

CloudController *DreamOS::GetCloudController() {
	return m_pSandbox->m_pCloudController;
}

std::shared_ptr<DreamSoundSystem> DreamOS::GetDreamSoundSystem() {
	return m_pDreamSoundSystem;
}

long DreamOS::GetUserID() {
	return m_pSandbox->m_pCloudController->GetUserID();
}

ControllerProxy* DreamOS::GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE controllerType) {
	return GetCloudController()->GetControllerProxy(controllerType);
}

DreamAppHandle* DreamOS::RequestCaptureAppUnique(std::string strAppName, DreamAppBase* pHoldingApp) {
	RESULT r = R_PASS;
	DreamAppHandle* pDreamAppHandle = nullptr;

	CN(pHoldingApp);

	{
		UID appUID = GetUniqueAppUID(strAppName);
		CB((appUID.IsValid()));

		pDreamAppHandle = CaptureApp(appUID, pHoldingApp);
		CN(pDreamAppHandle);

		return pDreamAppHandle;
	}

Error:
	if (pDreamAppHandle) {
		pDreamAppHandle = nullptr;
	}
	return nullptr;
}

DreamAppHandle* DreamOS::CaptureApp(UID uid, DreamAppBase* pHoldingApp) {
	return m_pSandbox->m_pDreamAppManager->CaptureApp(uid, pHoldingApp);
}

RESULT DreamOS::ReleaseApp(DreamAppHandle* pHandle, UID appUID, DreamAppBase* pHoldingApp) {
	return m_pSandbox->m_pDreamAppManager->ReleaseApp(pHandle, appUID, pHoldingApp);
}

RESULT DreamOS::RequestReleaseAppUnique(DreamAppHandle* pHandle, DreamAppBase* pHoldingApp) {
	RESULT r = R_PASS;

	DreamAppBase *pTargetApp = nullptr;

	CN(pHoldingApp);
	CN(pHandle);

	pTargetApp = dynamic_cast<DreamAppBase*>(pHandle);
	CN(pTargetApp);

	{
		UID appUID = GetUniqueAppUID(pTargetApp->GetAppName());
		CB((appUID.IsValid()));

		CR(m_pSandbox->m_pDreamAppManager->ReleaseApp(pHandle, appUID, pHoldingApp));
	}

Error:
	return r;
}

std::vector<UID> DreamOS::GetAppUID(std::string strAppName) {
	return m_pSandbox->m_pDreamAppManager->GetAppUID(strAppName);
}

UID DreamOS::GetUniqueAppUID(std::string strAppName) {
	std::vector<UID> vAppUID = m_pSandbox->m_pDreamAppManager->GetAppUID(strAppName);
	
	if (vAppUID.size() == 1) {
		return vAppUID[0];
	}
	else {
		return UID::MakeInvalidUID();
	}
}

std::shared_ptr<DreamAppBase> DreamOS::GetDreamAppFromUID(UID appUID) {
	return m_pSandbox->m_pDreamAppManager->GetDreamAppFromUID(appUID);
}

std::vector<UID> DreamOS::GetModuleUID(std::string strName) {
	return m_pSandbox->m_pDreamModuleManager->GetModuleUID(strName);
}

UID DreamOS::GetUniqueModuleUID(std::string strName) {
	std::vector<UID> vModuleUID = m_pSandbox->m_pDreamModuleManager->GetModuleUID(strName);
	
	if (vModuleUID.size() == 1) {
		return vModuleUID[0];
	}
	else {
		return UID::MakeInvalidUID();
	}
}

std::shared_ptr<DreamModuleBase> DreamOS::GetDreamModuleFromUID(UID moduleUID) {
	return m_pSandbox->m_pDreamModuleManager->GetDreamModuleFromUID(moduleUID);
}

HALImp* DreamOS::GetHALImp() {
	return m_pSandbox->m_pHALImp;
}

RESULT DreamOS::InitializeKeyboard() {
	RESULT r = R_PASS;

	m_pKeyboard = LaunchDreamApp<UIKeyboard>(this, false);
	CN(m_pKeyboard);

Error:
	return r;
}

RESULT DreamOS::InitializeDreamUserApp() {
	RESULT r = R_PASS;

	m_pDreamUserApp = LaunchDreamApp<DreamUserApp>(this, false);
	CNM(m_pDreamUserApp, "Failed to launch dream user app");

	//WCRM(m_pDreamUserApp->SetHand(GetHand(HAND_TYPE::HAND_LEFT)), "Warning: Failed to set left hand");
	//WCRM(m_pDreamUserApp->SetHand(GetHand(HAND_TYPE::HAND_RIGHT)), "Warning: Failed to set right hand");

Error:
	return r;
}

RESULT DreamOS::InitializeCloudController() {
	RESULT r = R_PASS;

	CR(m_pSandbox->InitializeCloudController());

Error:
	return r;
}

RESULT DreamOS::InitializeDreamSoundSystem() {
	RESULT r = R_PASS;

	DOSLOG(INFO, "Initializing Dream Sound System");

	m_pDreamSoundSystem = LaunchDreamModule<DreamSoundSystem>(this);
	CNM(m_pDreamSoundSystem, "Failed to launch Dream Sound System Module");

Error:
	return r;
}

RESULT DreamOS::RegisterSoundSystemObserver(DreamSoundSystem::observer *pObserver) {
	RESULT r = R_PASS;

	CNM(m_pDreamSoundSystem, "Sound system not initialized");
	CR(m_pDreamSoundSystem->RegisterObserver(pObserver));

Error:
	return r;
}

RESULT DreamOS::UnregisterSoundSystemObserver() {
	RESULT r = R_PASS;

	CNM(m_pDreamSoundSystem, "Sound system not initialized");
	CR(m_pDreamSoundSystem->UnregisterObserver());

Error:
	return r;
}

RESULT DreamOS::OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
	RESULT r = R_PASS;

	int nChannels = pCaptureBuffer->NumChannels();
	int samplingFrequency = pCaptureBuffer->GetSamplingRate();

	AudioPacket pendingAudioPacket;
	pCaptureBuffer->GetAudioPacket(numFrames, &pendingAudioPacket);

	// Measure time diff
	//static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
	//std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
	//auto diffVal = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();
	//lastUpdateTime = timeNow;
	
	// Broadcast out captured audio
	if (GetCloudController() != nullptr) {
		GetCloudController()->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);
	}

	//std::chrono::system_clock::time_point timeNow2 = std::chrono::system_clock::now();
	//auto diffVal2 = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow2 - timeNow).count();

Error:
	return r;
}

std::shared_ptr<SpatialSoundObject> DreamOS::AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) {
	RESULT r = R_PASS;

	CNM(m_pDreamSoundSystem, "Sound system not initialized");

	return m_pDreamSoundSystem->AddSpatialSoundObject(ptPosition, vEmitterDirection, vListenerDirection);

Error:
	return nullptr;
}

std::shared_ptr<SoundFile> DreamOS::LoadSoundFile(const std::wstring &wstrFilename, SoundFile::type soundFileType) {
	RESULT r = R_PASS;

	CNM(m_pDreamSoundSystem, "Sound system not initialized");

	return m_pDreamSoundSystem->LoadSoundFile(wstrFilename, soundFileType);

Error:
	return nullptr;
}

RESULT DreamOS::PlaySoundFile(std::shared_ptr<SoundFile> pSoundFile) {
	RESULT r = R_PASS;

	CNM(m_pDreamSoundSystem, "Sound system not initialized");

	return m_pDreamSoundSystem->PlaySoundFile(pSoundFile);

Error:
	return r;
}

// This is a pass-thru at the moment
RESULT DreamOS::AddPhysicsObject(VirtualObj *pObject) {
	return m_pSandbox->AddPhysicsObject(pObject);
}

RESULT DreamOS::AddObject(VirtualObj *pObject) {
	return m_pSandbox->AddObject(pObject);
}

// This is a pass-thru at the moment
RESULT DreamOS::AddObjectToInteractionGraph(VirtualObj *pObject) {
	return m_pSandbox->AddObjectToInteractionGraph(pObject);
}

RESULT DreamOS::AddInteractionObject(VirtualObj *pObject) {
	return m_pSandbox->AddInteractionObject(pObject);
}

/*
RESULT DreamOS::UpdateInteractionPrimitive(const ray &rCast) {
return m_pSandbox->UpdateInteractionPrimitive(rCast);
}
*/

RESULT DreamOS::SetGravityAcceleration(double acceleration) {
	return m_pSandbox->SetGravityAcceleration(acceleration);
}

// Sandbox Level Objects 
std::shared_ptr<NamedPipeClient> DreamOS::MakeNamedPipeClient(std::wstring strPipename) {
	return m_pSandbox->MakeNamedPipeClient(strPipename);
}

std::shared_ptr<NamedPipeServer> DreamOS::MakeNamedPipeServer(std::wstring strPipename) {
	return m_pSandbox->MakeNamedPipeServer(strPipename);
}

RESULT DreamOS::SetGravityState(bool fEnabled) {
	return m_pSandbox->SetGravityState(fEnabled);
}

RESULT DreamOS::RemoveObject(VirtualObj *pObject) {
	return m_pSandbox->RemoveObject(pObject);
}

RESULT DreamOS::RemoveAllObjects() {
	return m_pSandbox->RemoveAllObjects();
}

light* DreamOS::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->AddLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

light* DreamOS::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

FlatContext* DreamOS::AddFlatContext(int width, int height, int channels) {
	return m_pSandbox->AddFlatContext(width, height, channels);
}

RESULT DreamOS::RenderToTexture(FlatContext *pContext) {
	return m_pSandbox->RenderToTexture(pContext);
}

sphere* DreamOS::AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pSandbox->AddSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

sphere* DreamOS::MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pSandbox->MakeSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

volume* DreamOS::AddVolume(double side, bool fTriangleBased) {
	return m_pSandbox->AddVolume(side, fTriangleBased);
}

volume* DreamOS::AddVolume(double width, double length, double height, bool fTriangleBased) {
	return m_pSandbox->AddVolume(width, length, height, fTriangleBased);
}

cylinder* DreamOS::AddCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	return m_pSandbox->AddCylinder(radius, height, numAngularDivisions, numVerticalDivisions);
}

DimRay* DreamOS::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	return m_pSandbox->MakeRay(ptOrigin, vDirection, step, fDirectional);
}

DimRay* DreamOS::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	return m_pSandbox->AddRay(ptOrigin, vDirection, step, fDirectional);
}

DimPlane* DreamOS::AddPlane(point ptOrigin, vector vNormal) {
	return m_pSandbox->AddPlane(ptOrigin, vNormal);
}

volume* DreamOS::MakeVolume(double width, double length, double height, bool fTriangleBased) {
	return m_pSandbox->MakeVolume(width, length, height, fTriangleBased);
}

volume* DreamOS::MakeVolume(double side, bool fTriangleBased) {
	return m_pSandbox->AddVolume(side, fTriangleBased);
}

quad *DreamOS::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
	return m_pSandbox->AddQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
}

quad* DreamOS::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
	return m_pSandbox->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
}

HysteresisObject *DreamOS::MakeHysteresisObject(float onThreshold, float offThreshold, HysteresisObjectType objectType) {
	return m_pSandbox->MakeHysteresisObject(onThreshold, offThreshold, objectType);
}

std::shared_ptr<UIKeyboard> DreamOS::GetKeyboardApp() {
	return m_pKeyboard;
}

std::shared_ptr<DreamUserApp> DreamOS::GetUserApp() {
	return m_pDreamUserApp;
}

RESULT DreamOS::ReleaseFont(std::wstring wstrFontFileName) {
	RESULT r = R_PASS;

	auto it = m_fonts.find(wstrFontFileName);
	CBR((it != m_fonts.end()), R_NOT_FOUND);

	m_fonts.erase(it);

Error:
	return r;
}

std::shared_ptr<font> DreamOS::GetFont(std::wstring wstrFontFileName) {
	RESULT r = R_PASS;

	auto it = m_fonts.find(wstrFontFileName);
	CBR((it != m_fonts.end()), R_NOT_FOUND);

	return (*it).second;

Error:
	return nullptr;
}

RESULT DreamOS::ClearFonts() {
	m_fonts.clear();
	return R_PASS;
}

std::shared_ptr<font> DreamOS::MakeFont(std::wstring wstrFontFileName, bool fDistanceMap) {
	RESULT r = R_PASS;

	// First check font store
	std::shared_ptr<font> pFont = GetFont(wstrFontFileName);

	if (pFont == nullptr) {
		pFont = std::make_shared<font>(wstrFontFileName, fDistanceMap);
		CN(pFont);

		{
			std::wstring strFile = L"Fonts/" + pFont->GetFontImageFile();
			const wchar_t* pszFile = strFile.c_str();

			CR(pFont->SetTexture(std::shared_ptr<texture>(MakeTexture(texture::type::TEXTURE_2D, const_cast<wchar_t*>(pszFile)))));
		}

		// Push font into store
		m_fonts[wstrFontFileName] = pFont;
	}

	return pFont;

Error:
	if (pFont != nullptr) {
		pFont = nullptr;
	}

	return nullptr;
}

text* DreamOS::AddText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
	return m_pSandbox->AddText(pFont, pLayout, margin, textFlags);
}

text* DreamOS::MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
	return m_pSandbox->MakeText(pFont, pLayout, margin, textFlags);
}

text* DreamOS::AddText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
	return m_pSandbox->AddText(pFont, strContent, lineHeightM, textFlags);
}

text* DreamOS::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
	return m_pSandbox->MakeText(pFont, strContent, lineHeightM, textFlags);
}

text *DreamOS::AddText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
	return m_pSandbox->AddText(pFont, strContent, width, height, textFlags);
}

text *DreamOS::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
	return m_pSandbox->MakeText(pFont, strContent, width, height, textFlags);
}

text *DreamOS::MakeText(std::shared_ptr<font> pFont, const std::string& content, double width, double height, bool fBillboard) {
	return m_pSandbox->MakeText(pFont, content, width, height, fBillboard);
}

text* DreamOS::AddText(std::shared_ptr<font> pFont, const std::string& content, double width, double height, bool fBillboard) {
	return m_pSandbox->AddText(pFont, content, width, height, fBillboard);
}

texture* DreamOS::MakeTexture(texture::type type, const wchar_t *pszFilename) {
	return m_pSandbox->MakeTexture(type, pszFilename);
}

texture *DreamOS::MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n) {
	return m_pSandbox->MakeTextureFromFileBuffer(type, pBuffer, pBuffer_n);
}

texture* DreamOS::MakeTexture(const texture &srcTexture) {
	return m_pSandbox->MakeTexture(srcTexture);
}

texture* DreamOS::MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) {
	return m_pSandbox->MakeTexture(type, width, height, pixelFormat, channels, pBuffer, pBuffer_n);
}

cubemap* DreamOS::MakeCubemap(const std::wstring &wstrCubemapName) {
	return m_pSandbox->MakeCubemap(wstrCubemapName);
}

skybox *DreamOS::AddSkybox() {
	return m_pSandbox->AddSkybox();
}

skybox *DreamOS::MakeSkybox() {
	return m_pSandbox->MakeSkybox();
}

/*
model *DreamOS::AddModel(wchar_t *pszModelName) {
return m_pSandbox->AddModel(pszModelName);
}

model *DreamOS::MakeModel(wchar_t *pszModelName) {
return m_pSandbox->AddModel(pszModelName);
}
*/

model *DreamOS::MakeModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	return m_pSandbox->MakeModel(wstrModelFilename, pTexture);
}

model *DreamOS::AddModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	return m_pSandbox->AddModel(wstrModelFilename, pTexture);
}

model *DreamOS::MakeModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	return m_pSandbox->MakeModel(wstrModelFilename, modelFactoryFlags);
}

model *DreamOS::AddModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	return m_pSandbox->AddModel(wstrModelFilename, modelFactoryFlags);
}

composite *DreamOS::AddComposite() {
	return m_pSandbox->AddComposite();
}

composite *DreamOS::MakeComposite() {
	return m_pSandbox->MakeComposite();
}

user *DreamOS::MakeUser() {
	return m_pSandbox->MakeUser();
}

user *DreamOS::AddUser() {
	return m_pSandbox->AddUser();
}

RESULT DreamOS::RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback) {
	return m_pSandbox->RegisterUpdateCallback(fnUpdateCallback);
}

RESULT DreamOS::UnregisterUpdateCallback() {
	return m_pSandbox->UnregisterUpdateCallback();
}

RESULT DreamOS::SetSandboxConfiguration(SandboxApp::configuration sandboxconf) {
	return m_pSandbox->SetSandboxConfiguration(sandboxconf);
}

const SandboxApp::configuration& DreamOS::GetSandboxConfiguration() {
	return m_pSandbox->GetSandboxConfiguration();
}

std::wstring DreamOS::GetHardwareID() {
	return m_pSandbox->GetHardwareID();
}

std::string DreamOS::GetHMDTypeString() {
	return m_pSandbox->GetHMDTypeString();
}

// Physics Engine
RESULT DreamOS::RegisterObjectCollision(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	r = m_pSandbox->RegisterObjectAndSubscriber(pVirtualObject, this);
	CR(r);

Error:
	return r;
}

RESULT DreamOS::RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	CR(m_pSandbox->RegisterEventSubscriber(eventType, pInteractionSubscriber));

Error:
	return r;
}

RESULT DreamOS::RegisterEventSubscriber(VirtualObj* pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	CR(m_pSandbox->RegisterEventSubscriber(pObject, eventType, pInteractionSubscriber));

Error:
	return r;
}

RESULT DreamOS::UnregisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	return m_pSandbox->UnregisterInteractionObject(pObject, eventType, pInteractionSubscriber);
}

RESULT DreamOS::UnregisterInteractionSubscriber(Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	return m_pSandbox->UnregisterInteractionSubscriber(pInteractionSubscriber);
}

RESULT DreamOS::UnregisterInteractionObject(VirtualObj *pObject) {
	return m_pSandbox->UnregisterInteractionObject(pObject);
}

RESULT DreamOS::RemoveObjectFromInteractionGraph(VirtualObj *pObject) {
	return m_pSandbox->RemoveObjectFromInteractionGraph(pObject);
}

RESULT DreamOS::AddAndRegisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	CR(AddObjectToInteractionGraph(pObject));

	CR(m_pSandbox->RegisterEventSubscriber(pObject, eventType, pInteractionSubscriber));

Error:
	return r;
}

RESULT DreamOS::AddObjectToUIGraph(VirtualObj *pObject) {
	return m_pSandbox->AddObjectToUIGraph(pObject);
}

RESULT DreamOS::AddObjectToUIClippingGraph(VirtualObj *pObject) {
	return m_pSandbox->AddObjectToUIClippingGraph(pObject);
}

RESULT DreamOS::RemoveObjectFromUIGraph(VirtualObj *pObject) {
	return m_pSandbox->RemoveObjectFromUIGraph(pObject);
}

RESULT DreamOS::RemoveObjectFromUIClippingGraph(VirtualObj *pObject) {
	return m_pSandbox->RemoveObjectFromUIClippingGraph(pObject);
}

// Cloud Controller

RESULT DreamOS::RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver) {
	return m_pSandbox->RegisterPeerConnectionObserver(pPeerConnectionObserver);
}

RESULT DreamOS::RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver) {
	return m_pSandbox->RegisterEnvironmentObserver(pEnvironmentObserver);
}

RESULT DreamOS::RegisterUserObserver(CloudController::UserObserver *pUserObserver) {
	return m_pSandbox->RegisterUserObserver(pUserObserver);
}

RESULT DreamOS::BroadcastVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	return m_pSandbox->BroadcastVideoFrame(pVideoFrameBuffer, pxWidth, pxHeight, channels);
}

RESULT DreamOS::SendDataMessage(long userID, Message *pDataMessage) {
	return m_pSandbox->SendDataMessage(userID, pDataMessage);
}

RESULT DreamOS::BroadcastDataMessage(Message *pDataMessage) {
	return m_pSandbox->BroadcastDataMessage(pDataMessage);
}

RESULT DreamOS::BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage, DreamAppMessage::flags messageFlags) {
	return m_pSandbox->BroadcastDreamAppMessage(pDreamAppMessage, messageFlags);
}

RESULT DreamOS::RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber) {
	return m_pSandbox->RegisterSubscriber(keyEvent, pKeyboardSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber) {
	return m_pSandbox->RegisterSubscriber(typingEvent, pTypingSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber) {
	return m_pSandbox->RegisterSubscriber(mouseEvent, pMouseSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber) {
	return m_pSandbox->RegisterSubscriber(controllerEvent, pControllerSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseGamepadEventType gamePadEvent, Subscriber<SenseGamepadEvent>* pGamepadSubscriber) {
	return m_pSandbox->RegisterSubscriber(gamePadEvent, pGamepadSubscriber);
}

RESULT DreamOS::GetCredential(std::wstring wstrKey, std::string &strOut, CredentialManager::type credType) {
	return m_pSandbox->GetKeyValue(wstrKey, strOut, credType);
}

RESULT DreamOS::SaveCredential(std::wstring wstrKey, std::string strValue, CredentialManager::type credType, bool fOverwrite) {
	return m_pSandbox->SetKeyValue(wstrKey, strValue, credType, fOverwrite);
}

RESULT DreamOS::RemoveCredential(std::wstring wstrKey, CredentialManager::type credType) {
	return m_pSandbox->RemoveKeyValue(wstrKey, credType);
}

bool DreamOS::IsSandboxInternetConnectionValid() {
	return m_pSandbox->IsSandboxInternetConnectionValid();
}

long DreamOS::GetTickCount() {
	return m_pSandbox->GetTickCount();
}

RESULT DreamOS::RegisterVideoStreamSubscriber(PeerConnection *pVideoSteamPeerConnectionSource, DreamVideoStreamSubscriber *pVideoStreamSubscriber) {
	RESULT r = R_PASS;

	CN(pVideoStreamSubscriber);
	CBM((m_pVideoStreamSubscriber == nullptr), "Video Steam Subscriber is already set");

	m_pVideoSteamPeerConnectionSource = pVideoSteamPeerConnectionSource;
	m_pVideoStreamSubscriber = pVideoStreamSubscriber;

Error:
	return r;
}

RESULT DreamOS::UnregisterVideoStreamSubscriber(DreamVideoStreamSubscriber *pVideoStreamSubscriber) {
	RESULT r = R_PASS;

	CN(pVideoStreamSubscriber);
	CBM((m_pVideoStreamSubscriber == pVideoStreamSubscriber), "Video Steam Subscriber is not set to this object");

	m_pVideoStreamSubscriber = nullptr;

Error:
	return r;
}

bool DreamOS::IsRegisteredVideoStreamSubscriber(DreamVideoStreamSubscriber *pVideoStreamSubscriber) {
	return (m_pVideoStreamSubscriber == pVideoStreamSubscriber);
}

RESULT DreamOS::OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_NOT_HANDLED;

	if (m_pVideoStreamSubscriber != nullptr && pPeerConnection == m_pVideoSteamPeerConnectionSource) {
		CR(m_pVideoStreamSubscriber->OnVideoFrame(pPeerConnection, pVideoFrameDataBuffer, pxWidth, pxHeight));
	}

Error:
	return r;
}

bool DreamOS::IsSharing() {
	return m_pDreamShareView->IsStreaming();
}

texture* DreamOS::GetSharedContentTexture() {

	if (m_pDreamShareView != nullptr) {
		return m_pDreamShareView->GetCastingTexture();
	}
	
	return nullptr;
}

RESULT DreamOS::SetSharedContentTexture(texture* pSharedTexture) {
	if (m_pDreamShareView != nullptr) {
		m_pDreamShareView->SetCastingTexture(pSharedTexture);
	}

	return R_PASS;
}

RESULT DreamOS::BroadcastSharedVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CN(m_pDreamShareView);
	m_pDreamShareView->BroadcastVideoFrame(pVideoFrameBuffer, pxWidth, pxHeight);

Error:
	return r;
}

RESULT DreamOS::BroadcastSharedAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pDreamShareView);
	CR(m_pDreamShareView->BroadcastAudioPacket(pendingAudioPacket));

Error:
	return r;
}
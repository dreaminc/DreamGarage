#include "DreamOS.h"

#include "Logger/Logger.h"
#include "DreamAppManager.h"

#include "Primitives/font.h"

#include "Cloud/Environment/PeerConnection.h"
#include "DreamMessage.h"

// Messages
#include "PeerHandshakeMessage.h"
#include "PeerAckMessage.h"
#include "PeerStayAliveMessage.h"

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

	// Initialize logger
	Logger::InitializeLogger();

	// Create the Sandbox
	m_pSandbox = SandboxFactory::MakeSandbox(CORE_CONFIG_SANDBOX_PLATFORM);
	CNM(m_pSandbox, "Failed to create sandbox");
	CVM(m_pSandbox, "Sandbox is Invalid!");
	CRM(m_pSandbox->SetDreamOSHandle(this), "Failed to set DreamOS handle");

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
		
		LOG(INFO) << "Dream runs from web";

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
		LOG(INFO) << "Dream runs from EXE";

		// Initialize the sandbox
		CRM(m_pSandbox->Initialize(argc, argv), "Failed to initialize Sandbox");
	}

	// Cloud Controller
	CRM(RegisterPeerConnectionObserver(this), "Failed to register Peer Connection Observer");
	CRM(RegisterEnvironmentObserver(this), "Failed to register environment controller");

	// Give the Client a chance to set up the pipeline
	CRM(SetupPipeline(GetRenderPipeline()), "Failed to set up pipeline");

	// Load the scene
	CRM(LoadScene(), "Failed to load scene");

	//m_pKeyboard = LaunchDreamApp<UIKeyboard>(this);
	//CN(m_pKeyboard);

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
	LOG(INFO) << "DataString: " << strDataChannelMessage;

Error:
	return r;
}

RESULT DreamOS::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	/*
	long index = (fOfferor) ? pPeerConnection->GetOfferorPosition() : pPeerConnection->GetAnswererPosition();
	index -= 1;

	if (m_fSeated) {
		LOG(INFO) << "HandlePeersUpdate already seated" << index;
		return R_PASS;
	}

	LOG(INFO) << "HandlePeersUpdate " << index;
	OVERLAY_DEBUG_SET("seat", (std::string("seat=") + std::to_string(index)).c_str());

	if (!m_fSeated) {
		CBM((index < m_seatLookup.size()), "Peer index %d not supported by client", index);
		CR(SetRoundtablePosition(index));
		m_fSeated = true;
	}
	*/

	//CR(OnNewDreamPeer(pPeerConnection));

	// Create a new peer
	auto pDreamPeer = CreateNewPeer(pPeerConnection);
	CN(pDreamPeer);

	{
		// Initialize handshake - only add user when peer connection stabilized 
		PeerHandshakeMessage peerHandshakeMessage(userID, peerUserID);
		CR(SendDataMessage(peerUserID, &peerHandshakeMessage));

		pDreamPeer->SentHandshakeRequest();
	}
	
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
		// TODO: App messages
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

		if (pDreamPeer->IsPeerReady()) {
			int a = 5;
		}
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

			if (pDreamPeer->IsPeerReady()) {
				int a = 5;
			}
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

std::shared_ptr<DreamPeer> DreamOS::CreateNewPeer(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;
	std::shared_ptr<DreamPeer> pDreamPeer = nullptr;

	long peerUserID = pPeerConnection->GetPeerUserID();
	CBM((m_dreamPeers.find(peerUserID) == m_dreamPeers.end()), "Error: Peer user ID %d already exists", peerUserID);

	pDreamPeer = std::make_shared<DreamPeer>(this, pPeerConnection);
	CN(pDreamPeer);

	CR(pDreamPeer->Initialize());

	// Set map
	m_dreamPeers[peerUserID] = pDreamPeer;

	return pDreamPeer;

Error:
	if (pDreamPeer != nullptr) {
		pDreamPeer = nullptr;
	}

	return nullptr;
}

std::shared_ptr<DreamPeer> DreamOS::FindPeer(PeerConnection *pPeerConnection) {
	return FindPeer(pPeerConnection->GetPeerUserID());
}

std::shared_ptr<DreamPeer> DreamOS::FindPeer(long peerUserID) {
	std::map<long, std::shared_ptr<DreamPeer>>::iterator it;

	if ((it = m_dreamPeers.find(peerUserID)) != m_dreamPeers.end()) {
		return (*it).second;
	}

	return nullptr;
}

RESULT DreamOS::RemovePeer(long peerUserID) {
	std::map<long, std::shared_ptr<DreamPeer>>::iterator it;

	if ((it = m_dreamPeers.find(peerUserID)) != m_dreamPeers.end()) {
		m_dreamPeers.erase(it);
		return R_PASS;
	}

	return R_NOT_FOUND;
}

RESULT DreamOS::RemovePeer(std::shared_ptr<DreamPeer> pDreamPeer) {
	return RemovePeer(pDreamPeer->GetPeerUserID());
}

DreamPeer::state DreamOS::GetPeerState(long peerUserID) {
	std::shared_ptr<DreamPeer> pDreamPeer = nullptr;

	if ((pDreamPeer = FindPeer(peerUserID)) != nullptr) {
		return pDreamPeer->GetState();
	}

	return DreamPeer::state::INVALID;
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

hand *DreamOS::GetHand(hand::HAND_TYPE handType) {
	return m_pSandbox->GetHand(handType);
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
	return r;
}

RESULT DreamOS::Exit(RESULT exitcode) {
	DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", m_versionDreamOS.GetString().c_str(), exitcode);
	return exitcode;
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

RESULT DreamOS::SetHALConfiguration(HALImp::HALConfiguration halconf) {
	return m_pSandbox->SetHALConfiguration(halconf);
}

const HALImp::HALConfiguration& DreamOS::GetHALConfiguration() {
	return m_pSandbox->GetHALConfiguration();
}

CloudController *DreamOS::GetCloudController() {
	return m_pSandbox->m_pCloudController;
}

long DreamOS::GetUserID() {
	return m_pSandbox->m_pCloudController->GetUserID();
}

ControllerProxy* DreamOS::GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE controllerType) {
	return GetCloudController()->GetControllerProxy(controllerType);
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

// This is a pass-thru at the moment
RESULT DreamOS::AddPhysicsObject(VirtualObj *pObject) {
	return m_pSandbox->AddPhysicsObject(pObject);
}

// This is a pass-thru at the moment
RESULT DreamOS::AddObjectToInteractionGraph(VirtualObj *pObject) {
	return m_pSandbox->AddObjectToInteractionGraph(pObject);
}

RESULT DreamOS::AddInteractionObject(VirtualObj *pObject) {
	return m_pSandbox->AddInteractionObject(pObject);
}

RESULT DreamOS::CaptureObject(VirtualObj *pObject, VirtualObj *pInteractionObject, point ptContact, vector vDirection, float threshold) {
	return m_pSandbox->CaptureObject(pObject, pInteractionObject, ptContact, vDirection, threshold);
}

RESULT DreamOS::ReleaseObjects(VirtualObj *pInteractionObject) {
	return m_pSandbox->ReleaseObjects(pInteractionObject);
}

/*
RESULT DreamOS::UpdateInteractionPrimitive(const ray &rCast) {
	return m_pSandbox->UpdateInteractionPrimitive(rCast);
}
*/

RESULT DreamOS::SetGravityAcceleration(double acceleration) {
	return m_pSandbox->SetGravityAcceleration(acceleration);
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

DimRay* DreamOS::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	return m_pSandbox->AddRay(ptOrigin, vDirection, step, fDirectional);
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

std::shared_ptr<font> DreamOS::MakeFont(std::wstring wstrFontFileName, bool fDistanceMap ) {
	RESULT r = R_PASS;
	
	// First check font store
	std::shared_ptr<font> pFont = GetFont(wstrFontFileName);

	if (pFont == nullptr) {
		pFont = std::make_shared<font>(wstrFontFileName, fDistanceMap);
		CN(pFont);

		{
			std::wstring strFile = L"Fonts/" + pFont->GetFontImageFile();
			const wchar_t* pszFile = strFile.c_str();

			CR(pFont->SetTexture(std::shared_ptr<texture>(MakeTexture(const_cast<wchar_t*>(pszFile), texture::TEXTURE_TYPE::TEXTURE_COLOR))));
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

texture* DreamOS::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pSandbox->MakeTexture(pszFilename, type);
}

texture *DreamOS::MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type) {
	return m_pSandbox->MakeTextureFromFileBuffer(pBuffer, pBuffer_n, type);
}

texture* DreamOS::MakeTexture(const texture &srcTexture) {
	return m_pSandbox->MakeTexture(srcTexture);
}

texture* DreamOS::MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n) {
	return m_pSandbox->MakeTexture(type, width, height, format, channels, pBuffer, pBuffer_n);
}

skybox *DreamOS::AddSkybox() {
	return m_pSandbox->AddSkybox();
}

skybox *DreamOS::MakeSkybox() {
	return m_pSandbox->MakeSkybox();
}

model *DreamOS::AddModel(wchar_t *pszModelName) {
	return m_pSandbox->AddModel(pszModelName);
}

model *DreamOS::MakeModel(wchar_t *pszModelName) {
	return m_pSandbox->AddModel(pszModelName);
}
	
composite *DreamOS::AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
	return m_pSandbox->AddModel(wstrOBJFilename, pTexture, ptPosition, scale, vEulerRotation);
}

composite *DreamOS::AddComposite() {
	return m_pSandbox->AddComposite();
}

composite *DreamOS::MakeComposite() {
	return m_pSandbox->MakeComposite();
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

std::shared_ptr<UIKeyboard> DreamOS::GetKeyboard() {
	return m_pKeyboard;
}

// Physics Engine
RESULT DreamOS::RegisterObjectCollision(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	r = m_pSandbox->RegisterObjectAndSubscriber(pVirtualObject, this);
	CR(r);

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

RESULT DreamOS::UnregisterInteractionObject(VirtualObj *pObject) {
	return m_pSandbox->UnregisterInteractionObject(pObject);
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

// Cloud Controller

RESULT DreamOS::RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver) {
	return m_pSandbox->RegisterPeerConnectionObserver(pPeerConnectionObserver);
}

RESULT DreamOS::RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver) {
	return m_pSandbox->RegisterEnvironmentObserver(pEnvironmentObserver);
}

RESULT DreamOS::SendDataMessage(long userID, Message *pDataMessage) {
	return m_pSandbox->SendDataMessage(userID, pDataMessage);
}

RESULT DreamOS::BroadcastDataMessage(Message *pDataMessage) {
	return m_pSandbox->BroadcastDataMessage(pDataMessage);
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

long DreamOS::GetTickCount() {
	return m_pSandbox->GetTickCount();
}
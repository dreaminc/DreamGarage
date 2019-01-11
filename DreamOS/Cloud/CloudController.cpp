#include "CloudController.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Sandbox/CommandLineManager.h"

#include "Cloud/Message/Message.h"
//#include "Cloud/Message/UpdateHandMessage.h"
//#include "Cloud/Message/UpdateHeadMessage.h"
//#include "Cloud/Message/AudioDataMessage.h"

#include "User/User.h"
#include "User/TwilioNTSInformation.h"

#include "ControllerProxy.h"

#include <chrono>
#include <thread>

#include "Cloud/Environment/PeerConnection.h"

#if (defined(_WIN32) || defined(_WIN64))
#include "Sandbox/Windows/Win32Helper.h"
#else
#endif

CloudController::CloudController() :
	Controller(nullptr, this),
	m_pCloudImp(nullptr),
	m_pUserController(nullptr),
	m_pEnvironmentController(nullptr),
	m_fnHandleDataChannelStringMessageCallback(nullptr),
	m_fnHandleDataChannelMessageCallback(nullptr)
{
	// empty
}

CloudController::~CloudController() {
	// TODO: Maybe this should not be a singleton
	HTTPController *pHTTPController = HTTPController::instance();

	if (pHTTPController != nullptr) {
		pHTTPController->Stop();
	}
}

RESULT CloudController::CloudThreadProcess() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("ProcessingThread start");

	// TODO: This should be removed
	if (m_fLoginOnStart) {
		CR(Login());
	}

	m_fRunning = true;

	// TODO: WAT
	// Message pump goes here
#if (defined(_WIN32) || defined(_WIN64))
	Win32Helper::ThreadBlockingMessageLoop();
#else
#pragma message ("not implemented message loop")
	while (m_fRunning) {
	
	}
#endif

	DEBUG_LINEOUT("CloudThreadProcess End");

Error:
	return r;
}

RESULT CloudController::CloudThreadProcessParams(std::string strUsername, std::string strPassword, long environmentID) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CloudThreadProcess start");

	//TODO: this code is going to have to be changed further to have the correct login flow
	if (m_fLoginOnStart) {
		DEBUG_LINEOUT("Logging into server with user credentials");

		CN(m_pUserController);
		CRM(m_pUserController->Login(strUsername, strPassword), "Failed to login");

		DEBUG_LINEOUT("Loading user profile");

		// Get user profile
		CRM(m_pUserController->LoadProfile(), "Failed to load profile");
		CRM(m_pUserController->LoadTwilioNTSInformation(), "Failed to load Twilio NTS information");

		// Set this in the cloud implementation
		m_pEnvironmentController->SetTwilioNTSInformation(m_pUserController->GetTwilioNTSInformation());
		m_pEnvironmentController->SetUser(m_pUserController->GetUser());

		DEBUG_LINEOUT("Connecting to Environment ID %d", environmentID);

		// Connect to environment 
		CN(m_pEnvironmentController);
		CR(m_pEnvironmentController->ConnectToEnvironmentSocket(m_pUserController->GetUser(), environmentID));
	}

	m_fRunning = true;

	// Message pump goes here
#if (defined(_WIN32) || defined(_WIN64))
	Win32Helper::ThreadBlockingMessageLoop();
#else
#pragma message ("not implemented message loop")
	while (m_fRunning) {

	}
#endif

	DEBUG_LINEOUT("CloudThreadProcess End");

Error:
	return r;
}

RESULT CloudController::Start(bool fLoginOnStart) {
	RESULT r = R_PASS;

	CBM((m_fRunning == false), "Error: Cloud controller trying to start but already running");

	DEBUG_LINEOUT("CloudController::Start");

	m_fRunning = true;
	m_fLoginOnStart = fLoginOnStart;

	//m_cloudThread = std::thread(&CloudController::CloudThreadProcess, this);

Error:
	return r;
}

RESULT CloudController::Start(std::string strUsername, std::string strPassword, long environmentID) {
	RESULT r = R_PASS;

	CBM((m_fRunning == false), "Error: Cloud controller trying to start but already running");

	DEBUG_LINEOUT("CloudController::Start");

	m_fRunning = true;
	m_fLoginOnStart = true;

	//m_cloudThread = std::thread(&CloudController::CloudThreadProcessParams, this, strUsername, strPassword, environmentID);

Error:
	return r;
}

RESULT CloudController::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CloudController::Stop");

	m_fRunning = false;

	m_pEnvironmentController->m_pPeerConnectionController->m_pWebRTCImp->Shutdown();

#if (defined(_WIN32) || defined(_WIN64))
	Win32Helper::PostQuitMessage(m_cloudThread);
#else
#pragma message ("not implemented post quit to thread")
	while (m_fRunning) {

	}
#endif

	if (m_cloudThread.joinable()) {
		m_cloudThread.join();
	}

//Error:
	return r;
}

RESULT CloudController::RegisterDataChannelStringMessageCallback(HandleDataChannelStringMessageCallback fnHandleDataChannelStringMessageCallback) {
	if (m_fnHandleDataChannelStringMessageCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleDataChannelStringMessageCallback = fnHandleDataChannelStringMessageCallback;
		return R_PASS;
	}
}

RESULT CloudController::RegisterDataChannelMessageCallback(HandleDataChannelMessageCallback fnHandleDataChannelMessageCallback) {
	if (m_fnHandleDataChannelMessageCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleDataChannelMessageCallback = fnHandleDataChannelMessageCallback;
		return R_PASS;
	}
}

RESULT CloudController::SetCloudImp(std::unique_ptr<CloudImp> pCloudImp) {
	RESULT r = R_PASS;

	m_pCloudImp = std::move(pCloudImp);
	CN(m_pCloudImp);

Error:
	return r;
}

RESULT CloudController::Initialize() {
	RESULT r = R_PASS;

	CR(InitializeUser());
	CR(InitializeEnvironment(-1));

Error:
	return r;
}

RESULT CloudController::InitializeUser(version ver) {
	RESULT r = R_PASS;

	m_pUserController = std::unique_ptr<UserController>(UserFactory::MakeUserController(ver, this));
	CN(m_pUserController);
	m_pUserController->Initialize();
	m_pUserController->RegisterUserControllerObserver(this);

Error:
	return r;
}

RESULT CloudController::PrintEnvironmentPeerList() {
	RESULT r = R_PASS;

	CNM(m_pEnvironmentController, "Environment not set up bro!");
	CR(m_pEnvironmentController->PrintEnvironmentPeerList());

Error:
	return r;
}

/*
RESULT CloudController::AddIceCandidates() {
	return m_pCloudImp->AddIceCandidates();
}
*/

RESULT CloudController::OnICECandidatesGatheringDone() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("ICE Gathering Complete");
	CNM(m_pEnvironmentController, "Environment not set up bro!");

	///*
	if (m_pEnvironmentController->GetState() == EnvironmentController::state::ENVIRONMENT_CONNECTED_AND_READY) {
		DEBUG_LINEOUT("Updating the SDP offer");
		CR(m_pEnvironmentController->UpdateEnvironmentUser());
	}
	else {
		DEBUG_LINEOUT("Environment not ready - need to connect to server bro!");
	}
	//*/

Error:
	return r;
}

RESULT CloudController::OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) {
	RESULT r = R_PASS;

	CN(m_fnHandleDataChannelStringMessageCallback);
	CR(m_fnHandleDataChannelStringMessageCallback(pPeerConnection, strDataChannelMessage));

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnDataStringMessage(pPeerConnection, strDataChannelMessage));
	}

Error:
	return r;
}

RESULT CloudController::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnNewPeerConnection(userID, peerUserID, fOfferor, pPeerConnection));
	}

Error:
	return r;
}

RESULT CloudController::OnNewSocketConnection(int seatPosition) {
	RESULT r = R_PASS;

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnNewSocketConnection(seatPosition));
	}

Error:
	return r;
}

//RESULT 

RESULT CloudController::OnPeerConnectionClosed(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnPeerConnectionClosed(pPeerConnection));
	}

Error:
	return r;
}

RESULT CloudController::OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	if (m_fnHandleDataChannelMessageCallback != nullptr) {
		CR(m_fnHandleDataChannelMessageCallback(pPeerConnection, pDataChannelBuffer, pDataChannelBuffer_n));
	}

	Message *pDataChannelMessage = reinterpret_cast<Message*>(pDataChannelBuffer);
	CN(pDataChannelMessage);

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnDataMessage(pPeerConnection, pDataChannelMessage));
	}

	/*
	// TODO: Move this into DreamGarage
	switch (pDataChannelMessage->GetType()) {
		case Message::MessageType::MESSAGE_UPDATE_HEAD: {
			if (m_pPeerConnectionObserver != nullptr) {
				UpdateHeadMessage *pUpdateHeadMessage = reinterpret_cast<UpdateHeadMessage*>(pDataChannelBuffer);
				CN(pUpdateHeadMessage);
				// TODO: Add peer ID from
				CR(m_pPeerConnectionObserver->OnHeadUpdateMessage(peerUserID, pUpdateHeadMessage));
			}
		} break;

		case Message::MessageType::MESSAGE_UPDATE_HAND: {
			if (m_pPeerConnectionObserver != nullptr) {
				UpdateHandMessage *pUpdateHandMessage = reinterpret_cast<UpdateHandMessage*>(pDataChannelBuffer);
				CN(pUpdateHandMessage);
				// TODO: Add peer ID from
				CR(m_pPeerConnectionObserver->OnHandUpdateMessage(peerUserID, pUpdateHandMessage));
			}
		} break;

		default: {
			
		} break;
	}
	*/

Error:
	return r;
}

RESULT CloudController::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmnetAsset) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnEnvironmentAsset(pEnvironmnetAsset));
	}

Error:
	return r;
}

RESULT CloudController::OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnReceiveAsset(pEnvironmentShare));
	}

Error:
	return r;
}

RESULT CloudController::OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnStopSending(pEnvironmentShare));
	}

Error:
	return r;
}

RESULT CloudController::OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnShareAsset(pEnvironmentShare));
	}

Error:
	return r;
}

RESULT CloudController::OnCloseAsset() {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnCloseAsset());
	}

Error:
	return r;
}

RESULT CloudController::OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnStopReceiving(pEnvironmentShare));
	}

Error:
	return r;
}

RESULT CloudController::OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnOpenCamera(pEnvironmentAsset));
	}

Error:
	return r;
}

RESULT CloudController::OnCloseCamera() {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnCloseCamera());
	}

Error:
	return r;
}

RESULT CloudController::OnSendCameraPlacement() {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnSendCameraPlacement());
	}

Error:
	return r;
}

RESULT CloudController::OnStopSendingCameraPlacement() {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnStopSendingCameraPlacement());
	}

Error:
	return r;
}

RESULT CloudController::OnReceiveCameraPlacement(long userID) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnReceiveCameraPlacement(userID));
	}

Error:
	return r;
}

RESULT CloudController::OnStopReceivingCameraPlacement() {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnStopReceivingCameraPlacement());
	}

Error:
	return r;
}

RESULT CloudController::OnDataChannel(PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnDataChannel(pPeerConnection));
	}

Error:
	return r;
}

RESULT CloudController::OnAudioChannel(PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnAudioChannel(pPeerConnection));
	}

Error:
	return r;
}

RESULT CloudController::OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) {
	RESULT r = R_PASS;

	if (m_pEnvironmentObserver != nullptr) {
		CR(m_pEnvironmentObserver->OnGetForm(strKey, strTitle, strURL));
	}

Error:
	return r;
}

RESULT CloudController::OnAPIConnectionCheck(bool fIsConnected) {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnAPIConnectionCheck(fIsConnected));
	}

Error:
	return r;
}

RESULT CloudController::OnDreamVersion(version dreamVersion) {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnDreamVersion(dreamVersion));
	}

Error:
	return r;
}

RESULT CloudController::OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnGetSettings(ptPosition, qOrientation, fIsSet));
	}

Error:
	return r;
}

RESULT CloudController::OnSetSettings() {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnSetSettings());
	}

Error:
	return r;
}

RESULT CloudController::OnLogin() {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnLogin());
	}

Error:
	return r;
}

RESULT CloudController::OnLogout() {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnLogout());
	}

Error:
	return r;
}

RESULT CloudController::OnPendLogout() {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnPendLogout());
	}

Error:
	return r;
}

RESULT CloudController::OnSwitchTeams() {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnSwitchTeams());
	}

Error:
	return r;
}

RESULT CloudController::OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnFormURL(strKey, strTitle, strURL));
	}

Error:
	return r;
}

RESULT CloudController::OnAccessToken(bool fSuccess, std::string& strAccessToken) {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnAccessToken(fSuccess, strAccessToken));
	}

Error:
	return r;
}

RESULT CloudController::OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) {
	RESULT r = R_PASS;

	if (m_pUserObserver != nullptr) {
		CR(m_pUserObserver->OnGetTeam(fSuccess, environmentId, environmentModelId));
	}

Error:
	return r;
}

RESULT CloudController::OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	long senderUserID = pPeerConnection->GetPeerUserID();
	long recieverUserID = pPeerConnection->GetUserID();

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnVideoFrame(strVideoTrackLabel, pPeerConnection, pVideoFrameDataBuffer, pxWidth, pxHeight));
	}

Error:
	return r;
}

RESULT CloudController::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames)  {
	RESULT r = R_PASS;

	long senderUserID = pPeerConnection->GetPeerUserID();
	long recieverUserID = pPeerConnection->GetUserID();

	if (m_pPeerConnectionObserver != nullptr) {
		CR(m_pPeerConnectionObserver->OnAudioData(strAudioTrackLabel, pPeerConnection, pAudioDataBuffer, bitsPerSample, samplingRate, channels, frames));
	}

Error:
	return r;
}

RESULT CloudController::InitializeEnvironment(long environmentID) {
	RESULT r = R_PASS;

	//CBM((environmentID != -1), "Environment cannot be -1");

	m_pEnvironmentController = std::unique_ptr<EnvironmentController>(new EnvironmentController(this, environmentID));
	CN(m_pEnvironmentController);

	CR(m_pEnvironmentController->Initialize());
	CR(m_pEnvironmentController->RegisterEnvironmentControllerObserver(this));

Error:
	return r;
}

RESULT CloudController::CreateNewURLRequest(std::wstring& strURL) {
	RESULT r = R_PASS;

	CNM(m_pCloudImp, "Cloud Imp not initialized");
	CRM(m_pCloudImp->CreateNewURLRequest(strURL), "Failed to create CEF URL request for %S", strURL.c_str());

Error:
	return r;
}

/*
RESULT CloudController::CreateSDPOfferAnswer(std::string strSDPOfferJSON) {
	RESULT r = R_PASS;

	CN(m_pCloudImp);
	CR(m_pCloudImp->CreateSDPOfferAnswer(strSDPOfferJSON));

Error:
	return r;
}
*/

RESULT CloudController::Login() {
	RESULT r = R_PASS;

	CommandLineManager *pCommandLineManager = CommandLineManager::instance();

	std::string strUsername = pCommandLineManager->GetParameterValue("username");
	std::string strPassword = pCommandLineManager->GetParameterValue("password");
	std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");

	CR(LoginUser(strUsername, strPassword, strOTK));

Error:
	return r;
}

User CloudController::GetUser() {
	return m_pUserController->GetUser();
}

TwilioNTSInformation CloudController::GetTwilioNTSInformation() {
	return m_pUserController->GetTwilioNTSInformation();
}

bool CloudController::IsUserLoggedIn() {
	return m_pUserController->IsLoggedIn();
}

bool CloudController::IsEnvironmentConnected() {
	return m_pEnvironmentController->IsEnvironmentSocketConnected();
}

RESULT CloudController::LoginUser(std::string strUsername, std::string strPassword, std::string strOTK) {
	RESULT r = R_PASS;

	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	
	//std::string strURI = pCommandLineManager->GetParameterValue("api.ip");
	
	std::string strEnvironment = pCommandLineManager->GetParameterValue("environment");
	long environmentID;

	if (strOTK == "INVALIDONETIMEKEY") {
		DEBUG_LINEOUT(("Login user " + strUsername + "...").c_str());
		DEBUG_LINEOUT(("Login ip " + pCommandLineManager->GetParameterValue("api.ip") + "...").c_str());

		// TODO: command line / config file - right now hard coded
		CN(m_pUserController);
		CRM(m_pUserController->Login(strUsername, strPassword), "Failed to login");
	}
	else {
		// TODO: If OTK provided log in with that instead
		DEBUG_LINEOUT(("Login with OTK " + strOTK + "...").c_str());
		DEBUG_LINEOUT(("Login ip " + pCommandLineManager->GetParameterValue("api.ip") + "...").c_str());

		// TODO: We should move to this, but right now this is passed by cmd line
		long tempEnvironmentID;
		CN(m_pUserController);
		CRM(m_pUserController->LoginWithOTK(strOTK, tempEnvironmentID), "Failed to login with OTK");
		//strEnvironment = std::to_string(environmentID);
	}

	DEBUG_LINEOUT("Loading user profile...");

	// Get user profile
	// TODO: This should go into an API controller
	CRM(m_pUserController->LoadProfile(), "Failed to load profile");
	CRM(m_pUserController->LoadTwilioNTSInformation(), "Failed to load Twilio NTS information");

	// Set this in the cloud implementation
	m_pEnvironmentController->SetTwilioNTSInformation(m_pUserController->GetTwilioNTSInformation());
	m_pEnvironmentController->SetUser(m_pUserController->GetUser());

	// Set up environment
	//CR(InitializeEnvironment(m_pUserController->GetUserDefaultEnvironmentID()));
	if (strEnvironment == "default") {
		environmentID = m_pUserController->GetUserDefaultEnvironmentID();
	}
	else {
		environmentID = (long)(atoi(strEnvironment.c_str()));
	}

	DEBUG_LINEOUT("Connecting to Environment ID %d", environmentID);

	// Connect to environment 
	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->ConnectToEnvironmentSocket(m_pUserController->GetUser(), environmentID));

	DEBUG_LINEOUT("User is loaded and logged in");

Error:
	return r;
}

RESULT CloudController::Update() {
	RESULT r = R_PASS;

	CR(m_pCloudImp->Update());

Error:
	return r;
}

long CloudController::GetUserID() {
	if (m_pUserController != nullptr) {
		return m_pUserController->GetUserID();
	}

	return -1;
}

long CloudController::GetUserAvatarID() {
	if (m_pUserController != nullptr) {
		return m_pUserController->GetUserAvatarModelID();
	}
	return -1;
}

/*
RESULT CloudController::InitializeConnection(bool fMaster, bool fAddDataChannel) {
	RESULT r = R_PASS;

	// Test: attempt to establish SDB before socket - maybe issues there
	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->InitializeNewPeerConnection(fMaster, fAddDataChannel));

Error:
	return r;
}
*/

/*
std::string CloudController::GetSDPOfferString() {
	return m_pCloudImp->GetSDPOfferString();
}
*/

RESULT CloudController::SendDataChannelStringMessage(int peerID, std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->SendDataChannelStringMessage(peerID, strMessage));

Error:
	return r;
}

RESULT CloudController::SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->SendDataChannelMessage(peerID, pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT CloudController::BroadcastDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->BroadcastDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT CloudController::BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->BroadcastDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

// TODO: remove this code
void CloudController::CallGetUIThreadCallback(int msgID, void* data) {
	std::function<void(int msg_id, void* data)> fnUIThreadCallback;
	return fnUIThreadCallback(msgID, data);
}

// Send some messages
// TODO: This is duplicated code - use this in the below functions
RESULT CloudController::SendDataMessage(long userID, Message *pDataMessage) {
	RESULT r = R_PASS;

	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	CB(m_fRunning);

	// TODO: Fix this - remove m_pCloudImp
	//CB(m_pCloudImp->IsConnected());
	CB(m_pEnvironmentController->IsUserIDConnected(userID));
	CN(m_pUserController);
	{
		// Create the message
		pDatachannelBuffer_n = pDataMessage->GetSize();
		pDatachannelBuffer = new uint8_t[pDatachannelBuffer_n];
		CN(pDatachannelBuffer);
		memcpy(pDatachannelBuffer, pDataMessage, pDataMessage->GetSize());
		CR(SendDataChannelMessage(userID, pDatachannelBuffer, pDatachannelBuffer_n));
	}

Error:
	return r;
}



// Broadcast some messages
// TODO: This is duplicated code - use this in the below functions
RESULT CloudController::BroadcastDataMessage(Message *pDataMessage) {
	RESULT r = R_PASS;

	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	CBR(m_fRunning, R_SKIPPED);

	CN(m_pUserController);
	{
		// Create the message
		pDatachannelBuffer_n = pDataMessage->GetSize();
		pDatachannelBuffer = new uint8_t[pDatachannelBuffer_n];
		CN(pDatachannelBuffer);
		memcpy(pDatachannelBuffer, pDataMessage, pDataMessage->GetSize());

		CR(BroadcastDataChannelMessage(pDatachannelBuffer, pDatachannelBuffer_n));
	}

Error:
	return r;
}

// Audio 
RESULT CloudController::BroadcastAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CBR(m_fRunning, R_SKIPPED);	// Because otherwise this clogs the logs

	CN(m_pEnvironmentController);
	CN(m_pEnvironmentController->BroadcastAudioPacket(strAudioTrackLabel, pendingAudioPacket));

Error:
	return r;
}

float CloudController::GetRunTimeMicAverage() {
	// TODO: Fix this
	//if (m_pEnvironmentController != nullptr) {
	//	return m_pEnvironmentController->GetRunTimeMicAverage();
	//}

	return m_runTimeMicAverage;
}


RESULT CloudController::SetRunTimeMicAverage(float runTimeMicAverage) {
	m_runTimeMicAverage = runTimeMicAverage;
	return R_PASS;
}

// Video
RESULT CloudController::BroadcastVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	CB(m_fRunning);

	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->BroadcastVideoFrame(strVideoTrackLabel, pVideoFrameBuffer, pxWidth, pxHeight, channels));

Error:
	return r;
}

RESULT CloudController::BroadcastTextureFrame(const std::string &strVideoTrackLabel, texture *pTexture, int level, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	CB(m_fRunning);

	CN(m_pEnvironmentController);
	CN(pTexture);

	CR(pTexture->LoadImageFromTexture(0, pixelFormat));

	// Broadcast the data
	CR(m_pEnvironmentController->BroadcastVideoFrame(
		strVideoTrackLabel, pTexture->GetImageBuffer(), pTexture->GetWidth(), pTexture->GetHeight(), pTexture->GetChannels()
	));

Error:
	return r;
}

RESULT CloudController::StartVideoStreaming(const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	CB(m_fRunning);
	CN(m_pEnvironmentController);

	CR(m_pEnvironmentController->StartVideoStreaming(strVideoTrackLabel, pxDesiredWidth, pxDesiredHeight, desiredFPS, pixelFormat));

Error:
	return r;
}

RESULT CloudController::StopVideoStreaming(const std::string &strVideoTrackLabel) {
	RESULT r = R_PASS;

	CB(m_fRunning);
	CN(m_pEnvironmentController);

	CR(m_pEnvironmentController->StopVideoStreaming(strVideoTrackLabel));

Error:
	return r;
}

bool CloudController::IsVideoStreamingRunning(const std::string &strVideoTrackLabel) {
	RESULT r = R_PASS;

	CB(m_fRunning);
	CN(m_pEnvironmentController);

	return m_pEnvironmentController->IsVideoStreamingRunning(strVideoTrackLabel);

Error:
	return false;
}

// TODO: Fix inconsistency with proxy pattern (webrtc is correct, proxy shouldn't include register observer for example)
// TODO: Fill out pattern for other controllers
// TODO: Replace with polymorphic fns
ControllerProxy* CloudController::GetControllerProxy(CLOUD_CONTROLLER_TYPE controllerType) {
	ControllerProxy *pProxy = nullptr;

	switch (controllerType) {
		case CLOUD_CONTROLLER_TYPE::CLOUD: {
			pProxy = (ControllerProxy*)(this);
		} break;

		case CLOUD_CONTROLLER_TYPE::WEBRTC: {
			pProxy = (ControllerProxy*)(GetWebRTCControllerProxy());
		} break;

		case CLOUD_CONTROLLER_TYPE::MENU: {
			pProxy = (ControllerProxy*)(GetMenuControllerProxy());
		} break;

		case CLOUD_CONTROLLER_TYPE::HTTP: {
			pProxy = (ControllerProxy*)(GetHTTPControllerProxy());
		} break;

		case CLOUD_CONTROLLER_TYPE::ENVIRONMENT: {
			pProxy = (ControllerProxy*)(GetEnvironmentControllerProxy());
		} break;

		case CLOUD_CONTROLLER_TYPE::USER: {
			pProxy = (ControllerProxy*)(GetUserControllerProxy());
		} break;
	}

	return pProxy;
}

RESULT CloudController::RegisterControllerObserver(CLOUD_CONTROLLER_TYPE controllerType, ControllerObserver *pControllerObserver) {
	RESULT r = R_PASS;

	ControllerProxy *pProxy = nullptr;

	switch (controllerType) {
		case CLOUD_CONTROLLER_TYPE::MENU: {
			pProxy = (ControllerProxy*)(GetMenuControllerProxy());
			CN(pProxy);
			CR(pProxy->RegisterControllerObserver(pControllerObserver));
		} break;
	}

Error:
	return r;
}

RESULT CloudController::RegisterPeerConnectionObserver(PeerConnectionObserver* pPeerConnectionObserver) {
	RESULT r = R_PASS;

	CNM((pPeerConnectionObserver), "Observer cannot be nullptr");
	CBM((m_pPeerConnectionObserver == nullptr), "Can't overwrite peer connection observer");
	m_pPeerConnectionObserver = pPeerConnectionObserver;

Error:
	return r;
}

RESULT CloudController::RegisterEnvironmentObserver(EnvironmentObserver* pEnvironmentObserver) {
	RESULT r = R_PASS;

	CNM((pEnvironmentObserver), "Observer cannot be nullptr");
	CBM((m_pEnvironmentObserver == nullptr), "Can't overwrite environment observer");
	m_pEnvironmentObserver = pEnvironmentObserver;

Error:
	return r;
}

RESULT CloudController::RegisterUserObserver(UserObserver* pUserObserver) {
	RESULT r = R_PASS;

	CNM((pUserObserver), "Observer cannot be nullptr");
	CBM((m_pUserObserver == nullptr), "Can't overwrite environment observer");
	m_pUserObserver = pUserObserver;

Error:
	return r;
}

// TODO: Replace all of these with polymorphic fns 
UserControllerProxy* CloudController::GetUserControllerProxy() {
	if (m_pUserController != nullptr)
		return m_pUserController->GetUserControllerProxy();

	return nullptr;
}

WebRTCImpProxy* CloudController::GetWebRTCControllerProxy() {
	if (m_pEnvironmentController != nullptr) {
		return m_pEnvironmentController->GetWebRTCControllerProxy();
	}

	return nullptr;
}

MenuControllerProxy* CloudController::GetMenuControllerProxy() {
	if(m_pEnvironmentController != nullptr)
		return m_pEnvironmentController->GetMenuControllerProxy();

	return nullptr;
}

HTTPControllerProxy* CloudController::GetHTTPControllerProxy() {
	HTTPController *pHTTPController = HTTPController::instance();
	return pHTTPController->GetHTTPControllerProxy();
}

EnvironmentControllerProxy* CloudController::GetEnvironmentControllerProxy() {
	if(m_pEnvironmentController != nullptr)
		return m_pEnvironmentController->GetEnvironmentControllerProxy();

	return nullptr;
}
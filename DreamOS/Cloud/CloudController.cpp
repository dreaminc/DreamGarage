#include "CloudController.h"
#include "Logger/Logger.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Sandbox/CommandLineManager.h"

#include "Cloud/Message/Message.h"
#include "Cloud/Message/UpdateHandMessage.h"
#include "Cloud/Message/UpdateHeadMessage.h"
#include "Cloud/Message/AudioDataMessage.h"

#include "DreamConsole/DreamConsole.h"

#include "User/User.h"
#include "User/TwilioNTSInformation.h"

#include "ControllerProxy.h"

#include <chrono>
#include <thread>

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
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::CloudController, this);
}

CloudController::~CloudController() {
	// TODO: Maybe this should not be a singleton
	HTTPController *pHTTPController = HTTPController::instance();

	if (pHTTPController != nullptr) {
		pHTTPController->Stop();
	}
}

RESULT CloudController::ProcessingThread() {
	RESULT r = R_PASS;

	LOG(INFO) << "ProcessingThread start";

	m_fRunning = true;

	CR(Initialize());

	//std::this_thread::sleep_for(std::chrono::seconds(3));

	CR(Login());

	// Message pump goes here
#if (defined(_WIN32) || defined(_WIN64))
	Win32Helper::ThreadBlockingMessageLoop();
#else
#pragma message ("not implemented message loop")
	while (m_fRunning) {

	}
#endif

	LOG(INFO) << "ProcessingThread end";

Error:
	return r;
}

RESULT CloudController::Start() {
	RESULT r = R_PASS;

	if (m_fRunning) {
		// cloud already running
		HUD_OUT("cloud trying to start but already running");
		return R_FAIL;
	}

	DEBUG_LINEOUT("CloudController::Start");

	m_thread = std::thread(&CloudController::ProcessingThread, this);

//Error:
	return r;
}

RESULT CloudController::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CloudController::Stop");

	HUD_OUT("login into Relativity ...");

	m_fRunning = false;

#if (defined(_WIN32) || defined(_WIN64))
	Win32Helper::PostQuitMessage(m_thread);
#else
#pragma message ("not implemented post quit to thread")
	while (m_fRunning) {

	}
#endif

	if (m_thread.joinable()) {
		m_thread.join();
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

RESULT CloudController::RegisterPeersUpdateCallback(HandlePeersUpdateCallback fnHandlePeersUpdateCallback) {
	if (m_fnHandlePeersUpdateCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandlePeersUpdateCallback = fnHandlePeersUpdateCallback;
		return R_PASS;
	}
}

RESULT CloudController::RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback) {
	if (m_fnHandleDataMessageCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleDataMessageCallback = fnHandleDataMessageCallback;
		return R_PASS;
	}
}

RESULT CloudController::RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback) {
	if (m_fnHandleHeadUpdateMessageCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleHeadUpdateMessageCallback = fnHandleHeadUpdateMessageCallback;
		return R_PASS;
	}
}

RESULT CloudController::RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback) {
	if (m_fnHandleHandUpdateMessageCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleHandUpdateMessageCallback = fnHandleHandUpdateMessageCallback;
		return R_PASS;
	}
}

RESULT CloudController::RegisterAudioDataCallback(HandleAudioDataCallback fnHandleAudioDataCallback) {
	if (m_fnHandleAudioDataCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleAudioDataCallback = fnHandleAudioDataCallback;
		return R_PASS;
	}
}

RESULT CloudController::RegisterEnvironmentAssetCallback(HandleEnvironmentAssetCallback fnHandleEnvironmentAssetCallback) {
	if (m_fnHandleEnvironmentAssetCallback) {
		return R_FAIL;
	}
	else {
		m_fnHandleEnvironmentAssetCallback = fnHandleEnvironmentAssetCallback;
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

RESULT CloudController::OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) {
	RESULT r = R_PASS;

	CN(m_fnHandleDataChannelStringMessageCallback);
	CR(m_fnHandleDataChannelStringMessageCallback(peerConnectionID, strDataChannelMessage));

Error:
	return r;
}

RESULT CloudController::OnPeersUpdate(long index) {
	RESULT r = R_PASS;

	if (m_fnHandlePeersUpdateCallback != nullptr) {
		CR(m_fnHandlePeersUpdateCallback(index));
	}

Error:
	return r;
}

RESULT CloudController::OnDataChannelMessage(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	if (m_fnHandleDataChannelMessageCallback != nullptr) {
		CR(m_fnHandleDataChannelMessageCallback(peerUserID, pDataChannelBuffer, pDataChannelBuffer_n));
	}

	Message *pDataChannelMessage = reinterpret_cast<Message*>(pDataChannelBuffer);
	CN(pDataChannelMessage);

	switch (pDataChannelMessage->GetType()) {
		case Message::MessageType::MESSAGE_UPDATE_HEAD: {
			if (m_fnHandleHeadUpdateMessageCallback != nullptr) {
				UpdateHeadMessage *pUpdateHeadMessage = reinterpret_cast<UpdateHeadMessage*>(pDataChannelBuffer);
				CN(pUpdateHeadMessage);
				// TODO: Add peer ID from
				CR(m_fnHandleHeadUpdateMessageCallback(peerUserID, pUpdateHeadMessage));
			}
		} break;

		case Message::MessageType::MESSAGE_UPDATE_HAND: {
			if (m_fnHandleHandUpdateMessageCallback != nullptr) {
				UpdateHandMessage *pUpdateHandMessage = reinterpret_cast<UpdateHandMessage*>(pDataChannelBuffer);
				CN(pUpdateHandMessage);
				// TODO: Add peer ID from
				CR(m_fnHandleHandUpdateMessageCallback(peerUserID, pUpdateHandMessage));
			}
		} break;

		default: {
			if (m_fnHandleDataMessageCallback != nullptr) {
				// TODO: Add peer ID from
				CR(m_fnHandleDataMessageCallback(peerUserID, pDataChannelMessage));
			}
		} break;
	}

Error:
	return r;
}

RESULT CloudController::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmnetAsset) {
	RESULT r = R_PASS;

	if (m_fnHandleEnvironmentAssetCallback != nullptr) {
		CR(m_fnHandleEnvironmentAssetCallback(pEnvironmnetAsset));
	}

Error:
	return r;
}

RESULT CloudController::OnAudioData(long peerConnectionID,
	const void* audio_data,
	int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames) 
{
	RESULT r = R_PASS;

	if (m_fnHandleAudioDataCallback != nullptr) {
		AudioDataMessage audioDataMessage(peerConnectionID,
			peerConnectionID,
			audio_data,
			bits_per_sample,
			sample_rate,
			number_of_channels,
			number_of_frames);

		CR(m_fnHandleAudioDataCallback(peerConnectionID, &audioDataMessage));
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
		HUD_OUT(("Login user " + strUsername + "...").c_str());
		HUD_OUT(("Login ip " + pCommandLineManager->GetParameterValue("api.ip") + "...").c_str());

		// TODO: command line / config file - right now hard coded
		CN(m_pUserController);
		CRM(m_pUserController->Login(strUsername, strPassword), "Failed to login");
	}
	else {
		// TODO: If OTK provided log in with that instead
		HUD_OUT(("Login with OTK " + strOTK + "...").c_str());
		HUD_OUT(("Login ip " + pCommandLineManager->GetParameterValue("api.ip") + "...").c_str());

		CN(m_pUserController);
		CRM(m_pUserController->LoginWithOTK(strOTK), "Failed to login with OTK");
	}

	HUD_OUT("Loading user profile...");

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

	HUD_OUT("Connecting to Environment ID %d", environmentID);

	// Connect to environment 
	CN(m_pEnvironmentController);
	CR(m_pEnvironmentController->ConnectToEnvironmentSocket(m_pUserController->GetUser(), environmentID));

	HUD_OUT("User is loaded and logged in");

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

RESULT CloudController::SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
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
		UpdateHeadMessage updateHeadMessage(m_pUserController->GetUserID(), userID, ptPosition, qOrientation, vVelocity, qAngularVelocity);

		pDatachannelBuffer_n = sizeof(UpdateHeadMessage);
		pDatachannelBuffer = new uint8_t[pDatachannelBuffer_n];
		CN(pDatachannelBuffer);
		memcpy(pDatachannelBuffer, &updateHeadMessage, sizeof(UpdateHeadMessage));

		CR(SendDataChannelMessage(userID, pDatachannelBuffer, pDatachannelBuffer_n));
	}

Error:
	return r;
}

RESULT CloudController::SendUpdateHandMessage(long userID, hand::HandState handState) {
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
		UpdateHandMessage updateHeadMessage(m_pUserController->GetUserID(), userID, handState);

		pDatachannelBuffer_n = sizeof(UpdateHandMessage);
		pDatachannelBuffer = new uint8_t[pDatachannelBuffer_n];
		CN(pDatachannelBuffer);
		memcpy(pDatachannelBuffer, &updateHeadMessage, sizeof(UpdateHandMessage));

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

RESULT CloudController::BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	CN(m_pUserController);
	{
		// Create the message
		UpdateHeadMessage updateHeadMessage(m_pUserController->GetUserID(), -1, ptPosition, qOrientation, vVelocity, qAngularVelocity);

		pDatachannelBuffer_n = sizeof(UpdateHeadMessage);
		pDatachannelBuffer = new uint8_t[pDatachannelBuffer_n];
		CN(pDatachannelBuffer);
		memcpy(pDatachannelBuffer, &updateHeadMessage, sizeof(UpdateHeadMessage));

		CR(BroadcastDataChannelMessage(pDatachannelBuffer, pDatachannelBuffer_n));
	}

Error:
	return r;
}

RESULT CloudController::BroadcastUpdateHandMessage(hand::HandState handState) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	CN(m_pUserController);
	{
		// Create the message
		UpdateHandMessage updateHeadMessage(m_pUserController->GetUserID(), -1, handState);

		pDatachannelBuffer_n = sizeof(UpdateHandMessage);
		pDatachannelBuffer = new uint8_t[pDatachannelBuffer_n];
		CN(pDatachannelBuffer);
		memcpy(pDatachannelBuffer, &updateHeadMessage, sizeof(UpdateHandMessage));

		CR(BroadcastDataChannelMessage(pDatachannelBuffer, pDatachannelBuffer_n));
	}

Error:
	return r;
}

RESULT CloudController::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("list") == 0) {
		HUD_OUT("login : login to Dream");
		HUD_OUT("msg <msg> : broadcast a text msg, <msg>, to connected users");
	}

	if (event->GetArg(1).compare("login") == 0) {
		//
		Start();
	}

	if (event->GetArg(1).compare("msg") == 0) {
		std::string st(event->GetArg(2));
		BroadcastDataChannelStringMessage(st);
	}

//Error:
	return r;
}

// TODO: Fill out pattern for other controllers
ControllerProxy* CloudController::GetControllerProxy(CLOUD_CONTROLLER_TYPE controllerType) {
	ControllerProxy *pProxy = nullptr;

	switch (controllerType) {
		case CLOUD_CONTROLLER_TYPE::MENU: {
			pProxy = (ControllerProxy*)(GetMenuControllerProxy());
		} break;

		case CLOUD_CONTROLLER_TYPE::HTTP: {
			pProxy = (ControllerProxy*)(GetHTTPControllerProxy());
		} break;

		case CLOUD_CONTROLLER_TYPE::ENVIRONMENT: {
			pProxy = (ControllerProxy*)(GetEnvironmentControllerProxy());
		}
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
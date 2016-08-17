#include "CloudController.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Sandbox/CommandLineManager.h"

CloudController::CloudController() :
	m_pCloudImp(nullptr),
	m_pUserController(nullptr)
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

RESULT CloudController::SetCloudImp(std::unique_ptr<CloudImp> pCloudImp) {
	RESULT r = R_PASS;

	m_pCloudImp = std::move(pCloudImp);
	CN(m_pCloudImp);

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

RESULT CloudController::InitializeEnvironment(long environmentID) {
	RESULT r = R_PASS;

	CBM((environmentID != -1), "Environment cannot be -1");

	m_pEnvironmentController = std::unique_ptr<EnvironmentController>(new EnvironmentController(this, environmentID));
	CN(m_pEnvironmentController);

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

RESULT CloudController::AddICECandidateFromSDPOfferStringJSON(std::string strSDPOfferJSON) {
	RESULT r = R_PASS;

	CN(m_pCloudImp);
	CR(m_pCloudImp->AddICECandidateFromSDPOfferStringJSON(strSDPOfferJSON));

Error:
	return r;
}

RESULT CloudController::LoginUser() {
	RESULT r = R_PASS;

	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	int port = std::stoi(pCommandLineManager->GetParameterValue("port"));
	std::string strURI = pCommandLineManager->GetParameterValue("ip");
	std::string strUsername = pCommandLineManager->GetParameterValue("username");
	std::string strPassword = pCommandLineManager->GetParameterValue("password");

	// TODO: command line / config file - right now hard coded
	CN(m_pUserController);
	CRM(m_pUserController->Login(strUsername, strPassword), "Failed to login");

	// Get user profile
	CRM(m_pUserController->LoadProfile(), "Failed to load profile");

	// Set up environment
	CR(InitializeEnvironment(m_pUserController->GetUserDefaultEnvironmentID()));
	CN(m_pEnvironmentController);

	// Connect to environment 
	CR(m_pEnvironmentController->ConnectToEnvironmentSocket(m_pUserController->GetUser()));

Error:
	return r;
}

RESULT CloudController::Update() {
	RESULT r = R_PASS;

	CR(m_pCloudImp->Update());

Error:
	return r;
}


// TODO: Convert WebRTC Client code to server client code

// TODO: This will attempt to connect to the first peer in the list, should make more robust
// and expose the available peer list at the CloudController layer
RESULT CloudController::ConnectToPeer(int peerID) {
	RESULT r = R_PASS;

	CN(m_pCloudImp);
	CR(m_pCloudImp->ConnectToPeer(peerID));

Error:
	return r;
}

RESULT CloudController::InitializeConnection(bool fMaster, bool fAddDataChannel) {
	return m_pCloudImp->InitializeConnection(fMaster, fAddDataChannel);
}

std::string CloudController::GetSDPOfferString() {
	return m_pCloudImp->GetSDPOfferString();
}

RESULT CloudController::SendMessageToPeer(int peerID, std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pCloudImp);
	CR(m_pCloudImp->SendMessageToPeer(peerID, strMessage));

Error:
	return r;
}

std::function<void(int msgID, void* data)> CloudController::GetUIThreadCallback() {
	return m_pCloudImp->GetUIThreadCallback();
}

void CloudController::CallGetUIThreadCallback(int msgID, void* data) {
	std::function<void(int msg_id, void* data)> fnUIThreadCallback;
	return fnUIThreadCallback(msgID, data);
}
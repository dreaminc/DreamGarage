#include "CloudController.h"

#include "Cloud/HTTP/HTTPController.h"

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

	m_pUserController = std::unique_ptr<UserController>(UserFactory::MakeUserController(ver));
	CN(m_pUserController);

Error:
	return r;
}

RESULT CloudController::InitializeEnvironment(long environmentID) {
	RESULT r = R_PASS;

	CBM((environmentID != -1), "Environment cannot be -1");

	m_pEnvironmentController = std::unique_ptr<EnvironmentController>(new EnvironmentController(environmentID));
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

RESULT CloudController::LoginUser() {
	RESULT r = R_PASS;

	std::string strUsername = "dream@dreamos.com";
	std::string strPassword = "dreamy";

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
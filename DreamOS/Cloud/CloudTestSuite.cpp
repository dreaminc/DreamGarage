#include "CloudTestSuite.h"
#include "DreamOS.h"

#include "Sandbox/CommandLineManager.h"
#include "Cloud/Menu/MenuNode.h"
#include "Cloud/HTTP/HTTPController.h"

CloudTestSuite::CloudTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

CloudTestSuite::~CloudTestSuite() {
	// empty
}

RESULT CloudTestSuite::AddTests() {
	RESULT r = R_PASS;

	// TODO: Closed box testing (multi user/environment instances or cloud controllers if need be)
	//CR(AddTestMultiConnectTest());

	CR(AddTestConnectLogin());
	CR(AddTestMenuAPI());
	CR(AddTestDownloadFile());	// requires logged in

	// TODO: Add Websocket tests
	// TODO: Add HTTP / CURL tests


Error:
	return r;
}


RESULT CloudTestSuite::AddTestMultiConnectTest() {
	RESULT r = R_PASS;

	double sTestTime = 200.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCloudController);
		CN(pCommandLineManager);

		// For later
		m_pCloudController = pCloudController;

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			std::string strUsername = "jason_test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", "nightmare"));

			CRM(pCloudController->Start(), "Failed to start cloud controller");
		}

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service - this will hang for a while");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}


RESULT CloudTestSuite::AddTestDownloadFile() {
	RESULT r = R_PASS;

	double sTestTime = 20.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		//std::string strImagePlaceholderURI = "http://placehold.it/300.png/09f/fff";
		std::string strImagePlaceholderURI = "https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png";

		// Destination Path
		std::wstring strImageDest;
		PathManager* pPathManager = PathManager::instance();
		strImageDest = pPathManager->GetFilePath(PATH_VALUE_TYPE::PATH_DATA, L"testimg.png");

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		HTTPControllerProxy *pHTTPControllerProxy = nullptr;
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCloudController);
		CN(pCommandLineManager);

		// For later
		m_pCloudController = pCloudController;

		// Set up file request
		DEBUG_LINEOUT("Requesting File %s", strImagePlaceholderURI.c_str());
		pHTTPControllerProxy = (HTTPControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP));
		CNM(pHTTPControllerProxy, "Failed to get http controller proxy");

		CR(pHTTPControllerProxy->RequestFile(strImagePlaceholderURI, strImageDest));

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");

		/*
		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");
		*/

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Download File");
	pNewTest->SetTestDescription("Test downloading a file from arbitrary URL");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CloudTestSuite::AddTestConnectLogin() {
	RESULT r = R_PASS;

	double sTestTime = 2.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCloudController);
		CN(pCommandLineManager);

		// For later
		m_pCloudController = pCloudController;

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");

		// Log in 
		{
			std::string strUsername = pCommandLineManager->GetParameterValue("username");
			std::string strPassword = pCommandLineManager->GetParameterValue("password");
			std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");

			CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
		}

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CloudTestSuite::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	CR(pMenuNode->PrintMenuNode());

	if (pMenuNode->NumSubMenuNodes() > 0) {
		auto pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		std::string strScope = pMenuNode->GetSubMenuNodes()[0]->GetScope();
		std::string strPath = pMenuNode->GetSubMenuNodes()[0]->GetPath();

		CRM(pMenuControllerProxy->RequestSubMenu(strScope, strPath), "Failed to request sub menu");
	}

Error:
	return r;
}

RESULT CloudTestSuite::AddTestMenuAPI() {
	RESULT r = R_PASS;

	double sTestTime = 30.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		MenuControllerProxy *pMenuControllerProxy = nullptr;
		CN(pCloudController);
		CBM(pCloudController->IsUserLoggedIn(), "User not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket not connected");
		
		// Set up menu stuff
		DEBUG_LINEOUT("Requesting Menu");
		pMenuControllerProxy = (MenuControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		CRM(pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");
		CRM(pMenuControllerProxy->RequestSubMenu(), "Failed to request sub menu");

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Menu API");
	pNewTest->SetTestDescription("Test Menu API");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

CloudController *CloudTestSuite::GetCloudController() {
	return m_pDreamOS->GetCloudController();
}
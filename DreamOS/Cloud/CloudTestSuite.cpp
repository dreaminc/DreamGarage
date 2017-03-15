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

<<<<<<< HEAD
	//CR(AddTestMultiConnectTest());

	CR(AddTestDownloadFile());
=======
	CR(AddTestConnectLogin());

<<<<<<< HEAD
	//CR(AddTestDownloadFile());
>>>>>>> adding HTTPControllerProxy and some config stuff that's pretty useful
=======
	CR(AddTestDownloadFile());
>>>>>>> adding Data path to Dream (we might want to remove this eventually) and set everything up to get the file transfer set up

	// TODO: Add Websocket tests
	// TODO: Add HTTP / CURL tests

	CR(AddTestMenuAPI());

Error:
	return r;
}

<<<<<<< HEAD
RESULT CloudTestSuite::AddTestMultiConnectTest() {
	RESULT r = R_PASS;

	double sTestTime = 200.0f;
=======
RESULT CloudTestSuite::AddTestDownloadFile() {
	RESULT r = R_PASS;

	double sTestTime = 20.0f;
<<<<<<< HEAD
>>>>>>> adding HTTPRequestFileHandler and a new routes for file requests using CURL, also added the Oculus Lib debug stuff
=======
	std::string strImagePlaceholderURI = "http://placehold.it/300.png/09f/fff";

	// Dest
	std::wstring strImageDest;

	// Root folder
	PathManager* pPathManager = PathManager::instance();
	strImageDest = pPathManager->GetFilePath(PATH_VALUE_TYPE::PATH_DATA, L"testimg.png");
>>>>>>> adding Data path to Dream (we might want to remove this eventually) and set everything up to get the file transfer set up

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		HTTPControllerProxy *pHTTPControllerProxy = nullptr;
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCloudController);
		CN(pCommandLineManager);

<<<<<<< HEAD
		// For later
		m_pCloudController = pCloudController;

		//CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");

<<<<<<< HEAD
		// TODO: This way to start the cloud controller thread is not great
=======
		// Download file
>>>>>>> adding HTTPRequestFileHandler and a new routes for file requests using CURL, also added the Oculus Lib debug stuff
		{
			/*
			std::string strUsername = pCommandLineManager->GetParameterValue("username");
			std::string strPassword = pCommandLineManager->GetParameterValue("password");
			std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");
<<<<<<< HEAD
			*/

			std::string strUsername = "jason_test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", "nightmare"));

			CRM(pCloudController->Start(), "Failed to start cloud controller");
=======

			CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
			*/
>>>>>>> adding HTTPRequestFileHandler and a new routes for file requests using CURL, also added the Oculus Lib debug stuff
		}
=======
		// Set up file request
		DEBUG_LINEOUT("Requesting File %s", strImagePlaceholderURI.c_str());
		pHTTPControllerProxy = (HTTPControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP));
		CNM(pHTTPControllerProxy, "Failed to get http controller proxy");

		CR(pHTTPControllerProxy->RequestFile(strImagePlaceholderURI, strImageDest));
>>>>>>> adding Data path to Dream (we might want to remove this eventually) and set everything up to get the file transfer set up

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

<<<<<<< HEAD
		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");
=======
		/*
		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");
		*/
>>>>>>> adding HTTPRequestFileHandler and a new routes for file requests using CURL, also added the Oculus Lib debug stuff

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

<<<<<<< HEAD
	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service - this will hang for a while");
=======
	pNewTest->SetTestName("Test Download File");
	pNewTest->SetTestDescription("Test downloading a file from arbitrary URL");
>>>>>>> adding HTTPRequestFileHandler and a new routes for file requests using CURL, also added the Oculus Lib debug stuff
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CloudTestSuite::AddTestConnectLogin() {
	RESULT r = R_PASS;

	double sTestTime = 3.0f;

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
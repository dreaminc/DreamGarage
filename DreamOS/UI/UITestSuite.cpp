#include "UITestSuite.h"
#include "DreamOS.h"
#include "UI/UIMenuItem.h"
#include "DreamGarage/DreamUIBar.h"
#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "DreamGarage/DreamContentView.h"
#include "DreamGarage/DreamBrowser.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "Cloud/WebRequest.h"

#include "UI/UIKeyboard.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Sandbox/CommandLineManager.h"
#include "Cloud/Environment/EnvironmentAsset.h"
#include "Cloud/Menu/MenuNode.h"
#include "Cloud/HTTP/HTTPController.h"
#include "Cloud/WebRequest.h"

#include "Core/Utilities.h"

UITestSuite::UITestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	RESULT r = R_PASS;

	CN(m_pDreamUIBar);
	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UITestSuite::~UITestSuite() {
	// empty
}

RESULT UITestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestBrowserRequestWithMenuAPI());
	CR(AddTestBrowserRequest());

	CR(AddTestKeyboard());

	CR(AddTestBrowser());



//	CR(AddTestInteractionFauxUI());
//	CR(AddTestSharedContentView());

Error:
	return r;
}

RESULT UITestSuite::Initialize() {
	RESULT r = R_PASS;


	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	m_pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	point sceneOffset = point(90, -5, -25);
	float sceneScale = 0.1f;
	vector sceneDirection = vector(0.0f, 0.0f, 0.0f);

	/*
	m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\env.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);

	composite* pRiver = m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\river.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);

	m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\clouds.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
	//*/

	/*
	for (int i = 0; i < SenseControllerEventType::SENSE_CONTROLLER_INVALID; i++) {
		CR(m_pDreamOS->RegisterSubscriber((SenseControllerEventType)(i), this));
	}

	for (int i = 0; i < SenseMouseEventType::SENSE_MOUSE_INVALID; i++) {
		CR(m_pDreamOS->RegisterSubscriber((SenseMouseEventType)(i), this));
	}
	*/

//Error:
	return r;
}

RESULT UITestSuite::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	CR(pMenuNode->PrintMenuNode());

	if (pMenuNode->NumSubMenuNodes() > 0) {
		auto pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		for (auto &pSubMenuNode : pMenuNode->GetSubMenuNodes()) {
			std::string strScope = pSubMenuNode->GetScope();
			std::string strPath = pSubMenuNode->GetPath();
			std::string strTitle = pSubMenuNode->GetTitle();

			if (pSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
				if (strTitle == "Share" || 
					strTitle == "File" || 
					strTitle == "Google Drive" || 
					strTitle == "Golden" ||
					strTitle == "People") 
				{
					CRM(pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle), "Failed to request sub menu");
					return r;
				}
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {
				auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(pEnvironmentControllerProxy->RequestShareAsset(strScope, strPath, strTitle), "Failed to share environment asset");

				return r;
			}
		}
	}

Error:
	return r;
}

RESULT UITestSuite::HandleOnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	//https://api.develop.dreamos.com/environment-asset/{id}/file
	
	if (m_pDreamBrowser != nullptr) {
		//CR(m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset));
		//m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset);
		WebRequest webRequest;

		std::wstring strEnvironmentAssetURI = util::StringToWideString(pEnvironmentAsset->GetURI());

		webRequest.SetURL(strEnvironmentAssetURI);
		//webRequest.SetURL(L"http://www.youtube.com");

		auto pUserControllerProxy = dynamic_cast<UserControllerProxy*>(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
		CN(pUserControllerProxy);
		std::string strTokenValue = "Token " + pUserControllerProxy->GetUserToken();
		std::wstring wstrTokenValue = util::StringToWideString(strTokenValue);

		CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
		CR(webRequest.AddRequestHeader(L"Authorization", wstrTokenValue));
		//CR(webRequest.AddRequestHeader(L"Authorization", L"Tokenz"));

		// NOTE: this is kind of working, data is clearly being sent but there's
		// no real support for form/file etc yet
		// This is not yet needed
		// TODO: Break this out into a separate UI suite (Browser/CEF)
		//CR(webRequest.AddPostDataElement(L"post data element"));

		CR(m_pDreamBrowser->LoadRequest(webRequest));
	}
	

Error:
	return r;
}


RESULT UITestSuite::AddTestBrowserRequestWithMenuAPI() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::string strURL = "http://www.youtube.com";

		CN(m_pDreamOS);

		CR(SetupPipeline());

		// Create the Browser
		m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(m_pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		m_pDreamBrowser->SetDiagonalSize(10.0f);

		m_pDreamBrowser->SetPosition(point(0.0f, 1.0f, 0.0f));

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		MenuControllerProxy *pMenuControllerProxy = nullptr;
		CN(pContext);
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

		CR(pCloudController->RegisterEnvironmentAssetCallback(std::bind(&UITestSuite::HandleOnEnvironmentAsset, this, std::placeholders::_1)));

		
		Sleep(1000);

		// Set up menu stuff
		DEBUG_LINEOUT("Requesting Menu");
		pMenuControllerProxy = (MenuControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		CRM(pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");
		CRM(pMenuControllerProxy->RequestSubMenu("", "", "menu"), "Failed to request sub menu");

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(r);

	Error:
		return r;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS->GetCloudController());
	CN(pUITest);

	pUITest->SetTestName("Browser Request Test");
	pUITest->SetTestDescription("Basic test of browser working with a web request");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestBrowserRequest() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::string strURL = "http://www.youtube.com";

		WebRequest webRequest;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		// Create the Shared View App
		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(10.0f);

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		//pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
		//pDreamBrowser->SetURI(strURL);

		//webRequest.SetURL(L"http://httpbin.org/get");
		//CR(webRequest.SetURL(L"http://www.cnn.com"));
		//webRequest.SetURL(L"https://placehold.it/350x150");
		webRequest.SetURL(L"http://placehold.it/350x150/A00AAA/000000");

		CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
		CR(webRequest.AddRequestHeader(L"Authorization", L"Token "));

		// NOTE: this is kind of working, data is clearly being sent but there's
		// no real support for form/file etc yet
		// This is not yet needed
		// TODO: Break this out into a separate UI suite (Browser/CEF)
		//CR(webRequest.AddPostDataElement(L"post data element"));

		CR(pDreamBrowser->LoadRequest(webRequest));

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(r);

	Error:
		return r;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestName("Browser Request Test");
	pUITest->SetTestDescription("Basic test of browser working with a web request");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::SetupPipeline() {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());
	
	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT UITestSuite::AddTestBrowser() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;

		std::string strURL = "http://www.youtube.com";

		CN(m_pDreamOS);

		CR(SetupPipeline());

		//light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the Shared View App
		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(10.0f);

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		//pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
		pDreamBrowser->SetURI(strURL);

		/*
		{
			texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
			auto pComposite = m_pDreamOS->AddComposite();

			auto pQuad = pComposite->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(1.0f, 0.0f, 0.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pQuad->SetColorTexture(pColorTexture1);
		}
		*/

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(r);

	Error:
		return r;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestKeyboard() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		m_pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);
		m_pKeyboard->ShowKeyboard();

		CR(Initialize());

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(r);
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UITestSuite::AddTestSharedContentView() {
	RESULT r = R_PASS;

	double sTestTime = 30.0f;
	int nRepeats = 1;

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamContentView> pDreamContentView = nullptr;

		CN(m_pDreamOS);

		// Create the Shared View App
		pDreamContentView = m_pDreamOS->LaunchDreamApp<DreamContentView>(this);
		CNM(pDreamContentView, "Failed to create dream content view");

		// Set up the view
		pDreamContentView->SetParams(point(0.0f), 5.0f, DreamContentView::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		//pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
		pDreamContentView->SetScreenURI("https://static.dreamos.com/www/image/hero.387eddfc05dc.jpg");

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestInteractionFauxUI() {
	RESULT r = R_PASS;

	struct TestContext {
		DimRay *pRay = nullptr;
		composite *pComposite = nullptr;
	};

	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pChildComposite = nullptr;
		std::shared_ptr<composite> pChildItemComposite = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;

		pTestContext->pRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));
		CN(pTestContext->pRay);


		m_pSphere1 = m_pDreamOS->AddSphere(0.02f, 10, 10);
		m_pSphere2 = m_pDreamOS->AddSphere(0.02f, 10, 10);

		// Create Faux UI here

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		pTestContext->pComposite = pComposite;
		pComposite->InitializeOBB();

		//pComposite->SetMass(1.0f);

		// Layer
		pChildComposite = pComposite->AddComposite();
		CN(pChildComposite);
		CR(pChildComposite->InitializeOBB());

		// Quads
		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(20.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(10.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
		//pChildItemComposite->RotateYByDeg(0.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(1.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(-10.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(2.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(-20.0f);

		// Move Composite
		pChildComposite->SetPosition(point(0.0f, 0.0f, -4.0f));

		pComposite->SetPosition(point(0.0f, 1.5f, 6.0f));

		// Add composite to interaction
		//CR(m_pDreamOS->AddInteractionObject(pComposite));

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		hand* pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

		if (pRightHand != nullptr && pTestContext->pRay != nullptr) {
			pTestContext->pRay->SetPosition(pRightHand->GetPosition());
			pTestContext->pRay->SetOrientation(pRightHand->GetHandState().qOrientation);

			point p0 = pRightHand->GetPosition();
			//GetLookVector
			quaternion q = pRightHand->GetHandState().qOrientation;
			q.Normalize();

			vector v = q.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();
			vector v2 = vector(-v.x(), -v.y(), v.z());
			vector vHandLook = RotationMatrix(q) * vector(0.0f, 0.0f, -1.0f);

			ray rcast = ray(p0, vHandLook);

			CollisionManifold manifold = pTestContext->pComposite->Collide(rcast);

			if (manifold.NumContacts() > 0) {
				int numContacts = manifold.NumContacts();

				if (numContacts > 2)
					numContacts = 2;
				for (int i = 0; i < numContacts; i++) {
					sphere *pSphere = (i == 0) ? m_pSphere1 : m_pSphere2;

					if (pSphere != nullptr) {
						pSphere->SetVisible(true);
						pSphere->SetPosition(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}

			//m_pDreamOS->UpdateInteractionPrimitive(rcast);

		}
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("UI Faux Interaction Engine Test");
	pUITest->SetTestDescription("UI Basic Testing Environment");
	pUITest->SetTestDuration(10000.0);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UITestSuite::Notify(SenseControllerEvent *event) {
	RESULT r = R_PASS;

	SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;
	OVERLAY_DEBUG_SET("event", "none");

	if (event->state.type == CONTROLLER_RIGHT) {
		if (eventType == SENSE_CONTROLLER_TRIGGER_MOVE) {
			OVERLAY_DEBUG_SET("event", "trigger move");
		}
		else if (eventType == SENSE_CONTROLLER_PAD_MOVE) {
			OVERLAY_DEBUG_SET("event", "pad move");
		}

		else if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
			OVERLAY_DEBUG_SET("event", "trigger down");
		}

		// TODO:  soon this code will be replaced with api requests, 
		// as opposed to accessing the hard coded local data structures
		else if (eventType == SENSE_CONTROLLER_TRIGGER_UP) {
			OVERLAY_DEBUG_SET("event", "trigger up");
//			CR(m_pDreamUIBar->HandleSelect());
		}
		else if (eventType == SENSE_CONTROLLER_MENU_UP) {
			OVERLAY_DEBUG_SET("event", "menu up");
//			CR(m_pDreamUIBar->HandleMenuUp());
		}
	}
	else if (eventType == SENSE_CONTROLLER_GRIP_DOWN) {
		OVERLAY_DEBUG_SET("event", "grip down");
	}
	else if (eventType == SENSE_CONTROLLER_GRIP_UP) {
		OVERLAY_DEBUG_SET("event", "grip up");
	}
	else if (eventType == SENSE_CONTROLLER_MENU_DOWN) {
		OVERLAY_DEBUG_SET("event", "menu down");
	}
//Error:
	return r;
}

RESULT UITestSuite::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;
	CR(r);
Error:
	return r;
}

RESULT UITestSuite::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

//Error:
	return r;
}


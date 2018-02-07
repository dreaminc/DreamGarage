#include "DreamOSTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/UIStageProgram.h"

#include "DreamTestingApp.h"
#include "DreamUserApp.h"
#include "UI\UIKeyboard.h"
#include "DreamGarage\DreamUIBar.h"
#include "DreamControlView\DreamControlView.h"
#include "DreamShareView\DreamShareView.h"

#include "DreamGarage\DreamBrowser.h"
#include "DreamGarage\Dream2DMouseApp.h"

#include <chrono>

DreamOSTestSuite::DreamOSTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

DreamOSTestSuite::~DreamOSTestSuite() {
	// empty
}

RESULT DreamOSTestSuite::AddTests() {
	RESULT r = R_PASS;
	
	// Casting tests

	CR(AddTestDreamShareView());

	CR(AddTestDreamOS());

	CR(AddTestUserApp());	

	CR(AddTestDreamBrowser());

	CR(AddTestCaptureApp());

	CR(AddTestDreamApps());

	CR(AddTestUIKeyboard());

	CR(AddTestDreamUIBar());

	CR(AddTestCaptureApp());


Error:
	return r;
}

RESULT DreamOSTestSuite::SetupPipeline(std::string strRenderProgramName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderProgramName);
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	
	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));	

	//TODO: Matrix node
	//	CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

	// Connect output as pass-thru to internal blend program
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);
	
	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT DreamOSTestSuite::SetupDreamAppPipeline() {
	RESULT r = R_PASS;
	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	//CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Connect output as pass-thru to internal blend program
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//*/
	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

	// Screen Quad Shader (opt - we could replace this if we need to)
	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);

	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	// Connect Program to Display
	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamUIBar() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());
		{
			auto pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this);
			//CN(pDreamUIBar);	// still fails because it needs a user
		}

	Error:
		return r;
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

RESULT DreamOSTestSuite::AddTestDreamBrowser() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<Dream2DMouseApp> pDream2DMouse = nullptr;

		//std::string strURL = "http://www.youtube.com";

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		std::string strURL = "http://ncu.rcnpv.com.tw/Uploads/20131231103232738561744.pdf";

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the 2D Mouse App
		pDream2DMouse = m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		CNM(pDream2DMouse, "Failed to create dream 2D mouse app");

		// Create the Shared View App
		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(10.0f);

		pDreamBrowser->SetURI(strURL);

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

RESULT DreamOSTestSuite::AddTestUIKeyboard() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext : public Subscriber<SenseControllerEvent> {
		std::shared_ptr<UIKeyboard> pKeyboard = nullptr;

		virtual RESULT Notify(SenseControllerEvent *event) override {
			RESULT r = R_PASS;
			if (event->type == SENSE_CONTROLLER_MENU_UP) {
				//hardcoded values taken from DreamUIBar
				CR(pKeyboard->UpdateComposite(-0.30f));
			}
		Error:
			return r;
		}
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(Initialize());
		CR(SetupDreamAppPipeline());

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		pTestContext->pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);
		pTestContext->pKeyboard->ShowKeyboard();
		CR(m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_MENU_UP, pTestContext));

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamApps() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;
	//const int numTests = 5;

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamTestingApp> pDreamTestApps[5];// = { nullptr };

		CN(m_pDreamOS);

		CR(SetupPipeline());

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the testing apps
		for (int i = 0; i < 5; i++) {
			pDreamTestApps[i] = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this);
			CNM(pDreamTestApps[i], "Failed to create dream test app");
			pDreamTestApps[i]->SetTestingValue(i);
		}

		// Set up the view
		//pDreamTestApp->SetParams(point(0.0f), 5.0f, DreamTestApp::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		//pDreamTestApp->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
		//pDreamContentView->SetScreenURI("https://static.dreamos.com/www/image/hero.387eddfc05dc.jpg");

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

		// TODO: Kill apps

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

RESULT DreamOSTestSuite::AddTestCaptureApp() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	struct TestContext {

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		//CN(m_pDreamOS);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);

		CR(SetupPipeline());

		{
			std::vector<UID> pTestUIDs = m_pDreamOS->GetAppUID("UIKeyboard");
			CB(pTestUIDs.size() == 1);
			UID keyboardUID = pTestUIDs[0];

			auto pTestApp1 = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this).get();
			auto pTestHandle = m_pDreamOS->CaptureApp(keyboardUID, pTestApp1);
			CB(pTestHandle != nullptr);

			auto pTestApp2 = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this).get();
			auto pTestHandleFail = m_pDreamOS->CaptureApp(keyboardUID, pTestApp2);
			CB(pTestHandleFail == nullptr);

			auto pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(pTestHandle);

			CR(pKeyboardHandle->Show());

			CR(m_pDreamOS->ReleaseApp(pKeyboardHandle, keyboardUID, pTestApp1));

			CB(pKeyboardHandle->Show() == R_FAIL);

			pTestHandle = m_pDreamOS->CaptureApp(keyboardUID, pTestApp1);
			CB(pTestHandle != nullptr);
			CR(m_pDreamOS->ReleaseApp(pTestHandle, keyboardUID, pTestApp1));
		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

		// TODO: Kill apps as needed 

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestUserApp() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	struct TestContext : public Subscriber<InteractionObjectEvent> {
		user *pUser = nullptr;
		sphere *pSphere = nullptr;
		std::shared_ptr<DreamPeerApp> m_pPeers[4] = { nullptr };
		std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;
		DimRay *pMouseRay = nullptr;

		virtual RESULT Notify(InteractionObjectEvent *mEvent) override {
			RESULT r = R_PASS;

			CR(r);

			DEBUG_LINEOUT("stuff");

			if(mEvent->m_numContacts > 0)
				pSphere->SetPosition(mEvent->m_ptContact[0]);

		Error:
			return r;
		}

	} *pTestContext = new TestContext();

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// TODO:
		std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the Shared View App
		pTestContext->pDreamUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		CNM(pTestContext->pDreamUserApp, "Failed to create dream user app");

		// Add some users
		for (int i = 0; i < 1; i++) {
			pTestContext->m_pPeers[i] = m_pDreamOS->LaunchDreamApp<DreamPeerApp>(this);
			CNM(pTestContext->m_pPeers[i], "Failed to create dream peer app");
			
			auto pUserModel = m_pDreamOS->MakeUser();
			CN(pUserModel);
			pTestContext->m_pPeers[i]->AssignUserModel(pUserModel);
		
			pTestContext->m_pPeers[i]->SetPosition(point(-1.0f + (i * 1.0f), 0.0f, 2.0f));
			pTestContext->m_pPeers[i]->RotateByDeg(0.0f, 45.0f, 0.0f);
		}


		// Sphere test
		//pTestContext->pSphere = m_pDreamOS->AddSphere(0.025f, 10, 10);
		//CN(pTestContext->pSphere);
		//m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pSphere);

		// User test
		//pTestContext->pUser = m_pDreamOS->AddUser();
		//CN(pTestContext->pUser);
		//m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pUser);

		//// Mouse Ray
		//pTestContext->pMouseRay = m_pDreamOS->AddRay(point(-0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f).Normal());
		//CN(pTestContext->pMouseRay);
		//m_pDreamOS->AddInteractionObject(pTestContext->pMouseRay);

		//CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pUser, ELEMENT_INTERSECT_BEGAN, pTestContext));
		//CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pUser, ELEMENT_INTERSECT_ENDED, pTestContext));

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		ray rCast;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
		//pTestContext->pMouseRay->UpdateFromRay(rCast);

		pTestContext->m_pPeers[0]->RotateByDeg(0.1f, 0.0f, 0.0f);

	Error:
		return r;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

		// TODO: Kill apps as needed 

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// A test that includes all the basic UI apps in a functional state.
// User, ControlView, Keyboard, Browser, UIBar
RESULT DreamOSTestSuite::AddTestDreamOS() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext {
		std::shared_ptr<DreamUserApp> pUser = nullptr;
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamControlView> pDreamControlView = nullptr;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<DreamUIBar> pDreamUIBar = nullptr;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);
	
		CR(SetupDreamAppPipeline());
		{
			auto pCloudController = m_pDreamOS->GetCloudController();
			auto pCommandLineManager = CommandLineManager::instance();
			DEBUG_LINEOUT("Initializing Cloud Controller");
			quad *pQuad = nullptr;
			CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");
			{
				std::string strUsername = pCommandLineManager->GetParameterValue("username");
				std::string strPassword = pCommandLineManager->GetParameterValue("password");
				std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");
				long environmentID = 168;

				CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
				CRM(pCloudController->Start(false), "Failed to Start Cloud Controller");

			}
		}
		pDreamControlView = m_pDreamOS->LaunchDreamApp<DreamControlView>(this, false);
		CN(pDreamControlView);

		// UIKeyboard App
		CR(m_pDreamOS->InitializeKeyboard());
		pTestContext->pUser = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		CN(pTestContext->pUser);

		CR(pTestContext->pUser->SetHand(m_pDreamOS->GetHand(HAND_TYPE::HAND_LEFT)));
		CR(pTestContext->pUser->SetHand(m_pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT)));

		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(9.0f);
		pDreamBrowser->SetPosition(point(0.0f, 2.0f, -2.0f));

		pDreamBrowser->SetVisible(false);

		pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this, false);
		CN(pDreamUIBar);
		CR(pDreamUIBar->SetUIStageProgram(m_pUIProgramNode));

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
	
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Full test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamShareView() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

	struct TestTimingContext {
		double m_msStart;
		std::shared_ptr<DreamShareView> pDreamShareView;
	};

	TestTimingContext *pTestContext = new TestTimingContext();
	pTestContext->m_msStart = msNow;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());
		{
			std::shared_ptr<DreamShareView> pDreamShareView = nullptr;

			auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);
			pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this);
			pDreamShareView->Show();

			auto pCastTexture = m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			pDreamShareView->SetCastingTexture(std::shared_ptr<texture>(pCastTexture));

			pTestContext->pDreamShareView = pDreamShareView;
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

		auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);

		//auto pDreamShareViewHandle = dynamic_cast<DreamShareViewHandle*>(m_pDreamOS->RequestCaptureAppUnique("DreamShareView", this));
		
		double diff = msNow - pTestContext->m_msStart;
		int mod = ((int)diff / 500) % 2;

		if (mod == 0) {
			pTestContext->pDreamShareView->ShowLoadingTexture();
		}
		else {
			pTestContext->pDreamShareView->SendCastingEvent();
		}

		//m_pDreamOS->RequestReleaseAppUnique(pDreamShareViewHandle, this);
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}
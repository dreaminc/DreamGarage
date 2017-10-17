#include "DreamOSTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "DreamTestingApp.h"
#include "DreamUserApp.h"

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

	CR(AddTestUserApp());	

	CR(AddTestDreamApps());


Error:
	return r;
}

RESULT DreamOSTestSuite::SetupPipeline() {
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
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
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

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

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

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

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

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

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

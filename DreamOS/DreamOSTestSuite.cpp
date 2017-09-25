#include "DreamOSTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

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

	CR(AddTestDreamApps());

	CR(AddTestUserApp());	

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

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamApps() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamTestApp> pDreamTestApp = nullptr;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the Shared View App
		pDreamTestApp = m_pDreamOS->LaunchDreamApp<DreamTestApp>(this);
		CNM(pDreamTestApp, "Failed to create dream test app");

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

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the Shared View App
		pDreamUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		CNM(pDreamUserApp, "Failed to create dream user app");

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
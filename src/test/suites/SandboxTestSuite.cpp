#include "SandboxTestSuite.h"

#include "os/DreamOS.h"

#include "pipeline/ProgramNode.h"
#include "pipeline/SinkNode.h"
#include "pipeline/SourceNode.h"

#include "hal/UIStageProgram.h"

#include "apps/DreamTestingApp/DreamTestingApp.h"

#include "scene/CameraNode.h"
#include "scene/ObjectStoreNode.h"

SandboxTestSuite::SandboxTestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("sandbox", pDreamOS)
{
	// empty
}

SandboxTestSuite::~SandboxTestSuite() {
	// empty
}

RESULT SandboxTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestCompositeObject());
	
	CR(AddTestObjectPipeline());

	CR(AddTestWin64());

	CR(AddTestAndroid());

Error:
	return r;
}

RESULT SandboxTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	CNM(m_pDreamOS, "DreamOS handle is not set");

Error:
	return r;
}

RESULT SandboxTestSuite::DefaultResetProcess(void* pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

	// Reset the pipeline
	HALImp* pHAL;
	pHAL = m_pDreamOS->GetHALImp();

	Pipeline* pPipeline;
	pPipeline = pHAL->GetRenderPipelineHandle();
	CR(pPipeline->Reset(false));

Error:
	return r;
}

RESULT SandboxTestSuite::SetupPipeline(std::string strRenderProgramName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	{

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

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());
	}

Error:
	return r;
}

RESULT SandboxTestSuite::AddTestWin64() {
	RESULT r = R_PASS;

	TestObject::TestDescriptor testDescriptor;

	testDescriptor.sDuration = 2000.0f;
	testDescriptor.nRepeats = 1;
	testDescriptor.strTestName = "win64";
	testDescriptor.strTestDescription = "Test Win64 sandbox";

	struct TestContext {
		float width = 1.5f;
		float height = width;
		float length = width;
		float padding = 0.5f;
		DreamOS* pDreamOS = nullptr;

		TestContext(DreamOS* pDreamOS) : pDreamOS(pDreamOS) {}
	};
	testDescriptor.pContext = new TestContext(m_pDreamOS);

	// Initialize Code 
	testDescriptor.fnInitialize = [=](void* pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp* pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		TestContext* pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects 

		volume* pVolume;
		pVolume = nullptr;
		sphere* pSphere;
		pSphere = nullptr;

		light* pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * 0.0f));
			CR(pVolume->SetVertexColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));

			pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
			CN(pSphere);
			pSphere->SetPosition(point(pTestContext->width, 0.0f, 0.0f));
			CR(pSphere->SetVertexColor(COLOR_YELLOW));

		}

	Error:
		return r;
	};

	auto pNewTest = AddTest(testDescriptor);
	CN(pNewTest);

Error:
	return r;
}

RESULT SandboxTestSuite::AddTestAndroid() {
	RESULT r = R_PASS;

	TestObject::TestDescriptor testDescriptor;

	testDescriptor.sDuration = 2000.0f;
	testDescriptor.nRepeats = 1;
	testDescriptor.strTestName = "android";
	testDescriptor.strTestDescription = "Test Android Sandbox";

	struct TestContext {
		float width = 1.5f;
		float height = width;
		float length = width;
		float padding = 0.5f;
		DreamOS* pDreamOS = nullptr;

		TestContext(DreamOS* pDreamOS) : pDreamOS(pDreamOS) {}
	};
	testDescriptor.pContext = new TestContext(m_pDreamOS);

	// Initialize Code 
	testDescriptor.fnInitialize = [=](void* pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp* pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		TestContext* pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects 

		volume* pVolume;
		pVolume = nullptr;
		sphere* pSphere;
		pSphere = nullptr;

		light* pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * 0.0f));
			CR(pVolume->SetVertexColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(pTestContext->width, pTestContext->height, pTestContext->length);
			CN(pVolume);
			pVolume->SetPosition(point(-pTestContext->width, 0.0f, (pTestContext->length + pTestContext->padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));

			pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
			CN(pSphere);
			pSphere->SetPosition(point(pTestContext->width, 0.0f, 0.0f));
			CR(pSphere->SetVertexColor(COLOR_YELLOW));

		}

	Error:
		return r;
	};

	auto pNewTest = AddTest(testDescriptor);
	CN(pNewTest);

Error:
	return r;
}

RESULT SandboxTestSuite::SetupDreamAppPipeline() {
	RESULT r = R_PASS;
	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	{

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

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

		// Connect Program to Display
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
	}

Error:
	return r;
}

RESULT SandboxTestSuite::AddTestObjectPipeline() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());

		// TODO: Add some objects

		// Objects 
		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		{

			// TODO: Why does shit explode with no objects in scene
			auto pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
			pSphere->MoveTo(point(-1.0f, 0.0f, 0.0f));

			auto pCube = m_pDreamOS->AddVolume(0.5f);
			pCube->MoveTo(point(0.0f, 0.0f, 0.0f));

			auto pCylinder = m_pDreamOS->AddCylinder(0.25f, 0.5f, 20, 20);
			pCylinder->MoveTo(point(1.0f, -0.25f, 0.0f));
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

	auto pUITest = AddTest("objectpipeline", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("This is a test for the basic object pipeline arch");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SandboxTestSuite::AddTestCompositeObject() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());

		// TODO: Add some objects

		// Objects 
		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// TODO: Why does shit explode with no objects in scene
		composite *pComposite;
		pComposite = m_pDreamOS->AddComposite();
		
		pComposite->AddSphere(0.25f, 10, 10);

		//pComposite->ClearChildren();
		pComposite->ClearObjects();

		pComposite->AddVolume(0.5f);

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

	auto pUITest = AddTest("compositeobject", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("This is a test for the basic object pipeline arch");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}
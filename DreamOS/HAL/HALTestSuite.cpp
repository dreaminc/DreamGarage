#include "HALTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

#include "PhysicsEngine/CollisionManifold.h"

HALTestSuite::HALTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

HALTestSuite::~HALTestSuite() {
	// empty
}

RESULT HALTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestQuadObject());

	CR(AddTestBlinnPhongShaderTexture());

	CR(AddTestSenseHaptics());

	CR(AddTestMinimalTextureShader());

	CR(AddTestModel());
	
	CR(AddTestBlinnPhongShader());

	CR(AddTestText());

	CR(AddTestBlinnPhongShaderTextureCopy());

	CR(AddTestRenderToTextureQuad());

	CR(AddTestEnvironmentShader());

	CR(AddTestBlinnPhongShaderTextureHMD());

	CR(AddTestBlinnPhongShaderBlurHMD());

	CR(AddTestBlinnPhongShaderBlur());

	CR(AddTestMinimalShader());

	CR(AddTestMinimalShaderHMD());
	
	CR(AddTestDepthPeelingShader());

	CR(AddTestAlphaVolumes());

	CR(AddTestFramerateVolumes());

Error:
	return r;
}

RESULT HALTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

	// Reset the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();
	CR(pPipeline->Reset(false));

Error:
	return r;
}

RESULT HALTestSuite::AddTestDepthPeelingShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.3f;
	float alpha = 0.25f;

	struct depthPeelingTestContext {
		volume *pVolume1 = nullptr;
		volume *pVolume2 = nullptr;
	};

	depthPeelingTestContext *pTestContext = new depthPeelingTestContext;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("depthpeel");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pRenderProgramNode->ConnectToInput("input_framebufferA", pRenderProgramNode->Output("output_framebufferA")));
		CR(pRenderProgramNode->ConnectToInput("input_framebufferB", pRenderProgramNode->Output("output_framebufferB")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		depthPeelingTestContext *pTestContext = static_cast<depthPeelingTestContext*>(pContext);

		volume *pVolume = nullptr;
		sphere *pSphere = nullptr;

		/*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		//CR(pVolume->SetColor(COLOR_WHITE));
		CR(pVolume->SetRandomColor());
		CR(pVolume->SetAlpha(alpha));
		//*/

		///*
		pSphere = m_pDreamOS->AddSphere(1.0f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pSphere->SetColor(COLOR_WHITE));
		CR(pSphere->SetAlpha(alpha));
		//*/

		pTestContext->pVolume1 = m_pDreamOS->AddVolume(width * 0.5f, height * 0.5f, length * 0.5f);
		pVolume = pTestContext->pVolume1;
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 1.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		CR(pVolume->SetAlpha(1.0f));

		// Opposite ordering 
		pTestContext->pVolume2 = m_pDreamOS->AddVolume(width * 0.5f, height * 0.5f, length * 0.5f);
		pVolume = pTestContext->pVolume2;
		CN(pVolume);
		pVolume->SetPosition(point(width, 0.0f, (length + padding) * 1.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		CR(pVolume->SetAlpha(1.0f));

		/*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));
		CR(pVolume->SetAlpha(alpha));
		//*/
		
		///*
		pSphere = m_pDreamOS->AddSphere(1.0f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(width, 0.0f, (length + padding) * 0.0f));
		CR(pSphere->SetColor(COLOR_WHITE));
		CR(pSphere->SetAlpha(alpha));
		//*/

		/*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_WHITE));
		CR(pVolume->SetAlpha(alpha));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_WHITE));
		CR(pVolume->SetAlpha(alpha));
		*/


	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		depthPeelingTestContext *pTestContext = static_cast<depthPeelingTestContext*>(pContext);
		pTestContext->pVolume1->SetPosition(pTestContext->pVolume1->GetPosition() + vector(0.0f, 0.0f, -0.001f));
		pTestContext->pVolume2->SetPosition(pTestContext->pVolume2->GetPosition() + vector(0.0f, 0.0f, -0.001f));

		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Alpha Volumes");
	pNewTest->SetTestDescription("Test alpha with volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


// TODO: There should be a cleaner way to run w/ HMD or not (auto-detect)
// TODO: There's a bug with directional lights here
RESULT HALTestSuite::AddTestBlinnPhongShaderTextureHMD() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		/*
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));
		*/

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		texture *pColorTexture = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		pVolume->SetColorTexture(pColorTexture);

		//CR(pVolume->SetColor(COLOR_WHITE));

		///*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		//*/

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Texture Shader");
	pNewTest->SetTestDescription("Blinn phong texture shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestEnvironmentShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

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

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Debug Console
		ProgramNode* pDreamConsoleProgram = pHAL->MakeProgramNode("debugconsole");
		CN(pDreamConsoleProgram);
		CR(pDreamConsoleProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pDreamConsoleProgram->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		texture *pColorTexture = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		pVolume->SetColorTexture(pColorTexture);

		//CR(pVolume->SetColor(COLOR_WHITE));

		///*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		//*/

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Environment Shader");
	pNewTest->SetTestDescription("Environment shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// This will test the HAL model shit
// TODO: Fix this
RESULT HALTestSuite::AddTestModel() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	float width = 5.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

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

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Debug Console
		ProgramNode* pDreamConsoleProgram = pHAL->MakeProgramNode("debugconsole");
		CN(pDreamConsoleProgram);
		CR(pDreamConsoleProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pDreamConsoleProgram->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));


		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;
		sphere *pSphere = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));
		//light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{
			///*
			auto pHead = m_pDreamOS->AddModel(L"\\Models\\face4\\untitled.obj",
				nullptr,
				point(0.0f, 0.0f, 0.0f),
				0.018f,
				vector(0.0f, 0.0f, 0.0f));

			pHead->SetPosition(point(0.0f, 0.0f, 2.0f));
			//*/

			pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20, COLOR_RED);
			CN(pSphere);
			pSphere->SetPosition(point(2.0f, 0.0f, 0.0f));
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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestQuadObject() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	float width = 5.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
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

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Debug Console
		ProgramNode* pDreamConsoleProgram = pHAL->MakeProgramNode("debugconsole");
		CN(pDreamConsoleProgram);
		CR(pDreamConsoleProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pDreamConsoleProgram->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));
		//light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, -0.0f));
		//light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{
			//auto pQuad = m_pDreamOS->AddQuad(1.0f, 1.0f);
			//auto pQuad = m_pDreamOS->TAddQuad(1.0f, 1.0f);
			//auto pQuad = m_pDreamOS->Add<quad>(1.0f, 1.0f);
			
			//auto pQuad = m_pDreamOS->Add<quad>(4.0f, 1.0f, 10, 10, quad::CurveType::PARABOLIC);

			/*
			auto pComposite = m_pDreamOS->Add<composite>();
			auto pQuad = pComposite->Add<quad>(5.0f, 1.0f, 10, 10, quad::CurveType::CIRCLE);

			CN(pQuad);
			pQuad->SetPosition(0.0f, -2.0f, 0.0f);
			pQuad->RotateXByDeg(90.0f);
			*/

			/*
			float lineHeight = 0.35f;

			auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
			pFont->SetLineHeight(lineHeight);
			CN(pFont);

			auto pText = m_pDreamOS->Add<text>(pFont, "testing curved text", lineHeight * 10.0f, lineHeight * 2.6f, text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD | text::flags::CURVE_QUAD_CIRCLE);
			//auto pText = m_pDreamOS->Add<text>(pFont, "testing curved text", lineHeight * 10.0f, lineHeight * 2.6f, text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD | text::flags::CURVE_QUAD_PARABOLIC);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, 0.0f, 0.0f));
			pText->SetMaterialAmbient(0.8f);
			*/

			auto pFlatContext = m_pDreamOS->Add<FlatContext>(1024, 1024, 4);
			CN(pFlatContext);

			auto pQuad = pFlatContext->AddQuad(1.0f, 1.0f);
			CN(pQuad);
			//pQuad->RotateXByDeg(90.0f);

			pQuad = pFlatContext->AddQuad(1.0f, 1.0f);
			CN(pQuad);
			pQuad->translateX(-1.5f);
			//pQuad->RotateXByDeg(90.0f);

			pQuad = pFlatContext->AddQuad(1.0f, 1.0f);
			CN(pQuad);
			pQuad->translateX(1.5f);
			//pQuad->RotateXByDeg(90.0f);

			//pFlatContext->translateY(-1.0f);
			pFlatContext->translateZ(-1.0f);

			pFlatContext->RenderToQuad(quad::CurveType::CIRCLE);
			pFlatContext->RotateXByDeg(90.0f);
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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("HAL Quad Test");
	pNewTest->SetTestDescription("HAL Quad Geometry test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestText() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 5.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

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

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Debug Console
		ProgramNode* pDreamConsoleProgram = pHAL->MakeProgramNode("debugconsole");
		CN(pDreamConsoleProgram);
		CR(pDreamConsoleProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pDreamConsoleProgram->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		
		{
			auto pFlatContext = m_pDreamOS->AddFlatContext();

			auto pComposite = m_pDreamOS->AddComposite();
			auto pFont = std::make_shared<font>(L"Basis_Grotesque_Pro.fnt", pComposite, true);

			texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"Fonts/Basis_Grotesque_Pro.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
			auto pTextLetter = pFlatContext->AddText(pFont, pFont->GetTexture().get(), "hi", 1.0f, true);

			m_pDreamOS->RenderToTexture(pFlatContext);

			auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));
			pQuad->SetColorTexture(m_pDreamOS->MakeTexture(*(pFlatContext->GetFramebuffer()->GetColorTexture())));

			pFlatContext->ClearChildren();
			
			pTextLetter = pFlatContext->AddText(pFont, pFont->GetTexture().get(), "hello", 1.0f, true);
			m_pDreamOS->RenderToTexture(pFlatContext);
			
			pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(1.0f, 0.0f, 0.0f));
			pQuad->SetColorTexture(m_pDreamOS->MakeTexture(*(pFlatContext->GetFramebuffer()->GetColorTexture())));

			
		}

		/*
		texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
		texture *pColorTexture2 = m_pDreamOS->MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		texture *pColorTextureCopy = m_pDreamOS->MakeTexture(*pColorTexture1);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		pVolume->SetColorTexture(pColorTexture1);
		//CR(pVolume->SetColor(COLOR_WHITE));

		///*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(width, 0.0f, (length + padding) * -3.0f));
		//CR(pVolume->SetColor(COLOR_GREEN));
		pVolume->SetColorTexture(pColorTexture2);


		auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
		CN(pQuad);
		pQuad->SetPosition(point(width, 0.0f, (length + padding) * -0.0f));
		//CR(pVolume->SetColor(COLOR_GREEN));
		pQuad->SetColorTexture(pColorTextureCopy);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		//*/

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Text Test");
	pNewTest->SetTestDescription("Blinn phong text shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: There's a bug with directional lights here
RESULT HALTestSuite::AddTestBlinnPhongShaderTextureCopy() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
		texture *pColorTexture2 = m_pDreamOS->MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		texture *pColorTextureCopy = m_pDreamOS->MakeTexture(*pColorTexture1);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		pVolume->SetColorTexture(pColorTexture1);
		//CR(pVolume->SetColor(COLOR_WHITE));

		///*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(width, 0.0f, (length + padding) * -3.0f));
		//CR(pVolume->SetColor(COLOR_GREEN));
		pVolume->SetColorTexture(pColorTexture2);


		auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
		CN(pQuad);
		pQuad->SetPosition(point(width, 0.0f, (length + padding) * -0.0f));
		//CR(pVolume->SetColor(COLOR_GREEN));
		pQuad->SetColorTexture(pColorTextureCopy);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		//*/

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Texture Copy Shader");
	pNewTest->SetTestDescription("Blinn phong texture shader test with a textured copied over");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestSenseHaptics() {
	RESULT r = R_PASS;

	double sTestTime = 800.0f;
	int nRepeats = 1;

	float width = 1.0f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		volume *pVolume = nullptr;
		bool fCollide[2] = { false, false };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetPosition(point(0.0f, 1.0f, 4.0f));
		pTestContext->pVolume->SetColor(COLOR_BLUE);

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

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext); 

		// Get the controller sphere
		for (int i = 0; i < CONTROLLER_INVALID; i++) {
			sphere *pControllerSphere = (sphere*)(m_pDreamOS->GetHMD()->GetSenseControllerObject(CONTROLLER_TYPE(i)));

			if (pControllerSphere != nullptr) {
				auto manifold = pControllerSphere->Collide(pTestContext->pVolume);
				if (manifold.NumContacts() > 0 && pTestContext->fCollide[i] == false) {
					//m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticBuffer(CONTROLLER_TYPE(i), SenseController::HapticCurveType::SINE, 1.0f, 5.0f, 800.0f);
					m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(i), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1);
					pTestContext->fCollide[i] = true;
				}
				else if (manifold.NumContacts() == 0 && pTestContext->fCollide[i] == true) {
					pTestContext->fCollide[i] = false;
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Texture Shader");
	pNewTest->SetTestDescription("Blinn phong texture shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: There's a bug with directional lights here
RESULT HALTestSuite::AddTestBlinnPhongShaderTexture() {
	RESULT r = R_PASS;

	double sTestTime = 80.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
		texture *pColorTexture2 = m_pDreamOS->MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		pVolume->SetColorTexture(pColorTexture1);
		
		//CR(pVolume->SetColor(COLOR_WHITE));

		///*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(width, 0.0f, (length + padding) * -3.0f));
		//CR(pVolume->SetColor(COLOR_GREEN));
		pVolume->SetColorTexture(pColorTexture2);


		auto pQuad = m_pDreamOS->AddQuad(1.0f, 0.5f, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
		CN(pQuad);
		pQuad->SetPosition(point(width, 0.0f, (length + padding) * -0.0f));
		//CR(pVolume->SetColor(COLOR_GREEN));
		pQuad->SetColorTexture(pColorTexture1);

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		//*/

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Texture Shader");
	pNewTest->SetTestDescription("Blinn phong texture shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: There should be a cleaner way to run w/ HMD or not (auto-detect)
RESULT HALTestSuite::AddTestBlinnPhongShaderBlur() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderBlurQuad = pHAL->MakeProgramNode("blur");
		CN(pRenderBlurQuad);
		CR(pRenderBlurQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderBlurQuad->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Shader Blur");
	pNewTest->SetTestDescription("Blinn phong shader with blur test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT HALTestSuite::AddTestBlinnPhongShaderBlurHMD() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderBlurQuad = pHAL->MakeProgramNode("blur");
		CN(pRenderBlurQuad);
		CR(pRenderBlurQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderBlurQuad->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer_lefteye", pRenderScreenQuad->Output("output_framebuffer")));
		CR(pDestSinkNode->ConnectToInput("input_framebuffer_righteye", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Shader Blur HMD");
	pNewTest->SetTestDescription("Blinn phong shader with blur HMD test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestBlinnPhongShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));		

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;
		sphere *pSphere = nullptr;

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));

		pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
		CN(pSphere);
		pSphere->SetPosition(point(width, 0.0f, 0.0f));
		CR(pSphere->SetColor(COLOR_YELLOW));

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Shader HMD");
	pNewTest->SetTestDescription("Blinn phong shader HMD test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestMinimalShaderHMD() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		/*
		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));
		//*/

		CR(pDestSinkNode->ConnectToInput("input_framebuffer_lefteye", pRenderProgramNode->Output("output_framebuffer")));
		CR(pDestSinkNode->ConnectToInput("input_framebuffer_righteye", pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Minimal Shader HMD");
	pNewTest->SetTestDescription("Minimal shader HMD test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestMinimalShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: 
RESULT HALTestSuite::AddTestMinimalTextureShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline

		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
		
		texture *pColorTexture = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
		CN(pColorTexture);

		CR(pVolume->SetColorTexture(pColorTexture));


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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestRenderToTextureQuad() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.3f;
	float alpha = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		{
			// Set up the pipeline
			HALImp *pHAL = m_pDreamOS->GetHALImp();
			Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

			SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
			CNM(pDestSinkNode, "Destination sink node isn't set");

			CR(pHAL->MakeCurrentContext());

			ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
			CN(pRenderProgramNode);
			CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
			CN(pRenderScreenQuad);
			CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

			CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

			CR(pHAL->ReleaseCurrentContext());

			// Set up scene
			
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

			composite *pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);

			std::shared_ptr<FlatContext> pFlatContext = pComposite->MakeFlatContext();
			CN(pFlatContext);

			std::shared_ptr<quad> pFlatQuad = pFlatContext->AddQuad(1.0f, 1.0f, point(0.0f));
			CN(pContext);

			pComposite->RenderToTexture(pFlatContext);

			quad *pQuad = m_pDreamOS->AddQuad(width, height);
			CN(pQuad);
			CN(pQuad->SetPosition(point(0.0f, -2.0f, 0.0f)));
			pQuad->SetColor(COLOR_GREEN);

			// TODO: this is no longer supported:
			CR(pQuad->SetColorTexture(pFlatContext->GetFramebuffer()->GetColorTexture()));
			
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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestAlphaVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.3f;
	float alpha = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));
		CR(pVolume->SetAlpha(alpha));


		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 1.0f));
		CR(pVolume->SetColor(COLOR_WHITE));
		CR(pVolume->SetAlpha(alpha));
		
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));
		CR(pVolume->SetAlpha(alpha));
		
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		CR(pVolume->SetAlpha(alpha));
	

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Alpha Volumes");
	pNewTest->SetTestDescription("Test alpha with volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestFramerateVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 0.25f;
	float height = width;
	float length = width;

	float padding = 0.2f;

	int numObj = 7;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		m_pDreamOS->SetGravityState(false);

		volume *pVolume = nullptr;  
		
		for (int i = 0; i < numObj; i++) {
			for (int j = 0; j < numObj; j++) {
				for (int k = 0; k < numObj; k++) {
					float xPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (i * (width + padding));
					float yPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (j * (width + padding));
					float zPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (k * (width + padding));
					
					pVolume = m_pDreamOS->AddVolume(width, height, length);
					pVolume->SetPosition(point(xPos, yPos, zPos));
				}
			}
		}

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

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Framerate Volumes");
	pNewTest->SetTestDescription("Test frame rate vs many volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

HALImp* HALTestSuite::GetHALImp() {
	return m_pDreamOS->GetHALImp();
}

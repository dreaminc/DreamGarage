#include "HALTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

#include "Primitives/HysteresisCylinder.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "HAL/opengl/OGLProgramReflection.h"
#include "HAL/opengl/OGLProgramRefraction.h"
#include "HAL/opengl/OGLProgramWater.h"
#include "HAL/opengl/OGLProgramSkyboxScatter.h"
#include "HAL/opengl/OGLProgramScreenFade.h"
#include "HAL/opengl/OGLProgramSkybox.h"
#include "HAL/opengl/OGLProgramStandard.h"

#include "DreamGarage\DreamGamepadCameraApp.h"

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

	CR(AddTestModel());

	CR(AddTestPBOTextureReadback());
	
	CR(AddTestPBOTextureUpload());

	CR(AddTestTextureUpdate());

	CR(AddTestMinimalTextureShader());

	CR(AddTestIrradianceMap());

	CR(AddTest3rdPersonCamera());

	CR(AddTestEnvironmentMapping());

	CR(AddTestCubeMap());

	CR(AddTestWaterShaderCube());

	CR(AddTestBlinnPhongShaderTextureBumpDisplacement());

	CR(AddTestBlinnPhongShaderTextureBump());
	
	CR(AddTestGeometryShader());
  
	CR(AddTestCamera());

	CR(AddTestWaterShader());
	
	CR(AddTestObjectMaterialsBump());

	CR(AddTestObjectMaterialsColors());

	CR(AddTestFadeShader());

	CR(AddTestSkybox());

	CR(AddTestToonShader());

	CR(AddTestStandardShader());

	CR(AddTestBlinnPhongShader());

	CR(AddTestBlinnPhongShaderTexture());	

	CR(AddTestHeightQuadObject());

	CR(AddTestIncludeShader());

	CR(AddTestEnvironments());

	CR(AddTestRemoveObjects());

	CR(AddTestFlatContextNesting());

	CR(TestNestedOBB());

	CR(AddTestText());

	CR(AddTestUserModel());

	CR(AddTestModelInstancing());
	
	CR(AddTestRotation());

	CR(AddTestModelOrientation());

	CR(AddTestUIShaderStage());

	CR(AddTestRenderToTextureQuad());

	CR(AddTestMouseDrag());

	CR(AddTestMinimalShader());

	CR(AddTestQuadObject());

	CR(AddTestSenseHaptics());

	CR(AddTestBlinnPhongShaderBlur());

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
	HALImp *pHAL;
	pHAL = m_pDreamOS->GetHALImp();

	Pipeline* pPipeline;
	pPipeline = pHAL->GetRenderPipelineHandle();
	CR(pPipeline->Reset(false));

Error:
	return r;
}

RESULT HALTestSuite::SetupSkyboxPipeline(std::string strRenderShaderName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode;
	pRenderProgramNode = pHAL->MakeProgramNode(strRenderShaderName);
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram;
	pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	/*
	// Skybox
	ProgramNode* pSkyboxProgram;
	pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	//*/

	ProgramNode *pRenderScreenQuad;
	pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT HALTestSuite::AddTestRemoveObjects() {
	RESULT r = R_PASS;
	
	double sTestTime = 10000.0f;
	int nRepeats = 1;

	struct TestContext {

		double msLastSent = 0.0;
		double msTimeDelay = 500.0;
		bool fObject = true;

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupSkyboxPipeline();
		Initialize();
		m_pDreamOS->AddQuad(1.0f, 1.0f)->RotateXByDeg(90.0f);
	//Error:
		return r;
	};
	
	auto fnUpdate = [&](void *pContext) {
		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
		if (msTimeNow - pTestContext->msLastSent > pTestContext->msTimeDelay) {
			if (pTestContext->fObject) {
				CR(m_pDreamOS->RemoveAllObjects());
				pTestContext->fObject = false;
			}
			else {
				m_pDreamOS->AddQuad(1.0f, 1.0f)->RotateXByDeg(90.0f);
				pTestContext->fObject = true;
			}
			pTestContext->msLastSent = msTimeNow;
		}

	Error:
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	//auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Remove Test");
	pNewTest->SetTestDescription("remove");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("depthpeel");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pRenderProgramNode->ConnectToInput("input_framebufferA", pRenderProgramNode->Output("output_framebufferA")));
		CR(pRenderProgramNode->ConnectToInput("input_framebufferB", pRenderProgramNode->Output("output_framebufferB")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 
		depthPeelingTestContext *pTestContext;
		pTestContext = static_cast<depthPeelingTestContext*>(pContext);
		
		volume *pVolume;
		pVolume = nullptr;
		
		sphere *pSphere;
		pSphere = nullptr;

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
		CR(pSphere->SetVertexColor(COLOR_WHITE));
		CR(pSphere->SetAlpha(alpha));
		//*/

		pTestContext->pVolume1 = m_pDreamOS->AddVolume(width * 0.5f, height * 0.5f, length * 0.5f);
		pVolume = pTestContext->pVolume1;
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 1.0f));
		CR(pVolume->SetVertexColor(COLOR_BLUE));
		CR(pVolume->SetAlpha(1.0f));

		// Opposite ordering 
		pTestContext->pVolume2 = m_pDreamOS->AddVolume(width * 0.5f, height * 0.5f, length * 0.5f);
		pVolume = pTestContext->pVolume2;
		CN(pVolume);
		pVolume->SetPosition(point(width, 0.0f, (length + padding) * 1.0f));
		CR(pVolume->SetVertexColor(COLOR_BLUE));
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
		CR(pSphere->SetVertexColor(COLOR_WHITE));
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

RESULT HALTestSuite::AddTestSkybox() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(SetupSkyboxPipeline("standard"));

		CR(Initialize());

		m_pDreamOS->AddQuad(1.0f, 1.0f)->RotateXByDeg(90.0f);

	Error:
		return r;
	};

	auto fnPass = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnPass, fnPass, fnPass, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Sky Test");
	pNewTest->SetTestDescription("sky");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestEnvironments() {
	RESULT r = R_PASS;

	double sTestTime = 200.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		float sceneScale = 0.025f;

		// Set up the pipeline
		CR(SetupSkyboxPipeline("environment"));

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));
		
		light *pLight2;
		pLight2 = m_pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(5.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		// environment strings
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\001.fbx"); // open ceiling
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\002.fbx"); // angular
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\004.fbx"); // pillars
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\005.fbx"); // wave
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\006.fbx"); // dome
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\007.fbx"); // cave
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\008.fbx"); // wood house
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\009.fbx"); // industrial
		//model* pModel = m_pDreamOS->AddModel(L"\\TestEnvironments\\DREAM_OS_2018_05_07\\010.fbx"); // tube

		model* pModel;
		pModel = m_pDreamOS->AddModel(L"CaveEnvironment.fbx"); // cave
		//model* pModel = m_pDreamOS->AddModel(L"\\DREAM_OS_2018_05_22\\DREAM_OS_2018_05_22\\006.fbx"); // cave

		// Ambient Occlusion textures
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\001_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\002_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\004_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\005_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\006_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\007_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\008_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\009_AO.png"));
		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\TestEnvironments\\DREAM_OS_2018_05_07\\map\\010_AO.png"));

		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\DREAM_OS_2018_05_22\\DREAM_OS_2018_05_22\\map\\007_AO.png"));
		pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"..\\Models\\Map\\ao_color.tga"));

		//pModel->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"\\DREAM_OS_2018_05_22\\DREAM_OS_2018_05_22\\map\\006_AO.png"));

		//pModel->RotateXByDeg(-90.0f);
		//pModel->RotateYByDeg(90.0f);
		pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
		//pModel->RotateZByDeg(-90.0f);
		pModel->SetScale(sceneScale);

//		m_pDreamOS->AddUser();
		model* pHead;
		pHead = m_pDreamOS->AddModel(L"face4\\untitled.obj");

		//auto pHead = m_pDreamOS->AddModel(L"head.FBX");
		pHead->SetScale(sceneScale);
		pHead->RotateYByDeg(180.0f);

	Error:
		return r;
	};

	auto fnPass = [=](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnPass, fnPass, fnPass, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Environment Test");
	pNewTest->SetTestDescription("New Environment test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestIncludeShader() {
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Connect Program to Display
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, 0.0f, -0.5f));

		{
			auto pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			CN(pModel);
			pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
			pModel->SetScale(0.1f);
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

	pNewTest->SetTestName("Include Shader");
	pNewTest->SetTestDescription("Testing shaders including other shader files test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}
	
RESULT HALTestSuite::AddTestToonShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	struct TestContext {
		model *pModel = nullptr;
	};
	TestContext *pTestContext = new TestContext();

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("toon");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);


		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Connect Program to Display
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.35f, -1.0f, -1.0f));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			//auto pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			pTestContext->pModel = m_pDreamOS->AddModel(L"\\head_01\\head_01.fbx");
			CN(pTestContext->pModel);

			pTestContext->pModel->SetPosition(point(0.0f, -2.0f, -1.0f));
			pTestContext->pModel->SetScale(0.1f);
			//pTestContext->pModel->RotateXByDeg(-90.0f);
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
		RESULT r = R_PASS;

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pModel);
		pTestContext->pModel->RotateYByDeg(0.01f);

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

	pNewTest->SetTestName("Include Shader");
	pNewTest->SetTestDescription("Testing shaders including other shader files test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestGeometryShader() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		model *pModel = nullptr;
	};
	TestContext *pTestContext = new TestContext();

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

		///*
		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		//pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_texture");
		//pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		//*/

		// Reference Geometry Shader Program
		/*
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));
		*/

		// Visualize Normals
		///*
		ProgramNode* pVisualNormalsProgram;
		pVisualNormalsProgram = pHAL->MakeProgramNode("visualize_normals");
		//pVisualNormalsProgram = pHAL->MakeProgramNode("minimal");
		CN(pVisualNormalsProgram);
		CR(pVisualNormalsProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pVisualNormalsProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pVisualNormalsProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));
		//*/

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pVisualNormalsProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, -0.5f));

		{

			TestContext *pTestContext;
			pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			//texture *pColorTexture;
			//pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			//
			//volume *pVolume = m_pDreamOS->AddVolume(width, height, length);
			//CN(pVolume);
			//pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			//pVolume->SetDiffuseTexture(pColorTexture);
			//
			//pVolume = m_pDreamOS->AddVolume(width, height, length);
			//CN(pVolume);
			//pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			//CR(pVolume->SetVertexColor(COLOR_GREEN));
			//
			//pVolume = m_pDreamOS->AddVolume(width, height, length);
			//CN(pVolume);
			//pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			//CR(pVolume->SetVertexColor(COLOR_RED));
			//
			//pVolume = m_pDreamOS->AddVolume(width, height, length);
			//CN(pVolume);
			//pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			//CR(pVolume->SetVertexColor(COLOR_BLUE));

			model *pModel = nullptr;
			
			pModel = m_pDreamOS->AddModel(L"\\Avatars\\lefthand_1.FBX");
			CN(pModel);
			pModel->SetPosition(point(-1.0f, -3.0f, -3.0f));
			pModel->SetScale(0.1f);
			pModel->SetMaterialShininess(2.0f, true);
			//pModel->RotateXByDeg(-90.0f);
			//pModel->SetMaterialColors(COLOR_WHITE, true);
			
			pModel = m_pDreamOS->AddModel(L"\\Avatars\\righthand_1.FBX", ModelFactory::flags::FLIP_WINDING);
			CN(pModel);
			pModel->SetPosition(point(1.0f, -3.0f, -3.0f));
			pModel->SetScale(0.1f);
			pModel->SetMaterialShininess(2.0f, true);
			//pModel->RotateXByDeg(-90.0f);
			//pModel->SetMaterialColors(COLOR_WHITE, true);
			
			pModel = m_pDreamOS->AddModel(L"\\Avatars\\avatar_1.FBX");
			CN(pModel);
			pModel->SetPosition(point(1.0f, -1.0f, -3.0f));
			pModel->SetScale(0.1f);
			//pModel->RotateXByDeg(-90.0f);
			pModel->SetMaterialShininess(2.0f, true);
			//pModel->SetMaterialColors(COLOR_WHITE, true);
			//pModel->SetMaterialColors(color(1.0f, 0.5f, 0.0f, 0.0f), true);
			//pModel->SetMaterialSpecularColor(COLOR_WHITE, true);
			//pModel->SetMaterialDiffuseColor(COLOR_WHITE, true);
			pModel->SetMaterialAmbientColor(COLOR_WHITE, true);

			pTestContext->pModel = pModel;

			sphere *pSphere = m_pDreamOS->AddSphere(0.5f, 20, 20);
			CN(pSphere);
			pSphere->SetPosition(point(-1.0f, -1.0f, -3.0f));
			pSphere->SetMaterialShininess(2.0f, true);
			pSphere->SetMaterialColors(COLOR_WHITE, true);

			//pModel = m_pDreamOS->AddModel(L"\\Cave\\cave.FBX");
			//CN(pModel);
			//pModel->SetScale(0.1f);	
			//pModel->SetPosition(point(0.0f, -10.0f, 0.0f));
			//pModel->RotateXByDeg(-90.0f);
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
		RESULT r = R_PASS;

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pModel);
		pTestContext->pModel->RotateYByDeg(0.015f);

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

	pNewTest->SetTestName("Environment Shader");
	pNewTest->SetTestDescription("Environment shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestFadeShader() {
	RESULT r = R_PASS;

	double sTestTime = 300.0f;
	int nRepeats = 1;

	struct TestContext {
		OGLProgramScreenFade *pScreenFadeProgram = nullptr;
		bool fFirst = true;
		model *m_pModel = nullptr;

		std::chrono::high_resolution_clock::time_point m_startTime;
		int m_iteration = -1;
		double m_iterationDuration = 2.0f;

		
	};
	TestContext *pTestContext = new TestContext();

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		point sceneOffset = point(90, -5, -25);
		float sceneScale = 0.1f;
		vector sceneDirection = vector(0.0f, 0.0f, 0.0f);

		// Set up the pipeline
		HALImp *pHAL = GetHALImp();

		Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();
		SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CN(pTestContext);

		pTestContext->m_startTime = std::chrono::high_resolution_clock::now();
		pTestContext->m_pModel = m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\env.obj");

		//CR(pHAL->MakeCurrentContext());

		{

			// Reflection 

			ProgramNode* pReflectionProgramNode;
			pReflectionProgramNode = nullptr;
			pReflectionProgramNode = pHAL->MakeProgramNode("reflection");
			CN(pReflectionProgramNode);
			CR(pReflectionProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pReflectionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			ProgramNode* pReflectionSkyboxProgram;
			pReflectionSkyboxProgram = nullptr;
			pReflectionSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
			CN(pReflectionSkyboxProgram);
			CR(pReflectionSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pReflectionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// Connect output as pass-thru to internal blend program
			CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", pReflectionProgramNode->Output("output_framebuffer")));

			// Refraction

			ProgramNode* pRefractionProgramNode;
			pRefractionProgramNode = pHAL->MakeProgramNode("refraction");
			CN(pRefractionProgramNode);
			CR(pRefractionProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pRefractionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// "Water"

			ProgramNode* pWaterProgramNode;
			pWaterProgramNode = nullptr;
			pWaterProgramNode = pHAL->MakeProgramNode("water");
			CN(pWaterProgramNode);
			CR(pWaterProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pWaterProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// TODO: This is not particularly general yet
			// Uncomment below to turn on water effects
			CR(pWaterProgramNode->ConnectToInput("input_refraction_map", pRefractionProgramNode->Output("output_framebuffer")));
			CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));

			// Standard shader

			ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("standard");
			CN(pRenderProgramNode);
			CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			CR(pRenderProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));

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

			ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
			CN(pUIProgramNode);
			CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
			CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
			CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// TODO: Matrix node
			//CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

			// Connect output as pass-thru to internal blend program
			CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

			EnvironmentProgram* pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

			// Screen Quad Shader (opt - we could replace this if we need to)
			ProgramNode *pRenderScreenFade = pHAL->MakeProgramNode("screenfade");
			CN(pRenderScreenFade);
			CR(pRenderScreenFade->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

			pTestContext->pScreenFadeProgram = dynamic_cast<OGLProgramScreenFade*>(pRenderScreenFade);

			// Connect Program to Display
			CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenFade->Output("output_framebuffer")));
			//CR(pDestSinkNode->ConnectToAllInputs(pUIProgramNode->Output("output_framebuffer")));

			quad *pWaterQuad = m_pDreamOS->MakeQuad(1000.0f, 1000.0f);
			point ptQuadOffset = point(90.0f, -1.3f, -25.0f);
			pWaterQuad->SetPosition(ptQuadOffset);
			pWaterQuad->SetMaterialColors(color(57.0f / 255.0f, 88.0f / 255.0f, 151.0f / 255.0f, 1.0f));
			CN(pWaterQuad);

			if (pWaterProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(pWaterQuad));
			}

			if (pReflectionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramReflection*>(pReflectionProgramNode)->SetReflectionObject(pWaterQuad));
			}

			if (pRefractionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramRefraction*>(pRefractionProgramNode)->SetRefractionObject(pWaterQuad));
			}

			if (pReflectionSkyboxProgram != nullptr) {
				CR(dynamic_cast<OGLProgramSkyboxScatter*>(pReflectionSkyboxProgram)->SetReflectionObject(pWaterQuad));
			}

			vector vLightDirection = vector(1.0f, -1.0f, 0.0f);
			float lightIntensity = 1.0f;
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLightDirection);
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.70f * lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f * vLightDirection));
		}

		m_pDreamOS->GetCamera()->SetPosition(point(0.0f, 5.0f, 0.0f));

	Error:
		return r;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		{
			auto msCurrentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pTestContext->m_startTime).count();
			int iteration = (int)(msCurrentTime / (1000.0f * pTestContext->m_iterationDuration));
			if (iteration != pTestContext->m_iteration) {
				pTestContext->m_iteration = iteration;

				if (pTestContext->m_iteration % 2 == 0) {
					pTestContext->pScreenFadeProgram->FadeOut();
				}
				else {
					pTestContext->pScreenFadeProgram->FadeIn();
				}
			}
		}

		/*
		if (pTestContext->fFirst) {
			pTestContext->fFirst = false;
			pTestContext->pScreenFadeProgram->FadeOut();
		}
		//*/

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

	pNewTest->SetTestName("Environment Fade Shader");
	pNewTest->SetTestDescription("Environment fade shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestWaterShader() {
	RESULT r = R_PASS;

	double sTestTime = 300.0f;
	int nRepeats = 1;

	struct TestContext {
		quad *pWaterQuad = nullptr;
		sphere *pSphere = nullptr;
		volume *pVolume = nullptr;
		quad *pLandQuad = nullptr;
	};
	TestContext *pTestContext = new TestContext();

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


		// Skybox

		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reflection 

		ProgramNode* pReflectionProgramNode;
		pReflectionProgramNode = nullptr;
		pReflectionProgramNode = pHAL->MakeProgramNode("reflection");
		CN(pReflectionProgramNode);
		CR(pReflectionProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReflectionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pReflectionSkyboxProgram;
		pReflectionSkyboxProgram = nullptr;
		pReflectionSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pReflectionSkyboxProgram);
		CR(pReflectionSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReflectionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		
		// Connect output as pass-thru to internal blend program
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", pReflectionProgramNode->Output("output_framebuffer")));

		// Refraction

		ProgramNode* pRefractionProgramNode;
		pRefractionProgramNode = pHAL->MakeProgramNode("refraction");
		CN(pRefractionProgramNode);
		CR(pRefractionProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRefractionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		//ProgramNode* pRefractionSkyboxProgram;
		//pRefractionSkyboxProgram = nullptr;
		//pRefractionSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		//CN(pRefractionSkyboxProgram);
		//CR(pRefractionSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		//CR(pRefractionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		//
		//// Connect output as pass-thru to internal blend program
		//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer", pRefractionProgramNode->Output("output_framebuffer")));

		// "Water"

		ProgramNode* pWaterProgramNode;
		pWaterProgramNode = nullptr;
		pWaterProgramNode = pHAL->MakeProgramNode("water");
		CN(pWaterProgramNode);
		//CR(pWaterProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pWaterProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// TODO: This is not particularly general yet
		CR(pWaterProgramNode->ConnectToInput("input_refraction_map", pRefractionProgramNode->Output("output_framebuffer")));
		CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));

		// Standard Shader

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pRenderProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			vector vLightDirection = vector(1.0f, -1.0f, 0.0f);
			float lightIntensity = 1.0f;
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLightDirection);
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.70f * lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f * vLightDirection));

			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_bump.jpg");

			texture *pBumpTextureWater;

			//pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"Dirt-1-2048-normal.png");
			//pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"normal-map-bumpy.png");
			pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"water_new_height.png");

			/*
			pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
			CN(pTestContext->pSphere);
			pTestContext->pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));
			pTestContext->pSphere->SetDiffuseTexture(pColorTexture);
			pTestContext->pSphere->SetBumpTexture(pBumpTexture);

			pTestContext->pVolume = m_pDreamOS->AddVolume(0.5f);
			CN(pTestContext->pVolume);
			pTestContext->pVolume->SetPosition(point(0.0f, -0.5f, 0.0f));
			CR(pTestContext->pVolume->SetVertexColor(COLOR_WHITE));
			pTestContext->pVolume->SetDiffuseTexture(pColorTexture);
			pTestContext->pVolume->SetBumpTexture(pBumpTexture);
			*/

			//pReflectionQuad = m_pDreamOS->AddQuad(5.0f, 5.0f, 1, 1);
			//pTestContext->pReflectionQuad = m_pDreamOS->MakeQuad(5.0f, 5.0f, 1, 1, nullptr, vector::jVector());
			pTestContext->pWaterQuad = m_pDreamOS->MakeQuad(1000.0f, 1000.0f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f).Normal());
			CN(pTestContext->pWaterQuad);
			pTestContext->pWaterQuad->SetPosition(90.0f, -1.25f, -25.0f);

			//pTestContext->pWaterQuad->SetPosition(0.0f, -0.1f, 0.0f);

			//pTestContext->pWaterQuad->SetBumpTexture(pBumpTextureWater);
			//pTestContext->pReflectionQuad->RotateZByDeg(45.0f);
			//pReflectionQuad->SetDiffuseTexture(dynamic_cast<OGLProgram*>(pReflectionProgramNode)->GetOGLFramebufferColorTexture());


			/*
			point ptSceneOffset = point(90, -5.0, -25);
			float sceneScale = 0.025f;
			vector vSceneEulerOrientation = vector(0.0f, 0.0f, 0.0f);

			model *pCaveModel = m_pDreamOS->AddModel(L"\\Cave\\cave.FBX");
			//model *pCaveModel = m_pDreamOS->AddModel(L"\\Cave\\cave_exported.fbx");
			CN(pCaveModel);
			pCaveModel->SetScale(sceneScale);

			m_pDreamOS->GetCamera()->SetPosition(-5.0f, 4.0f, -5.0f);
			m_pDreamOS->GetCamera()->RotateYByDeg(90.0f);
			*/

			///*
			texture *pLandColorTexture;
			texture *pLandHeightTexture;

			pLandColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"island-diffuse.jpg");
			pLandHeightTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"island-height.jpg");

			pTestContext->pLandQuad = m_pDreamOS->AddQuad(50.0f, 50.0f, 500, 500, pLandHeightTexture);
			CN(pTestContext->pLandQuad);
			pTestContext->pLandQuad->SetDiffuseTexture(pLandColorTexture);
			pTestContext->pLandQuad->SetPosition(0.0f, -0.75f, 0.0f);

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 0.25f, 20.0f);
			pTestContext->pWaterQuad->SetPosition(0.0f, -0.1f, 0.0f);
			//*/

			if (pWaterProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(pTestContext->pWaterQuad));
			}

			if (pReflectionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramReflection*>(pReflectionProgramNode)->SetReflectionObject(pTestContext->pWaterQuad));
			}

			if (pRefractionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramRefraction*>(pRefractionProgramNode)->SetRefractionObject(pTestContext->pWaterQuad));
			}

			if (pReflectionSkyboxProgram != nullptr) {
				CR(dynamic_cast<OGLProgramSkyboxScatter*>(pReflectionSkyboxProgram)->SetReflectionObject(pTestContext->pWaterQuad));
			}

			// NOTE: Refraction skybox needs no reflection plane - it's just looking through

			// TOOD: Test clipping
			//sphere *pSphere;
			//pSphere = m_pDreamOS->AddSphere(0.125f, 10, 10);
			//CN(pSphere);
			//pSphere->SetPosition(point(0.0f, -1.15f, 0.0f));

			//m_pDreamOS->GetCamera()->SetPosition(0.0f, -1.1f, 10.0f);

			/*
			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
			*/

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp)

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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->pSphere->translateY(-0.001f);
		//pTestContext->pVolume->translateY(0.001f);

		//pTestContext->pReflectionQuad->translateY(-0.0001f);
		//pTestContext->pReflectionQuad->RotateZByDeg(0.01f);
		//pTestContext->pWaterQuad->RotateXByDeg(0.002f);

		//m_pDreamOS->GetCamera()->translateZ(0.0001f);
		//m_pDreamOS->GetCamera()->translateY(0.0001f);

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

	pNewTest->SetTestName("Environment Shader");
	pNewTest->SetTestDescription("Environment shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: FIX: Reflections are view dependent!! 
RESULT HALTestSuite::AddTestEnvironmentMapping() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	float width		= 2.0f; 
	float height	= 2.0f;
	float length	= 2.0f;
	float padding	= 0.25f;

	struct TestContext {
		sphere *pSphere = nullptr;
		volume *pVolume = nullptr;
	};
	TestContext *pTestContext = new TestContext();

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


		// Skybox

		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Standard Shader

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pRenderProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

		// Skybox

		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReflectionSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display
		CR(pDestSinkNode->ConnectToAllInputs(pSkyboxProgram->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		volume *pVolume;
		sphere *pSphere;

		{
			float lightIntensity = 1.0f;
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f, -1.0f, -1.0f));

			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_bump.jpg");

			texture *pBumpTextureWater;

			//pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"Dirt-1-2048-normal.png");
			//pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"normal-map-bumpy.png");
			pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"water_new_height.png");

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			//pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			pVolume->SetMaterialRefractivity(0.5f);
			pVolume->SetMaterialReflectivity(0.5f);

			pTestContext->pVolume = pVolume;

			pSphere = m_pDreamOS->AddSphere(0.5f, 20, 20);
			CN(pSphere);
			pSphere->SetPosition(-2.0f, 0.0f, 0.0f);
			pSphere->SetMaterialReflectivity(0.9f);

			pTestContext->pSphere = pSphere;

			//cubemap *pCubemap = m_pDreamOS->MakeCubemap(L"LarnacaCastle");
			//CN(pCubemap);
			//
			//CR(dynamic_cast<OGLProgramStandard*>(pRenderProgramNode)->SetCubemap(pCubemap));

			/*
			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
			//*/

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp);
			CR(pDreamGamepadApp->SetCamera(m_pDreamOS->GetCamera(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));

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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->pSphere->translateY(-0.001f);
		//pTestContext->pVolume->translateY(0.001f);

		//pTestContext->pReflectionQuad->translateY(-0.0001f);
		//pTestContext->pReflectionQuad->RotateZByDeg(0.01f);
		//pTestContext->pWaterQuad->RotateXByDeg(0.002f);

		//m_pDreamOS->GetCamera()->translateZ(0.0001f);
		//m_pDreamOS->GetCamera()->translateY(0.0001f);

		if (pTestContext->pVolume != nullptr) {
			pTestContext->pVolume->RotateBy(0.00025f, 0.0005f, 0.000125f);
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

	pNewTest->SetTestName("Environment Shader");
	pNewTest->SetTestDescription("Environment shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestWaterShaderCube() {
	RESULT r = R_PASS;

	double sTestTime = 300.0f;
	int nRepeats = 1;

	struct TestContext {
		quad *pWaterQuad = nullptr;
		sphere *pSphere = nullptr;
		volume *pVolume = nullptr;
		quad *pLandQuad = nullptr;
	};
	TestContext *pTestContext = new TestContext();

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


		// Skybox

		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reflection 

		ProgramNode* pReflectionProgramNode;
		pReflectionProgramNode = pHAL->MakeProgramNode("reflection");
		CN(pReflectionProgramNode);
		CR(pReflectionProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReflectionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pReflectionSkyboxProgram;
		pReflectionSkyboxProgram = pHAL->MakeProgramNode("skybox");
		CN(pReflectionSkyboxProgram);
		CR(pReflectionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", pReflectionProgramNode->Output("output_framebuffer")));

		// Refraction

		ProgramNode* pRefractionProgramNode;
		pRefractionProgramNode = pHAL->MakeProgramNode("refraction");
		CN(pRefractionProgramNode);
		CR(pRefractionProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRefractionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		//ProgramNode* pRefractionSkyboxProgram;
		//pRefractionSkyboxProgram = pHAL->MakeProgramNode("skybox");
		//CN(pRefractionSkyboxProgram);
		//CR(pRefractionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer", pRefractionProgramNode->Output("output_framebuffer")));

		// "Water"

		ProgramNode* pWaterProgramNode;
		pWaterProgramNode = pHAL->MakeProgramNode("water");
		CN(pWaterProgramNode);
		//CR(pWaterProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pWaterProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		
		// TODO: This is not particularly general yet
		CR(pWaterProgramNode->ConnectToInput("input_refraction_map", pRefractionProgramNode->Output("output_framebuffer")));
		CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));
		
		// Standard Shader

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		
		CR(pRenderProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));
		
		// Skybox

		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReflectionSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display
		//CR(pDestSinkNode->ConnectToAllInputs(pSkyboxProgram->Output("output_framebuffer")));
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			vector vLightDirection = vector(1.0f, -1.0f, 0.0f);
			float lightIntensity = 1.0f;
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLightDirection);
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.70f * lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f * vLightDirection));

			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_bump.jpg");

			texture *pBumpTextureWater;

			//pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"Dirt-1-2048-normal.png");
			//pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"normal-map-bumpy.png");
			pBumpTextureWater = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"water_new_height.png");

			/*
			pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
			CN(pTestContext->pSphere);
			pTestContext->pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));
			pTestContext->pSphere->SetDiffuseTexture(pColorTexture);
			pTestContext->pSphere->SetBumpTexture(pBumpTexture);

			pTestContext->pVolume = m_pDreamOS->AddVolume(0.5f);
			CN(pTestContext->pVolume);
			pTestContext->pVolume->SetPosition(point(0.0f, -0.5f, 0.0f));
			CR(pTestContext->pVolume->SetVertexColor(COLOR_WHITE));
			pTestContext->pVolume->SetDiffuseTexture(pColorTexture);
			pTestContext->pVolume->SetBumpTexture(pBumpTexture);
			*/

			//pReflectionQuad = m_pDreamOS->AddQuad(5.0f, 5.0f, 1, 1);
			//pTestContext->pReflectionQuad = m_pDreamOS->MakeQuad(5.0f, 5.0f, 1, 1, nullptr, vector::jVector());
			pTestContext->pWaterQuad = m_pDreamOS->MakeQuad(1000.0f, 1000.0f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f).Normal());
			CN(pTestContext->pWaterQuad);
			pTestContext->pWaterQuad->SetPosition(90.0f, -1.25f, -25.0f);
			pTestContext->pWaterQuad->SetMaterialDisplacement(0.1f);
			pTestContext->pWaterQuad->SetMaterialShininess(16.0f);

			color cWater;
			cWater.SetColor(57.0f / 255.0f, 112.0f / 255.0f, 151.0f / 255.0f, 1.0f);
			pTestContext->pWaterQuad->SetMaterialDiffuseColor(cWater);
			
			//pTestContext->pWaterQuad->SetPosition(0.0f, -0.1f, 0.0f);
			
			//pTestContext->pWaterQuad->SetBumpTexture(pBumpTextureWater);
			//pTestContext->pReflectionQuad->RotateZByDeg(45.0f);
			//pReflectionQuad->SetDiffuseTexture(dynamic_cast<OGLProgram*>(pReflectionProgramNode)->GetOGLFramebufferColorTexture());


			/*
			point ptSceneOffset = point(90, -5.0, -25);
			float sceneScale = 0.025f;
			vector vSceneEulerOrientation = vector(0.0f, 0.0f, 0.0f);
			
			model *pCaveModel = m_pDreamOS->AddModel(L"\\Cave\\cave.FBX");
			//model *pCaveModel = m_pDreamOS->AddModel(L"\\Cave\\cave_exported.fbx");
			CN(pCaveModel);
			pCaveModel->SetScale(sceneScale);

			m_pDreamOS->GetCamera()->SetPosition(-5.0f, 4.0f, -5.0f);
			m_pDreamOS->GetCamera()->RotateYByDeg(90.0f);
			*/

			///*
			texture *pLandColorTexture;
			texture *pLandHeightTexture;

			pLandColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"island-diffuse.jpg");
			pLandHeightTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"island-height.jpg");
			
			pTestContext->pLandQuad = m_pDreamOS->AddQuad(50.0f, 50.0f, 500, 500, pLandHeightTexture);
			CN(pTestContext->pLandQuad);
			pTestContext->pLandQuad->SetDiffuseTexture(pLandColorTexture);
			pTestContext->pLandQuad->SetPosition(0.0f, -0.75f, 0.0f);

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 0.25f, 20.0f);
			pTestContext->pWaterQuad->SetPosition(0.0f, -0.1f, 0.0f);
			//*/

			if (pWaterProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(pTestContext->pWaterQuad));
			}

			if (pReflectionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramReflection*>(pReflectionProgramNode)->SetReflectionObject(pTestContext->pWaterQuad));
			}

			if (pRefractionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramRefraction*>(pRefractionProgramNode)->SetRefractionObject(pTestContext->pWaterQuad));
			}

			if (pReflectionSkyboxProgram != nullptr) {
				CR(dynamic_cast<OGLProgramSkybox*>(pReflectionSkyboxProgram)->SetReflectionObject(pTestContext->pWaterQuad));
			}

			// NOTE: Refraction skybox needs no reflection plane - it's just looking through

			// TOOD: Test clipping
			//sphere *pSphere;
			//pSphere = m_pDreamOS->AddSphere(0.125f, 10, 10);
			//CN(pSphere);
			//pSphere->SetPosition(point(0.0f, -1.15f, 0.0f));

			//m_pDreamOS->GetCamera()->SetPosition(0.0f, -1.1f, 10.0f);

			/*
			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
			*/

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp)
			
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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->pSphere->translateY(-0.001f);
		//pTestContext->pVolume->translateY(0.001f);

		//pTestContext->pReflectionQuad->translateY(-0.0001f);
		//pTestContext->pReflectionQuad->RotateZByDeg(0.01f);
		//pTestContext->pWaterQuad->RotateXByDeg(0.002f);

		//m_pDreamOS->GetCamera()->translateZ(0.0001f);
		//m_pDreamOS->GetCamera()->translateY(0.0001f);

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

	pNewTest->SetTestName("Environment Shader");
	pNewTest->SetTestDescription("Environment shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestStandardShader() {
	RESULT r = R_PASS;

	double sTestTime = 400.0f;
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(1.0f, -1.0f, -1.0f));

		

//#ifndef _DEBUG
#if 1
		{
			point ptSceneOffset = point(90, -5, -25);
			//float sceneScale = 0.025f;
			float sceneScale = 0.1f;
			vector vSceneEulerOrientation = vector(0.0f, 0.0f, 0.0f);

			//model* pModel = m_pDreamOS->AddModel(L"\\FloatingIsland\\env.obj");
			//pModel->SetPosition(ptSceneOffset);
			//pModel->SetScale(sceneScale);
			////pModel->SetEulerOrientation(vSceneEulerOrientation);
			//
			//model* pRiver = m_pDreamOS->AddModel(L"\\FloatingIsland\\river.obj");
			//pRiver->SetPosition(ptSceneOffset);
			//pRiver->SetScale(sceneScale);
			////pModel->SetEulerOrientation(vSceneEulerOrientation);
			//
			model* pClouds = m_pDreamOS->AddModel(L"\\FloatingIsland\\clouds.obj");
			pClouds->SetPosition(ptSceneOffset);
			pClouds->SetScale(sceneScale);
			//pModel->SetEulerOrientation(vSceneEulerOrientation);
			
			// Need true for children on models (might want to override)
			pClouds->SetMaterialAmbient(0.8f, true);

			//model *pCaveModel = m_pDreamOS->AddModel(L"\\Cave\\cave.fbx");
			//CN(pCaveModel);
			//pCaveModel->SetScale(sceneScale);

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 1.0f, -2.0f);
			m_pDreamOS->GetCamera()->RotateYByDeg(90.0f);
		}
#else

		texture *pColorTexture;
		pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");

		texture *pBumpTexture;
		pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_bump.jpg");

		sphere *pSphere;
		pSphere = m_pDreamOS->AddSphere(0.5f, 20, 20);
		CN(pSphere);
		pSphere->SetDiffuseTexture(pColorTexture);
		pSphere->SetBumpTexture(pBumpTexture);

		///*
		volume *pVolume;
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetVertexColor(COLOR_WHITE));
		pVolume->SetDiffuseTexture(pColorTexture);
		pVolume->SetBumpTexture(pBumpTexture);

		/*
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetVertexColor(COLOR_GREEN));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetVertexColor(COLOR_RED));

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetVertexColor(COLOR_BLUE));
		*/
#endif

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

RESULT HALTestSuite::AddTestFlatContextNesting() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	struct TestContext {
		std::shared_ptr<quad> pInnerQuads[4] = { nullptr, nullptr, nullptr, nullptr };
		composite *pComposite = nullptr;
		std::shared_ptr<FlatContext> pFlatContext = nullptr;
		std::shared_ptr<composite> pInnerComposite = nullptr;
		quad *pRenderQuad = nullptr;
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		float spacing = 0.5f;
		float side = 0.25f;

		// Set up the pipeline
		CR(SetupSkyboxPipeline("environment"));

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			pTestContext->pComposite = m_pDreamOS->AddComposite();
			CN(pTestContext->pComposite);
			pTestContext->pComposite->InitializeOBB();

			pTestContext->pFlatContext = pTestContext->pComposite->AddFlatContext();
			CN(pTestContext->pFlatContext);
			pTestContext->pFlatContext->SetIsAbsolute(true);
			pTestContext->pFlatContext->SetAbsoluteBounds(2.0f, 2.0f);

			pTestContext->pInnerComposite = pTestContext->pFlatContext->AddComposite();
			CN(pTestContext->pInnerComposite);
			pTestContext->pInnerComposite->InitializeOBB();

			auto pQuad = pTestContext->pInnerQuads[0] = pTestContext->pInnerComposite->AddQuad(side, side);
			CN(pQuad);
			pQuad->SetPosition(-spacing, 0.0f, -spacing);
			pQuad->SetMaterialColors(color(COLOR_RED));
			pQuad->SetVertexColor(color(COLOR_RED));

			pQuad = pTestContext->pInnerQuads[1] = pTestContext->pInnerComposite->AddQuad(side, side);
			CN(pQuad);
			pQuad->SetPosition(spacing, 0.0f, -spacing);
			pQuad->SetMaterialColors(color(COLOR_BLUE));
			pQuad->SetVertexColor(color(COLOR_BLUE));
			
			pQuad = pTestContext->pInnerQuads[2] = pTestContext->pInnerComposite->AddQuad(side, side);
			CN(pQuad);
			pQuad->SetPosition(-spacing, 0.0f, spacing);
			pQuad->SetMaterialColors(color(COLOR_GREEN));
			pQuad->SetVertexColor(color(COLOR_GREEN));
			
			pQuad = pTestContext->pInnerQuads[3] = pTestContext->pInnerComposite->AddQuad(side, side);
			CN(pQuad);
			pQuad->SetPosition(spacing, 0.0f, spacing);
			pQuad->SetMaterialColors(color(COLOR_YELLOW));
			pQuad->SetVertexColor(color(COLOR_YELLOW));

			pTestContext->pFlatContext->SetPosition(0.0f, -2.0f, 0.0f);
			//pTestContext->pFlatContext->translateX(0.25f);
			//pTestContext->pInnerComposite->translateX(0.25f);

			pTestContext->pRenderQuad = m_pDreamOS->AddQuad(2.0f, 2.0f);
			CN(pTestContext->pRenderQuad);
			pTestContext->pRenderQuad->RotateXByDeg(90.0f);


		}


	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->pComposite->RotateYByDeg(0.035f);
		//pTestContext->pVolume[2]->RotateYByDeg(0.035f);

		CR(pTestContext->pFlatContext->RenderToQuad(pTestContext->pRenderQuad, 0, 0));

		//pTestContext->pComposite->translateX(0.001f);
		//pTestContext->pComposite->translateY(0.001f);
		//pTestContext->pComposite->translateZ(0.001f);
		//
		//pTestContext->pComposite->RotateXByDeg(0.01f);
		//pTestContext->pComposite->RotateYByDeg(0.01f);
		//pTestContext->pComposite->RotateZByDeg(0.01f);
		
		pTestContext->pInnerQuads[0]->RotateXByDeg(0.05f);
		pTestContext->pInnerQuads[1]->RotateYByDeg(0.05f);
		pTestContext->pInnerQuads[2]->RotateZByDeg(0.05f);

		//pTestContext->pInnerComposite->translateX(0.001f);
		//pTestContext->pInnerComposite->translateY(0.001f);
		//pTestContext->pInnerComposite->translateZ(0.001f);

		//pTestContext->pInnerComposite->RotateXByDeg(0.01f);
		//pTestContext->pInnerComposite->RotateYByDeg(0.01f);
		//pTestContext->pInnerComposite->RotateZByDeg(0.01f);
		
		pTestContext->pFlatContext->translateX(0.001f);
		pTestContext->pFlatContext->translateY(0.001f);
		pTestContext->pFlatContext->translateZ(0.001f);
		
		pTestContext->pFlatContext->RotateXByDeg(0.01f);
		pTestContext->pFlatContext->RotateYByDeg(0.01f);
		pTestContext->pFlatContext->RotateZByDeg(0.01f);

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

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::TestNestedOBB() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	struct TestContext {
		composite *pComposite = nullptr;
	} *pTestContext = new TestContext();

	float width = 0.5f;
	float height = width;
	float length = width;
	float padding = 0.75f;

	int numDimension = 5;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		CR(SetupSkyboxPipeline("environment"));

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		{
			pTestContext->pComposite = m_pDreamOS->AddComposite();
			CN(pTestContext->pComposite);

			pTestContext->pComposite->InitializeOBB();
			pTestContext->pComposite->SetPosition(0.0f, 0.0f, -5.0f);
			pTestContext->pComposite->Scale(0.5f);

			for (int i = 0; i < numDimension; i++) {

				float xPos = -((float)numDimension / 2.0f) * (width + padding) + ((width + padding) * i);

				for (int j = 0; j < numDimension; j++) {

					float yPos = -((float)numDimension / 2.0f) * (width + padding) + ((width + padding) * j);

					for (int k = 0; k < numDimension; k++) {

						float zPos = -((float)numDimension / 2.0f) * (width + padding) + ((width + padding) * k);

						auto pVolume = pTestContext->pComposite->AddVolume(width);
						CN(pVolume);

						pVolume->SetPosition(point(xPos, yPos, zPos));
					}
				}
			}
		}


	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->pComposite->RotateYByDeg(0.035f);
		//pTestContext->pVolume[2]->RotateYByDeg(0.035f);

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

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestRotation() {
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

		CR(SetupSkyboxPipeline("minimal"));

		// Objects 

		DimRay *pRay;
		pRay = nullptr;

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(-1.0f);

			pRay->RotateZByDeg(45.0f * i);
		}

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(0.0f);

			pRay->RotateZBy(M_PI_4 * i);
		}

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(1.0f);

			pRay->SetRotate(0.0f, 0.0f, (M_PI_4 * i));
		}

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(2.0f);

			pRay->SetRotateDeg(0.0f, 0.0f, (45.0f * i));
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

RESULT HALTestSuite::AddTestUserModel() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	struct TestContext {
		user *pUser = nullptr;
		composite *pComposite = nullptr;
		std::shared_ptr<model> pModel = nullptr;
	} *pTestContext = new TestContext();

	float width = 5.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("environment"));

		// Objects 
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		volume *pVolume;
		pVolume = nullptr;
		sphere *pSphere;
		pSphere = nullptr;

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		///*
		{
			pTestContext->pComposite = m_pDreamOS->AddComposite();
			CN(pTestContext->pComposite);
			pTestContext->pComposite->InitializeOBB();
			pTestContext->pComposite->SetPosition(0.0f, -2.0f, 0.0f);
			
			//auto pModel = pTestContext->pComposite->AddModel(L"\\face4\\untitled.obj");
			//pTestContext->pComposite->SetPosition(point(0.0f, -1.0f, -5.0f));
			//pTestContext->pComposite->SetScale(0.1f);

			//pTestContext->pModel = pTestContext->pComposite->AddModel(L"cube.obj");
			//CN(pTestContext->pModel);

			pTestContext->pUser = m_pDreamOS->AddUser();
			CN(pTestContext->pUser);
			pTestContext->pUser->SetPosition(0.0f, -2.0f, 0.0f);
			pTestContext->pUser->UpdateMouth(1.0f);

			pTestContext->pUser->GetHead()->RotateYByDeg(0.035f);
		}
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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		if (pTestContext->pUser != nullptr) {
			pTestContext->pUser->RotateYByDeg(0.035f);
			//pTestContext->pUser->GetHead()->RotateYByDeg(0.035f);
		}
		else if (pTestContext->pComposite != nullptr) {
			pTestContext->pComposite->RotateYByDeg(0.035f);
		}
		else if (pTestContext->pModel != nullptr) {
			pTestContext->pModel->RotateYByDeg(0.035f);
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

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: This is a deeper project
RESULT HALTestSuite::AddTestModelInstancing() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	struct TestContext {
		model *pModel = nullptr;
	} *pTestContext = new TestContext();

	float width = 5.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("environment"));

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		{
			pTestContext->pModel = m_pDreamOS->MakeModel(L"\\face4\\untitled.obj");
			//pTestContext->pModel->SetPosition(point(-7.0f + i * 1.5f, 0.0f, -5.0f));
			//pTestContext->pModel->SetScale(0.03f);

			for (int i = 0; i < 10; i++) {

				auto pComposite = m_pDreamOS->AddComposite();

				pComposite->AddObject(std::shared_ptr<model>(pTestContext->pModel));
				CN(pComposite);

				pComposite->SetPosition(point(-7.0f + i * 1.5f, 0.0f, -5.0f));
				pComposite->SetScale(0.03f);
			}
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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->pModel->RotateYByDeg(0.035f);

		//ObjectStoreImp *pObjectStoreImp = m_pDreamOS->GetUISceneGraphNode()->GetSceneGraphStore();
		//VirtualObj *pVirtualObj = nullptr;
		//
		//CN(pObjectStoreImp);
		//
		//m_pDreamOS->GetUISceneGraphNode()->Reset();
		//while ((pVirtualObj = pObjectStoreImp->GetNextObject()) != nullptr) {
		//	pVirtualObj->translateX(0.001f);
		//}

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

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// This will test the HAL model 
RESULT HALTestSuite::AddTestModel() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	struct TestContext {
		model *pModel = nullptr;
	} *pTestContext = new TestContext();

	float width = 5.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		//CR(SetupSkyboxPipeline("blinnphong_texture"));
		CR(SetupSkyboxPipeline("standard"));

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		volume *pVolume;
		pVolume = nullptr;
		
		sphere *pSphere;
		pSphere = nullptr;
		
		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));
		//light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{
			/*
			pTestContext->pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			pTestContext->pModel->SetPosition(point(40.0f, -10.0f, -100.0f));

			pTestContext->pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			pTestContext->pModel->SetPosition(point(-40.0f, -10.0f, -100.0f));
			//*/

			///*
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\car\\untitled.obj",
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\FloatingIsland\\env.obj",
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\dodgeviper\\fbx.FBX",
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\converse\\converse_fbx.FBX",
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\home\\model.fbx",
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\shelby\\Shelby.fbx",
			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\converse\\converse_fbx.fbx",

			//pTestContext->pModel = m_pDreamOS->AddModel(L"\\Avatars\\righthand_1_ascii.FBX");
			PathManager *pPathManager = PathManager::instance();
			std::wstring wstrAssetPath;
			pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);
			std::wstring wstrHeadModel = wstrAssetPath + L"/camera/camera.fbx";
			//std::wstring wstrHeadModel = wstrAssetPath + L"/avatar/" + std::to_wstring(6) + L"/head.fbx";
			pTestContext->pModel = m_pDreamOS->AddModel(L"//camera//camera.fbx");
			pTestContext->pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
			pTestContext->pModel->SetScale(0.003f);

			/*
			pTestContext->pModel = m_pDreamOS->AddModel(L"\\nanosuit\\nanosuit.obj");
			pTestContext->pModel->SetPosition(point(0.0f, -5.0f, -8.0f));
			pTestContext->pModel->SetScale(0.5f);
			//*/

			//pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20, COLOR_RED);
			//CN(pSphere);
			//pSphere->SetPosition(point(2.0f, 0.0f, 0.0f));
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
		RESULT r = R_PASS;
  
    TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->pModel->RotateYByDeg(0.035f);
  
		ObjectStoreImp *pObjectStoreImp;
		pObjectStoreImp = m_pDreamOS->GetUISceneGraphNode()->GetSceneGraphStore();
		
		VirtualObj *pVirtualObj;
		pVirtualObj = nullptr;

		CN(pObjectStoreImp);

		m_pDreamOS->GetUISceneGraphNode()->Reset();

		while ((pVirtualObj = pObjectStoreImp->GetNextObject()) != nullptr) {
			pVirtualObj->translateX(0.001f);
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

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestModelOrientation() {
	RESULT r = R_PASS;

	double sTestTime = 70.0f;
	int nRepeats = 1;

	struct TestContext {
		model *pModelRight = nullptr;
		model *pModelLeft = nullptr;
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("environment");
		//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
		//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		///*
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//*/

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 4.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		{
			/*
			pTestContext->pModelLeft = m_pDreamOS->AddModel(L"\\face4\\LeftHand.obj");
			pTestContext->pModelLeft->SetPosition(point(0.0f, -5.0f, -8.0f));
			pTestContext->pModelLeft->SetScale(0.25f);
			pTestContext->pModelLeft->SetOrientationOffset((float)(-M_PI_2), (float)(M_PI_2), 0.0f);

			pTestContext->pModelRight = m_pDreamOS->AddModel(L"\\face4\\RightHand.obj");
			pTestContext->pModelRight->SetPosition(point(0.0f, -5.0f, -8.0f));
			pTestContext->pModelRight->SetScale(0.25f);
			pTestContext->pModelRight->SetOrientationOffset((float)(-M_PI_2), (float)(-M_PI_2), 0.0f);
			*/

			//texture *pColorTexture2 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"crate_color.png");

			pTestContext->pModelLeft = m_pDreamOS->AddModel(L"\\OculusTouch\\RightController\\oculus_cv1_controller_right.obj");
			CN(pTestContext->pModelLeft);

			pTestContext->pModelLeft->SetScale(30.0f);
			
			//pTestContext->pModelLeft->GetFirstChild<mesh>()->SetMaterialDiffuseColor(color(COLOR_WHITE));

			/*
			pTestContext->pModelRight = m_pDreamOS->AddModel(L"\\OculusTouch\\LeftController\\oculus_cv1_controller_left.obj");
			CN(pTestContext->pModelRight);

			//pTestContext->pModelRight->SetPosition(point(0.0f, -5.0f, -8.0f));
			pTestContext->pModelRight->SetOrientationOffset((float)(-M_PI_2), (float)(-M_PI_2), 0.0f);
			*/
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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		pTestContext->pModelLeft->RotateYByDeg(0.035f);

		/*
		hand *pLeftHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_LEFT);
		hand *pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

		if (pLeftHand != nullptr) {
			point ptHand = pLeftHand->GetPosition();
			quaternion qHand = pLeftHand->GetHandState().qOrientation;
			qHand.Normalize();

			pTestContext->pModelLeft->SetOrientation(qHand);
		}

		if (pRightHand != nullptr) {
			point ptHand = pRightHand->GetPosition();
			quaternion qHand = pRightHand->GetHandState().qOrientation;
			qHand.Normalize();

			pTestContext->pModelRight->SetOrientation(qHand);
		}
		*/

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

	pNewTest->SetTestName("HAL Model Test");
	pNewTest->SetTestDescription("HAL Model test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestHeightQuadObject() {
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.1f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		{
			//quad *pWaterQuad = m_pDreamOS->MakeQuad(20.0f, 20.0f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f).Normal());
			//CN(pWaterQuad);
			//pWaterQuad->SetPosition(0.0f, -1.5f, 0.0f);
			//pWaterQuad->SetBumpTexture(pBumpTextureWater);
			//pTestContext->pReflectionQuad->RotateZByDeg(45.0f);
			//pReflectionQuad->SetDiffuseTexture(dynamic_cast<OGLProgram*>(pReflectionProgramNode)->GetOGLFramebufferColorTexture());

			texture *pLandColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"island-diffuse.jpg");
			texture *pLandHeightTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"island-height.jpg");

			quad *pLandQuad = m_pDreamOS->AddQuad(20.0f, 20.0f, 200, 200, pLandHeightTexture);
			CN(pLandQuad);
			pLandQuad->SetDiffuseTexture(pLandColorTexture);
			pLandQuad->SetPosition(0.0f, -2.0f, -5.0f);
			
			m_pDreamOS->GetCamera()->SetPosition(0.0f, 5.0f, 10.0f);

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

	pNewTest->SetTestName("UI Shader Stage Test");
	pNewTest->SetTestDescription("UI Shader Stage shader test");

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("environment");
		//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");

		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));
		
		{

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

			pFlatContext->translateY(1.0f);
			pFlatContext->translateZ(-1.0f);

			pFlatContext->RenderToQuad(quad::CurveType::FLAT);
			pFlatContext->RotateXByDeg(90.0f);


			pFlatContext = m_pDreamOS->Add<FlatContext>(1024, 1024, 4);
			CN(pFlatContext);

			pQuad = pFlatContext->AddQuad(1.0f, 1.0f);
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

			pFlatContext->translateY(1.0f);
			pFlatContext->translateZ(-1.0f);

			//pFlatContext->RenderToQuad(quad::CurveType::PARABOLIC);
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

	pNewTest->SetTestName("UI Shader Stage Test");
	pNewTest->SetTestDescription("UI Shader Stage shader test");

	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);
Error:
	return r;
}

RESULT HALTestSuite::AddTestUIShaderStage() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	float width = 1.0f;

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("environment");
		//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");

		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// UI shader stage 
		ProgramNode* pUIStageProgram;
		pUIStageProgram = pHAL->MakeProgramNode("uistage");
		CN(pUIStageProgram);
		CR(pUIStageProgram->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
		CR(pUIStageProgram->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
		CR(pUIStageProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pUIStageProgram->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIStageProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;
		
		quad *pQuad;
		pQuad = nullptr;

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		{
			/*
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
			*/

			for (int i = -4; i < 5; i++) {
				pQuad = m_pDreamOS->MakeQuad(1.0f, 1.0f);
				CN(pQuad);
				pQuad->RotateXByDeg(90.0f);
				pQuad->translateX(i * 1.05f);
				CR(m_pDreamOS->AddObjectToUIGraph(pQuad));
			}
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("environment");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram;
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Screen Quad Shader (opt - we could replace this if we need to)
		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);

		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Connect Program to Display

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		
		{
			auto pFlatContext = m_pDreamOS->AddFlatContext();

			auto pComposite = m_pDreamOS->AddComposite();
			auto pFont = std::make_shared<font>(L"Basis_Grotesque_Pro.fnt", pComposite, true);

			texture *pColorTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"Fonts/Basis_Grotesque_Pro.png");
			auto pTextLetter = pFlatContext->AddText(pFont, pFont->GetTexture().get(), "hi", 1.0f, true);

			m_pDreamOS->RenderToTexture(pFlatContext);

			auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));
			pQuad->SetDiffuseTexture(m_pDreamOS->MakeTexture(*(pFlatContext->GetFramebuffer()->GetColorTexture())));

			/*
			pFlatContext->ClearChildren();
			
			pTextLetter = pFlatContext->AddText(pFont, pFont->GetTexture().get(), "hello", 1.0f, true);
			m_pDreamOS->RenderToTexture(pFlatContext);
			
			pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(1.0f, 0.0f, 0.0f));
			pQuad->SetDiffuseTexture(m_pDreamOS->MakeTexture(*(pFlatContext->GetFramebuffer()->GetColorTexture())));
			*/

			
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

RESULT HALTestSuite::AddTestBlinnPhongShaderTextureBumpDisplacement() {
	RESULT r = R_PASS;

	double sTestTime = 200.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		quad *m_pQuad = nullptr;
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_texture_bump_displacement");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		volume *pVolume;
		pVolume = nullptr;

		light *pLight;
		//pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.5f, point(0.0f, 1.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 1.0f));
		pLight = m_pDreamOS->AddLight(LIGHT_POINT, 0.5f, point(0.0f, 1.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f, -1.0f, 0.0f));

		{
			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"bricks2_diffuse.jpg");
			texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"bricks2_normal.jpg");

			//texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			//texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_bump.jpg");
			
			//texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"PyramidNormal_01.jpg");
			texture *pDisplacementTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"bricks2_displacement.jpg");

			pTestContext->m_pQuad = m_pDreamOS->AddQuad(5.0f, 5.0f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pTestContext->m_pQuad);
			pTestContext->m_pQuad->SetMaterialShininess(25.0f);

			//pTestContext->m_pQuad->RotateXByDeg(90.0f);

			pTestContext->m_pQuad->SetPosition(0.0f, 0.0f, 0.0f);
			pTestContext->m_pQuad->SetDiffuseTexture(pColorTexture);
			pTestContext->m_pQuad->SetBumpTexture(pBumpTexture);
			pTestContext->m_pQuad->SetDisplacementTexture(pDisplacementTexture);
			pTestContext->m_pQuad->SetMaterialDisplacement(0.1f);

			/*
			texture *pColorTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			texture *pColorTexture2 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"crate_color.png");

			texture *pColorTextureCopy = m_pDreamOS->MakeTexture(*pColorTexture1);

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			pVolume->SetDiffuseTexture(pColorTexture1);
			//CR(pVolume->SetColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(width, 0.0f, (length + padding) * -3.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pVolume->SetDiffuseTexture(pColorTexture2);

			auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(width, 0.0f, (length + padding) * -0.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pQuad->SetDiffuseTexture(pColorTextureCopy);

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
			//*/

			/*
			auto pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			CN(pModel);
			pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
			pModel->SetScale(0.1f);
			//*/

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 1.0f, 3.5f);

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp)
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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->m_pQuad->RotateYByDeg(0.005f);
		//pTestContext->m_pQuad->RotateZByDeg(0.005f);

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

	pNewTest->SetTestName("Blinn Phong Texture Normal Displacement Shader");
	pNewTest->SetTestDescription("Blinn phong texture bump maapping and displacement parallax shader test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestBlinnPhongShaderTextureBump() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		quad *m_pQuad = nullptr;
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_texture_bump");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		volume *pVolume;
		pVolume = nullptr;

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(1.0f, -1.0f, -1.0f));
		//pLight = m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(1.0f, -1.0f, -1.0f));
		
		//pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_GREEN), color(COLOR_GREEN), vector(-1.0f, -1.0f, 1.0f));
		//pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_BLUE), color(COLOR_BLUE), vector(-1.0f, -1.0f, -1.0f));

		{
			
			//texture *pColorTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"emboss-texture-256.jpg");
			//texture *pColorTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			
			texture *pBumpTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"PyramidNormal_01.jpg");
			//texture *pBumpTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"SimpleNormals.png");
			//texture *pBumpTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"diamond-pattern-bump.jpg");
			//texture *pBumpTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"emboss-normalmap-256.jpg");
			//texture *pBumpTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_bump.jpg");
			
			

			pTestContext->m_pQuad = m_pDreamOS->AddQuad(5.0f, 5.0f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pTestContext->m_pQuad);

			pTestContext->m_pQuad->RotateXByDeg(90.0f);

			pTestContext->m_pQuad->SetPosition(0.0f, 0.0f, 0.0f);
			//pTestContext->m_pQuad->SetDiffuseTexture(pColorTexture1);
			pTestContext->m_pQuad->SetBumpTexture(pBumpTexture1);

			/*
			texture *pColorTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			texture *pColorTexture2 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"crate_color.png");

			texture *pColorTextureCopy = m_pDreamOS->MakeTexture(*pColorTexture1);

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			pVolume->SetDiffuseTexture(pColorTexture1);
			//CR(pVolume->SetColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(width, 0.0f, (length + padding) * -3.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pVolume->SetDiffuseTexture(pColorTexture2);

			auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(width, 0.0f, (length + padding) * -0.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pQuad->SetDiffuseTexture(pColorTextureCopy);

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
			//*/

			/*
			auto pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			CN(pModel);
			pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
			pModel->SetScale(0.1f);
			//*/
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
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//pTestContext->m_pQuad->RotateYByDeg(0.01f);
		pTestContext->m_pQuad->RotateZByDeg(0.005f);

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

	pNewTest->SetTestName("Blinn Phong Texture Copy Shader");
	pNewTest->SetTestDescription("Blinn phong texture shader test with a textured copied over");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: There's a bug with directional lights here
RESULT HALTestSuite::AddTestBlinnPhongShaderTexture() {
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;
		
		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.3f, -1.0f, -1.0f));

		{
			/*
			texture *pColorTexture1 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			texture *pColorTexture2 = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"crate_color.png");

			texture *pColorTextureCopy = m_pDreamOS->MakeTexture(*pColorTexture1);

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			pVolume->SetDiffuseTexture(pColorTexture1);
			//CR(pVolume->SetColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(width, 0.0f, (length + padding) * -3.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pVolume->SetDiffuseTexture(pColorTexture2);

			auto pQuad = m_pDreamOS->AddQuad(width, height, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f).Normal());
			CN(pQuad);
			pQuad->SetPosition(point(width, 0.0f, (length + padding) * -0.0f));
			//CR(pVolume->SetColor(COLOR_GREEN));
			pQuad->SetDiffuseTexture(pColorTextureCopy);

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
			//*/

			auto pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
			CN(pModel);
			pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
			pModel->SetScale(0.1f);
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetPosition(point(0.0f, 1.0f, 4.0f));
		pTestContext->pVolume->SetVertexColor(COLOR_BLUE);

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderBlurQuad;
		pRenderBlurQuad = pHAL->MakeProgramNode("blur");
		CN(pRenderBlurQuad);
		CR(pRenderBlurQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderBlurQuad->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{
			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			CR(pVolume->SetVertexColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
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

	pNewTest->SetTestName("Blinn Phong Shader Blur");
	pNewTest->SetTestDescription("Blinn phong shader with blur test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestObjectMaterialsColors() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float radius = 1.2f;

	float padding = 0.5f;

	struct TestContext {
		sphere *pSphere = nullptr;
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode *pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		{
			TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			light *pLight = m_pDreamOS->AddLight(LIGHT_POINT, 2.0f, point(-5.0f, 5.0f, 5.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(1.0f, -1.0f, -1.0f));

			pTestContext->pSphere = m_pDreamOS->AddSphere(radius, 20, 20);
			CN(pTestContext->pSphere);

			float ambientVal = 0.0f;
			float shineVal = 5.0f;
			material tempMaterial = material(shineVal, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE), ambientVal);
			pTestContext->pSphere->SetMaterial(tempMaterial);
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
		RESULT r = R_PASS;
		
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			// Change materials 
			color curColor = pTestContext->pSphere->GetDiffuseColor();
			curColor.IncrementRGB(0.0001f);

			CR(pTestContext->pSphere->SetMaterialDiffuseColor(curColor));
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

	pNewTest->SetTestName("Material Color Test");
	pNewTest->SetTestDescription("Testing the changing of material color properties");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestCamera() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float radius = 0.2f;

	struct TestContext {
		volume *pVolume1 = nullptr;
		volume *pVolume2 = nullptr;
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CR(SetupSkyboxPipeline("minimal_texture"));

		pTestContext->pVolume1 = m_pDreamOS->AddVolume(0.1f);
		pTestContext->pVolume1->SetPosition(point(0.0f, 0.0f, 0.0f));
		
		pTestContext->pVolume2 = m_pDreamOS->AddVolume(0.1f);
		pTestContext->pVolume2->SetPosition(point(0.0f, 0.0f, -0.25f));

		m_pDreamOS->GetCamera()->SetPosition(point(0.0f, 0.0f, 0.0f));

		m_pDreamOS->RecenterHMD();

	Error:
		return r;
	};

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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Blinn Phong Shader HMD");
	pNewTest->SetTestDescription("Blinn phong shader HMD test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestObjectMaterialsBump() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float radius = 0.2f;
	int numObjs = 10;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode *pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 
	
		{
			light *pLight = m_pDreamOS->AddLight(LIGHT_POINT, 2.0f, point(-5.0f, 5.0f, 5.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(1.0f, -1.0f, -1.0f));
			
			sphere *pSphere = nullptr;
			texture *pBumpTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"PyramidNormal_01.jpg");

			for (int i = 0; i < numObjs; i++) {
				for (int j = 0; j < numObjs; j++) {
					pSphere = m_pDreamOS->AddSphere(radius, 20, 20);
					CN(pSphere);
					float xPos = radius * 2.5f * (((float)(i)) - (float)(numObjs) / 2.0f);
					float zPos = radius * 2.5f * (((float)(j)) - (float)(numObjs) / 2.0f);

					pSphere->SetPosition(point(xPos, zPos, 0.0f));

					float shineVal = (float)(i*numObjs + j + 1) / (float)((numObjs * numObjs));
					shineVal *= 100.0f;

					float ambientVal = 0.0f;

					material tempMaterial = material(shineVal, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE), ambientVal);

					pSphere->SetMaterial(tempMaterial);

					pSphere->SetBumpTexture(pBumpTexture);

					pSphere->SetMaterialBumpiness(0.5f);
					pSphere->SetMaterialUVTiling(2.0f, 2.0f);
				}
			}

			//m_pDreamOS->GetCamera()->RotateXByDeg(35.0f);

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

	pNewTest->SetTestName("Blinn Phong Shader HMD");
	pNewTest->SetTestDescription("Blinn phong shader HMD test");
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));		

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;
		sphere *pSphere;
		pSphere = nullptr;

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

		{

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			CR(pVolume->SetVertexColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));

			pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
			CN(pSphere);
			pSphere->SetPosition(point(width, 0.0f, 0.0f));
			CR(pSphere->SetVertexColor(COLOR_YELLOW));

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

	pNewTest->SetTestName("Blinn Phong Shader HMD");
	pNewTest->SetTestDescription("Blinn phong shader HMD test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestMouseDrag() {
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;

		{
			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			CR(pVolume->SetVertexColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
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

	pNewTest->SetTestName("Mouse Drag");
	pNewTest->SetTestDescription("Testing mouse dragging with minimal shader");
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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 
		
		volume *pVolume;
		pVolume = nullptr;

		{

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
			CR(pVolume->SetVertexColor(COLOR_GREEN));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
			CR(pVolume->SetVertexColor(COLOR_WHITE));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
			CR(pVolume->SetVertexColor(COLOR_RED));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
			CR(pVolume->SetVertexColor(COLOR_BLUE));
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

// TODO:
RESULT HALTestSuite::AddTestTextureUpdate() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		quad *pQuad = nullptr;
		texture *pTexture = nullptr;
		unsigned char *pLoadBuffer = nullptr;
		unsigned char *pUpdateBuffer = nullptr;
	} *pTestContext = new TestContext();

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		{
			auto pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			pTestContext->pQuad = m_pDreamOS->AddQuad(4.0f, 4.0f, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f));
			CN(pTestContext->pQuad);

			pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pTestContext->pTexture);

			size_t bufferSize = pTestContext->pTexture->GetTextureSize();
			pTestContext->pUpdateBuffer = (unsigned char *)malloc(bufferSize);
			CN(pTestContext->pUpdateBuffer);

			pTestContext->pLoadBuffer = (unsigned char *)malloc(bufferSize);
			CN(pTestContext->pLoadBuffer);

			CR(pTestContext->pQuad->SetDiffuseTexture(pTestContext->pTexture));
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
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		
		{
			// Download the texture to a buffer, change it, and update the texture
			int width = pTestContext->pTexture->GetWidth();
			int height = pTestContext->pTexture->GetHeight();
			int channels = pTestContext->pTexture->GetChannels();

			//unsigned char *pTempBuffer

			size_t bufferSize = pTestContext->pTexture->GetTextureSize();
			pTestContext->pTexture->LoadBufferFromTexture(pTestContext->pLoadBuffer, bufferSize);

			// Scroll the texture in binary
			for (int i = 0; i < height; i++) {
				size_t lineSize = width * channels * sizeof(unsigned char);

				int destI = i + 1;
				if (destI >= height)
					destI = 0;

				void *pSource = (void*)((pTestContext->pLoadBuffer) + (lineSize * i));
				void *pDest = (void*)((pTestContext->pUpdateBuffer) + (lineSize * destI));

				memcpy(pDest, pSource, lineSize);
			}


			// Upload the texture back to the texture
			pTestContext->pTexture->UpdateTextureFromBuffer(pTestContext->pUpdateBuffer, bufferSize);
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

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO:
RESULT HALTestSuite::AddTestPBOTextureUpload() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		quad *pQuad = nullptr;
		texture *pTexture = nullptr;
		unsigned char *pUpdateBuffer = nullptr;
	} *pTestContext = new TestContext();

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		{
			auto pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			pTestContext->pQuad = m_pDreamOS->AddQuad(3.0f, 3.0f, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f));
			CN(pTestContext->pQuad);

			pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pTestContext->pTexture);

			// Enable PBO unpack
			CR(dynamic_cast<OGLTexture*>(pTestContext->pTexture)->EnableOGLPBOUnpack());

			size_t bufferSize = pTestContext->pTexture->GetTextureSize();
			pTestContext->pUpdateBuffer = (unsigned char *)malloc(bufferSize);
			CN(pTestContext->pUpdateBuffer);

			CR(pTestContext->pTexture->LoadBufferFromTexture(pTestContext->pUpdateBuffer, bufferSize));

			CR(pTestContext->pQuad->SetDiffuseTexture(pTestContext->pTexture));
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
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			size_t bufferSize = pTestContext->pTexture->GetTextureSize();

			// Upload the texture back to the texture
			CR(pTestContext->pTexture->UpdateTextureFromBuffer(pTestContext->pUpdateBuffer, bufferSize));
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

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT HALTestSuite::AddTestPBOTextureReadback() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		quad *pQuad = nullptr;
		texture *pTexture = nullptr;
		unsigned char *pLoadBuffer = nullptr;
	} *pTestContext = new TestContext();

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		{
			auto pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			pTestContext->pQuad = m_pDreamOS->AddQuad(3.0f, 3.0f, 1, 1, nullptr, vector(0.0f, 0.0f, 1.0f));
			CN(pTestContext->pQuad);

			//pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"Brick_1280x720.jpg");
			CN(pTestContext->pTexture);

			size_t bufferSize = pTestContext->pTexture->GetTextureSize();
			pTestContext->pLoadBuffer = (unsigned char *)malloc(bufferSize);
			CN(pTestContext->pLoadBuffer);

			//CR(pTestContext->pTexture->LoadBufferFromTexture(pTestContext->pLoadBuffer, bufferSize));

			// Enable PBO pack
			CR(dynamic_cast<OGLTexture*>(pTestContext->pTexture)->EnableOGLPBOPack());

			CR(pTestContext->pQuad->SetDiffuseTexture(pTestContext->pTexture));
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
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			size_t bufferSize = pTestContext->pTexture->GetTextureSize();

			// Upload the texture back to the texture
			//CR(pTestContext->pTexture->UpdateTextureFromBuffer(pTestContext->pUpdateBuffer, bufferSize));

			pTestContext->pTexture->LoadBufferFromTexture(pTestContext->pLoadBuffer, bufferSize);
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

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

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

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects 

		volume *pVolume;
		pVolume = nullptr;

		{

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));

			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pColorTexture);

			CR(pVolume->SetDiffuseTexture(pColorTexture));
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

			ProgramNode* pRenderProgramNode;
			pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
			CN(pRenderProgramNode);
			CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			ProgramNode *pRenderScreenQuad;
			pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
			CN(pRenderScreenQuad);
			CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

			CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

			CR(pHAL->ReleaseCurrentContext());

			// Set up scene
			
			light *pLight;
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

			{

				composite *pComposite = m_pDreamOS->AddComposite();
				CN(pComposite);

				std::shared_ptr<FlatContext> pFlatContext = pComposite->MakeFlatContext();
				CN(pFlatContext);

				std::shared_ptr<quad> pFlatQuad = pFlatContext->AddQuad(0.25f, 0.25f, point(0.0f));
				CN(pFlatQuad);
				pFlatQuad->translateX(-0.5f);
				pFlatQuad->translateZ(-0.5f);

				pFlatQuad = pFlatContext->AddQuad(0.25f, 0.25f, point(0.0f));
				CN(pFlatQuad);
				pFlatQuad->translateX(0.5f);
				pFlatQuad->translateZ(0.5f);

				//pComposite->RenderToTexture(pFlatContext);
				pFlatContext->RenderToTexture();

				quad *pQuad = m_pDreamOS->AddQuad(width, height);
				CN(pQuad);
				CN(pQuad->SetPosition(point(0.0f, -2.0f, 0.0f)));
				pQuad->SetVertexColor(COLOR_GREEN);

				// TODO: this is no longer supported:
				CR(pQuad->SetDiffuseTexture(pFlatContext->GetFramebuffer()->GetColorTexture()));
			}
			
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

		// TODO: Add pipeline, this must be an old test

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetVertexColor(COLOR_GREEN));
		CR(pVolume->SetAlpha(alpha));


		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 1.0f));
		CR(pVolume->SetVertexColor(COLOR_WHITE));
		CR(pVolume->SetAlpha(alpha));
		
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetVertexColor(COLOR_RED));
		CR(pVolume->SetAlpha(alpha));
		
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetVertexColor(COLOR_BLUE));
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

RESULT HALTestSuite::AddTest3rdPersonCamera() {
	RESULT r = R_PASS;

	double sTestTime = 400.0f;

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

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode *pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		SinkNode *pAuxSinkNode;
		pAuxSinkNode = pPipeline->GetAuxiliarySinkNode();
		//CNM(pAuxSinkNode, "Aux sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		///*
		
		// Skybox
		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);

		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pSkyboxProgramNode;
		pSkyboxProgramNode = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgramNode);
		CR(pSkyboxProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgramNode->Output("output_framebuffer")));

		// Connect to output
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		//*/

		// Aux

		CameraNode* pAuxCamera;
		pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(2560, 1386, 60));
		//pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(3840, 2107, 60));
		CN(pAuxCamera);
		CB(pAuxCamera->incRefCount());

		// Skybox
		ProgramNode* pScatteringSkyboxProgram2;
		pScatteringSkyboxProgram2 = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram2);
		CR(pScatteringSkyboxProgram2->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));

		ProgramNode* pRenderProgramNode2;
		pRenderProgramNode2 = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode2);
		CR(pRenderProgramNode2->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode2->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));

		ProgramNode* pSkyboxProgramNode2;
		pSkyboxProgramNode2 = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgramNode2);
		CR(pSkyboxProgramNode2->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));
		CR(pSkyboxProgramNode2->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram2->Output("output_framebuffer_cube")));
		CR(pSkyboxProgramNode2->ConnectToInput("input_framebuffer", pRenderProgramNode2->Output("output_framebuffer")));

		// Connect to aux (we will likely need to reproduce the pipeline)
		if (pAuxSinkNode != nullptr) {
			CR(pAuxSinkNode->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));
			CR(pAuxSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgramNode2->Output("output_framebuffer")));
		}

		CR(pHAL->ReleaseCurrentContext());

		{

			auto pDreamGamepadCamera = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CR(pDreamGamepadCamera->SetCamera(m_pDreamOS->GetCamera(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));

			volume *pVolume;
			pVolume = nullptr;

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
			pVolume->RotateZByDeg(1.0f);

			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pColorTexture);

			CR(pVolume->SetDiffuseTexture(pColorTexture));
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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("3rd person camera test");
	pNewTest->SetTestDescription("Test 3rd person camera auxiliary sink node target");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestIrradianceMap() {
	RESULT r = R_PASS;

	double sTestTime = 500.0f;
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

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		// Skybox
		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pSkyboxConvolutionProgramNode;
		pSkyboxConvolutionProgramNode = pHAL->MakeProgramNode("cubemap_convolution");
		CN(pSkyboxConvolutionProgramNode);
		CR(pSkyboxConvolutionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxConvolutionProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

		ProgramNode* pRenderProgramNode;
		//pRenderProgramNode = pHAL->MakeProgramNode("irrandiance_map_lighting");
		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		//pRenderProgramNode = pHAL->MakeProgramNode("gbuffer");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("input_framebuffer_irradiance_cubemap", pSkyboxConvolutionProgramNode->Output("output_framebuffer_cube")));
		CR(pRenderProgramNode->ConnectToInput("input_framebuffer_environment_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		//ProgramNode *pSSAOProgram;
		//pSSAOProgram = pHAL->MakeProgramNode("ssao");
		//CN(pSSAOProgram);
		//CR(pSSAOProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));		

		ProgramNode* pVisualNormalsProgram;
		pVisualNormalsProgram = pHAL->MakeProgramNode("visualize_normals");
		//pVisualNormalsProgram = pHAL->MakeProgramNode("minimal");
		CN(pVisualNormalsProgram);
		CR(pVisualNormalsProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pVisualNormalsProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		
		CR(pVisualNormalsProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));
		
		ProgramNode* pSkyboxProgramNode;
		pSkyboxProgramNode = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgramNode);
		CR(pSkyboxProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		//CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer_cubemap", pSkyboxConvolutionProgramNode->Output("output_framebuffer_cube")));
		CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgramNode->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSSAOProgram->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToInput("input_framebuffer", pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		{
			//sphere *pSphere;
			//pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
			//CN(pSphere);

			//cubemap *pCubemap = m_pDreamOS->MakeCubemap(L"LarnacaCastle");
			//CN(pCubemap);

			//CR(dynamic_cast<OGLProgramSkybox*>(pRenderProgramNode)->SetCubemap(pCubemap));
			//CR(pRenderProgramNode->ConnectToInput("cubemap", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
			
			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pColorTexture);

			vector vSunDirection = vector(-1.0f, -0.25f, 0.1f);

			light *pLight;
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vSunDirection);
			
			sphere * pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
			CN(pSphere);

			volume *pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
			//CR(pVolume->SetDiffuseTexture(pColorTexture));

			//model *pModel = m_pDreamOS->AddModel(L"\\head_01\\head_01.FBX");
			model *pModel = m_pDreamOS->AddModel(L"\\4\\head.fbx");
			CN(pModel);
			pModel->SetPosition(point(2.0f, 0.0f, 0.0f));
			pModel->SetScale(0.05f);
			
			color modelColor = pModel->GetChildMesh(0)->GetDiffuseColor();
			pModel->SetMaterialSpecularColor(modelColor, true);
			pModel->SetMaterialShininess(4.0f, true);
			pModel->RotateYByDeg(45.0f);

			pModel = m_pDreamOS->AddModel(L"\\4\\left-hand.fbx");
			CN(pModel);
			pModel->SetPosition(point(3.5f, 0.0f, 0.0f));
			pModel->SetScale(0.05f);
			pModel->RotateXByDeg(-90.0f);
			pModel->RotateYByDeg(45.0f);

			modelColor = pModel->GetChildMesh(0)->GetDiffuseColor();
			pModel->SetMaterialSpecularColor(modelColor, true);
			pModel->SetMaterialShininess(4.0f, true);

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp);
			CR(pDreamGamepadApp->SetCamera(m_pDreamOS->GetCamera(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));

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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Irradiance Map Test");
	pNewTest->SetTestDescription("Test cube map based irradiance map shader");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestCubeMap() {
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

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		// Skybox
		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		ProgramNode* pSkyboxProgramNode;
		pSkyboxProgramNode = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgramNode);
		CR(pSkyboxProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pSkyboxProgramNode->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad;
		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		{

			volume *pVolume;
			pVolume = nullptr;

			//sphere *pSphere;
			//pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);
			//CN(pSphere);

			//cubemap *pCubemap = m_pDreamOS->MakeCubemap(L"LarnacaCastle");
			//CN(pCubemap);

			//CR(dynamic_cast<OGLProgramSkybox*>(pRenderProgramNode)->SetCubemap(pCubemap));
			//CR(pRenderProgramNode->ConnectToInput("cubemap", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			pVolume = m_pDreamOS->AddVolume(width, height, length);
			CN(pVolume);
			pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
			pVolume->RotateZByDeg(1.0f);

			texture *pColorTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pColorTexture);

			CR(pVolume->SetDiffuseTexture(pColorTexture));
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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Cube Map Test");
	pNewTest->SetTestDescription("Test cube map shaders and cube map pipeline nodes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}
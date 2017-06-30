#include "AnimationTestSuite.h"
#include "AnimationQueue.h"

#include "DreamOS.h"
#include "Primitives/composite.h"
#include "Primitives/sphere.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

AnimationTestSuite::AnimationTestSuite(DreamOS *pDreamOS) {
	// empty
	m_pDreamOS = pDreamOS;
}

AnimationTestSuite::~AnimationTestSuite() {
	// empty
}

RESULT AnimationTestSuite::AddTests() {
	RESULT r = R_PASS;
	CR(AddTestUIColor());
	CR(AddTestRotate());
	CR(AddTestCurves());
	CR(AddTestAnimationBasic());
	CR(AddTestCancel());
Error:
	return r;
}

RESULT AnimationTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT AnimationTestSuite::InitializeAnimationTest(void *pContext) {
	RESULT r = R_PASS;

	CR(SetupPipeline());

	AnimationTestContext *pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);

	pTestContext->pComposite = m_pDreamOS->AddComposite();
	CN(pTestContext->pComposite);
	pTestContext->pComposite->InitializeOBB();

	pTestContext->pSphere = m_pDreamOS->AddSphere(1.0f, 10.0f, 10.0f);
	pTestContext->pSphere->MoveTo(0.0f, 0.0f, -2.0f);
	CN(pTestContext->pSphere);

	CR(pTestContext->pSphere->InitializeOBB());

	pTestContext->pSphere2 = m_pDreamOS->AddSphere(1.0f, 10.0f, 10.0f);
	pTestContext->pSphere2->MoveTo(2.0f, 0.0f, -2.0f);
	CN(pTestContext->pSphere2);

	CR(pTestContext->pSphere2->InitializeOBB());

	pTestContext->fCancelled = false;

	pTestContext->startTime = std::chrono::high_resolution_clock::now();

Error:
	return r;
}

RESULT AnimationTestSuite::AddTestCurves() {
	RESULT r = R_PASS;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(SetupPipeline());

		sphere *m_pSphere1 = nullptr;
		sphere *m_pSphere2 = nullptr;
		sphere *m_pSphere3 = nullptr;
		sphere *m_pSphere4 = nullptr;

		m_pSphere1 = m_pDreamOS->AddSphere(0.5f, 10.0f, 10.0f);
		m_pSphere2 = m_pDreamOS->AddSphere(0.5f, 10.0f, 10.0f);
		m_pSphere3 = m_pDreamOS->AddSphere(0.5f, 10.0f, 10.0f);
		m_pSphere4 = m_pDreamOS->AddSphere(0.5f, 10.0f, 10.0f);

		m_pSphere1->MoveTo(-4.0f, 0.0f, 0.0f);
		m_pSphere2->MoveTo(-2.0f, 0.0f, 0.0f);
		m_pSphere3->MoveTo(0.0f, 0.0f, 0.0f);
		m_pSphere4->MoveTo(2.0f, 0.0f, 0.0f);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			m_pSphere1,
			m_pSphere1->GetPosition(),
			m_pSphere1->GetOrientation(),
			vector(2.0f),
			5.0f,
			AnimationCurveType::LINEAR,
			AnimationFlags()
		);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			m_pSphere2,
			m_pSphere2->GetPosition(),
			m_pSphere2->GetOrientation(),
			vector(2.0f),
			5.0f,
			AnimationCurveType::EASE_OUT_QUAD,
			AnimationFlags()
		);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			m_pSphere3,
			m_pSphere3->GetPosition(),
			m_pSphere3->GetOrientation(),
			vector(2.0f),
			5.0f,
			AnimationCurveType::EASE_OUT_QUART,
			AnimationFlags()
		);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			m_pSphere4,
			m_pSphere4->GetPosition(),
			m_pSphere4->GetOrientation(),
			vector(2.0f),
			5.0f,
			AnimationCurveType::EASE_OUT_BACK,
			AnimationFlags()
		);

	Error:
		return r;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	auto fnUpdate = fnTest;

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Animation Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(5.0);
	pNewTest->SetTestRepeats(1);

Error:
	return r;
}

RESULT AnimationTestSuite::AddTestRotate() {
	RESULT r = R_PASS;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(SetupPipeline());
		sphere *m_pSphere1 = nullptr;

		m_pSphere1 = m_pDreamOS->AddSphere(0.5f, 10.0f, 10.0f);
		m_pSphere1->MoveTo(0.0f, 0.0f, 0.0f);
		quaternion q;
		q.SetValues(1.0f, 0.0f, 0.0f, 0.0f);
		m_pSphere1->SetOrientation(q);

		quaternion qRotation = quaternion::MakeQuaternionWithEuler((float)(M_PI) / 2.0f, 0.0f, 0.0f);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			m_pSphere1,
			m_pSphere1->GetPosition() + point(1.0f, 0.0f, 0.0f),
			qRotation,
			m_pSphere1->GetScale(),
			2.0f,
			AnimationCurveType::LINEAR,
			AnimationFlags()
		);

	Error:
		return r;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	auto fnUpdate = fnTest;

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Animation Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(5.0);
	pNewTest->SetTestRepeats(1);
Error:
	return r;
}

RESULT AnimationTestSuite::AddTestUIColor() {
	RESULT r = R_PASS;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto cColor = color(0.0f, 0.0f, 1.0f, 0.0f);

//		CR(SetupColorTestPipeline());
		CR(SetupUINodePipeline());

		auto pComposite = m_pDreamOS->MakeComposite();
		m_pDreamOS->AddObjectToUIGraph(pComposite);

		texture* pPNG = m_pDreamOS->MakeTexture(L"icons_600\\icon_png_600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		quad *m_pQuad = pComposite->MakeQuad(1.0f, 1.0f).get();
		m_pQuad->MoveTo(0.0f, 0.0f, 0.0f);
		m_pQuad->GetMaterial()->SetColors(COLOR_GREEN, COLOR_GREEN, COLOR_GREEN);
		m_pQuad->SetColorTexture(pPNG);
		m_pDreamOS->AddObjectToUIGraph(m_pQuad);

		quaternion q;
		q.SetValues(1.0f, 0.0f, 0.0f, 0.0f);
		m_pQuad->SetOrientation(q);
		m_pQuad->RotateXByDeg(90.0f);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			m_pQuad,
			cColor,
			4.0,
			AnimationCurveType::LINEAR,
			AnimationFlags());

	Error:
		return r;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	auto fnUpdate = fnTest;

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Animation Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(10.0);
	pNewTest->SetTestRepeats(1);
Error:
	return r;
}

RESULT AnimationTestSuite::AddTestAnimationBasic() {

	RESULT r = R_PASS;

	AnimationTestContext *pTestContext = new AnimationTestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		AnimationQueue *pQueue = nullptr;
		AnimationState aState;
		AnimationState aState2;

		CR(InitializeAnimationTest(pContext));
		AnimationTestContext *pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);

		CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pComposite));
		aState.ptPosition = point(0.0f, 1.0f, -2.0f);
		aState.qRotation = quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		aState.vScale = vector(5.0f, 1.25f, 1.25f);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(pTestContext->pSphere, aState.ptPosition, aState.qRotation, aState.vScale, 2.0f, AnimationCurveType::LINEAR, AnimationFlags());
		
		aState2.ptPosition = point(1.0f, 0.0f, 0.0f);
		aState2.qRotation = quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		aState2.vScale = vector(1.0f, 1.0f, 1.0f);
		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(pTestContext->pSphere, aState2.ptPosition, aState2.qRotation, aState2.vScale, 2.0f, AnimationCurveType::LINEAR, AnimationFlags());

		aState2.ptPosition = point(2.0f, 1.0f, -2.0f);
		aState2.qRotation = quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		aState2.vScale = vector(0.5f, 0.5f, 0.5f);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(pTestContext->pSphere2, aState2.ptPosition, aState2.qRotation, aState2.vScale, 5.0f, AnimationCurveType::LINEAR, AnimationFlags());

	Error:
		return R_PASS;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	auto fnUpdate = fnTest;

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Animation Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(10.0);
	pNewTest->SetTestRepeats(1);
Error:
	return r;
}

RESULT AnimationTestSuite::AddTestCancel() {
	RESULT r = R_PASS;

	AnimationTestContext *pTestContext = new AnimationTestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		AnimationQueue *pQueue = nullptr;
		AnimationState aState;
		AnimationState aState2;

		CR(InitializeAnimationTest(pContext));
		AnimationTestContext *pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);

		CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pComposite));

		aState.ptPosition = point(0.0f, 1.0f, -2.0f);
		aState.vScale = vector(1.25f, 1.25f, 1.25f);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			pTestContext->pSphere, 
			aState.ptPosition, 
			pTestContext->pSphere->GetOrientation(),
			aState.vScale, 
			2.0f, 
			AnimationCurveType::LINEAR, 
			AnimationFlags());

	Error:
		return R_PASS;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		AnimationTestContext* pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);
		CN(pTestContext);

		auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - pTestContext->startTime).count();

		if (diff > 1.0 && !pTestContext->fCancelled) {
			m_pDreamOS->GetInteractionEngineProxy()->CancelAnimation(pTestContext->pSphere);
			pTestContext->fCancelled = true;
		}

	Error:
		return R_PASS;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Animation Cancel Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(5.0);
	pNewTest->SetTestRepeats(1);
Error:
	return r;

}

RESULT AnimationTestSuite::Notify(InteractionObjectEvent *event) {
	return R_PASS;
}

RESULT AnimationTestSuite::Notify(SenseKeyboardEvent *kbEvent) {
	return R_PASS;
}

RESULT AnimationTestSuite::SetupPipeline() {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
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

RESULT AnimationTestSuite::SetupUINodePipeline() {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	//CR(pHAL->MakeCurrentContext());

	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
//	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Skybox
	ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

//*
	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("minimal_texture");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//*/

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
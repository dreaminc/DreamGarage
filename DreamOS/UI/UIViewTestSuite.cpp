#include "UIViewTestSuite.h"

#include "DreamOS.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamControlView.h"

#include "UIView.h"
#include "UIButton.h"
#include "UIScrollView.h"
#include "UIMallet.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/UIStageProgram.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/hand.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include <string>
#include "Sense/SenseController.h"

UIViewTestSuite::UIViewTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIViewTestSuite::~UIViewTestSuite() {

}

RESULT UIViewTestSuite::Initialize() {

	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	m_pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	return R_PASS;
}

RESULT UIViewTestSuite::SetupPipeline() {
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

RESULT UIViewTestSuite::SetupUIStagePipeline(UIStageProgram* &pUIStageProgram) {
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
	//CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

/*
	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("minimal_texture");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//*/

//*
	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Connect output as pass-thru to internal blend program
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//*/
	pUIStageProgram = dynamic_cast<UIStageProgram*>(pUIProgramNode);

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

RESULT UIViewTestSuite::SetupUINodePipeline() {
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
	//CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

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

RESULT UIViewTestSuite::AddTests() {
	RESULT r = R_PASS;
	
	//CR(AddTestDreamUIBar());
	//CR(AddTestUIScrollView());
	//CR(AddTestUIButtons());
	//CR(AddTestUIButton());
	//CR(AddTestUIView());
	CR(AddTestKeyboardAngle());
	//CR(AddTestCurvedTitle());
	//CR(AddTestDreamControlView());

Error:
	return r;
}

RESULT UIViewTestSuite::DefaultCallback(void *pContext) {
	return R_PASS;
}

RESULT UIViewTestSuite::ResetTestCallback(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT UIViewTestSuite::UpdateHandRay(void *pContext) {
	RESULT r = R_PASS;

	ray rCast;
	CN(m_pDreamOS);
	hand *pHand = m_pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT);

	if (pHand != nullptr) {
		point ptHand = pHand->GetPosition();

		//GetLookVector
		quaternion qHand = pHand->GetHandState().qOrientation;
		qHand.Normalize();

		//TODO: investigate how to properly get look vector for controllers
		//vector vHandLook = qHand.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();

		vector vHandLook = RotationMatrix(qHand) * vector(0.0f, 0.0f, -1.0f);
		vHandLook.Normalize();

		rCast = ray(ptHand, vHandLook);
	}

//	m_pDreamOS->UpdateInteractionPrimitive(rCast);

Error:
	return r;
}

RESULT UIViewTestSuite::Rotate45(UIButton *pButtonContext, void *pContext) {
	pButtonContext->GetSurface()->RotateZByDeg(45.0f);
	return R_PASS;
};

RESULT UIViewTestSuite::Rotate15(UIButton *pButtonContext, void *pContext) {
	pButtonContext->GetSurface()->RotateZByDeg(15.0f);
	return R_PASS;
};

RESULT UIViewTestSuite::ResetRotation(UIButton *pButtonContext, void *pContext) {
	pButtonContext->GetSurface()->ResetRotation();
	return R_PASS;
};

RESULT UIViewTestSuite::AnimateScaleUp(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;

	DimObj *pObj = pButtonContext->GetSurface().get();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition(),
		pObj->GetOrientation(),
		vector(1.25f, 1.25, 1.25f),
		0.1,
		AnimationCurveType::LINEAR,
		AnimationFlags()
	));

Error:
	return r;
}

RESULT UIViewTestSuite::AnimateScaleReset(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;

	DimObj *pObj = pButtonContext->GetSurface().get();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition(),
		pObj->GetOrientation(),
		vector(1.0f, 1.0, 1.0f),
		0.1,
		AnimationCurveType::LINEAR,
		AnimationFlags()
	));

Error:
	return r;
}

RESULT UIViewTestSuite::AnimateMoveUpAndBack(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;

	DimObj *pObj = pButtonContext->GetSurface().get();

	point ptOrigin = pObj->GetPosition();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		point(ptOrigin.x(), 1.0f, 0.0f),
		pObj->GetOrientation(),
		pObj->GetScale(),
		0.1,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags()
	));
/*
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		point(ptOrigin.x(), 0.0f, 0.0f),
		pObj->GetOrientation(),
		pObj->GetScale(),
		0.1,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags()
	));
	//*/

Error:
	return r;
}

RESULT UIViewTestSuite::AddTestUIView() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		{
			auto pComposite = m_pDreamOS->AddComposite();
			pComposite->InitializeOBB();

			auto pView = pComposite->AddUIView(m_pDreamOS);
			pView->InitializeOBB();
			
			auto pQuad = pView->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector());

			for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
				CR(m_pDreamOS->RegisterEventSubscriber(nullptr, (InteractionEventType)(i), pView.get()));
			}
			for (int i = 0; i < (int)(UIEventType::UI_EVENT_INVALID); i++) {
				CR(pView->RegisterSubscriber((UIEventType)(i), this));
			}
			
			m_pDreamOS->AddInteractionObject(pComposite);
		}
		
		CR(Initialize());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize,
		std::bind(&UIViewTestSuite::UpdateHandRay, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::DefaultCallback, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::ResetTestCallback, this, std::placeholders::_1),
		nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Basic test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UIViewTestSuite::AddTestUIButton() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		{
			auto pComposite = m_pDreamOS->AddComposite();
			pComposite->InitializeOBB();

			auto pView = pComposite->AddUIView(m_pDreamOS);
			pView->InitializeOBB();

			auto pButton = pView->AddUIButton();
			pButton->RegisterToInteractionEngine(m_pDreamOS);

			pButton->RegisterEvent(UIEventType::UI_HOVER_BEGIN, 
				std::bind(&UIViewTestSuite::Rotate45, this, std::placeholders::_1, std::placeholders::_2));
			pButton->RegisterEvent(UIEventType::UI_HOVER_ENDED,
				std::bind(&UIViewTestSuite::ResetRotation, this, std::placeholders::_1, std::placeholders::_2));
		
			auto p2 = pView->AddUIButton();
			p2->SetPosition(point(2.0f, 0.0f, 0.0f));
			p2->RegisterToInteractionEngine(m_pDreamOS);

			p2->RegisterEvent(UIEventType::UI_HOVER_BEGIN,
				std::bind(&UIViewTestSuite::Rotate15, this, std::placeholders::_1, std::placeholders::_2));
			p2->RegisterEvent(UIEventType::UI_HOVER_ENDED,
				std::bind(&UIViewTestSuite::ResetRotation, this, std::placeholders::_1, std::placeholders::_2));

			m_pDreamOS->AddInteractionObject(pComposite);

			auto p3 = pView->AddUIButton();
			p3->SetPosition(point(-2.0f, 0.0f, 0.0f));
			p3->RegisterToInteractionEngine(m_pDreamOS);

			p3->RegisterEvent(UIEventType::UI_HOVER_BEGIN,
				std::bind(&UIViewTestSuite::AnimateScaleUp, this, std::placeholders::_1, std::placeholders::_2));
			p3->RegisterEvent(UIEventType::UI_HOVER_ENDED,
				std::bind(&UIViewTestSuite::AnimateScaleReset, this, std::placeholders::_1, std::placeholders::_2));
		}
	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize,
		std::bind(&UIViewTestSuite::UpdateHandRay, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::DefaultCallback, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::ResetTestCallback, this, std::placeholders::_1),
		nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Basic test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UIViewTestSuite::AddTestUIButtons() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		{
			auto pComposite = m_pDreamOS->AddComposite();
			pComposite->InitializeOBB();

			auto pView = pComposite->AddUIView(m_pDreamOS);
			pView->InitializeOBB();

			for (int i = 0; i < 4; i++) {
				auto pButton = pView->AddUIButton();
				pButton->RegisterToInteractionEngine(m_pDreamOS);
				pButton->SetPosition(point(i * 1.5f - 2.25f, 0.0f, 0.0f));

				pButton->RegisterEvent(UIEventType::UI_HOVER_BEGIN,
					std::bind(&UIViewTestSuite::AnimateScaleUp, this, std::placeholders::_1, std::placeholders::_2));
				pButton->RegisterEvent(UIEventType::UI_HOVER_ENDED,
					std::bind(&UIViewTestSuite::AnimateScaleReset, this, std::placeholders::_1, std::placeholders::_2));
				//pButton->RegisterEvent(UI_SELECT_ENDED,
				//	std::bind(&UIViewTestSuite::AnimateMoveUpAndBack, this, std::placeholders::_1));
			}
		}
	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize,
		std::bind(&UIViewTestSuite::UpdateHandRay, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::DefaultCallback, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::ResetTestCallback, this, std::placeholders::_1),
		nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Basic test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UIViewTestSuite::AddTestUIScrollView() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext {
		composite* pComposite = nullptr;
		std::shared_ptr<UIView> pView = nullptr;
		std::shared_ptr<UIScrollView> pScrollView = nullptr;
		std::vector<std::shared_ptr<UIButton>> pButtons = {};
		UIMallet* pLeftMallet = nullptr;
		UIMallet* pRightMallet = nullptr;
	};
	TestContext *pContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		auto& pComposite = pTestContext->pComposite;
		auto& pView = pTestContext->pView;
		auto& pScrollView = pTestContext->pScrollView;
		auto& pButtons = pTestContext->pButtons;
		auto& pLeftMallet = pTestContext->pLeftMallet;
		auto& pRightMallet = pTestContext->pRightMallet;

		int numButtons = 8;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		//CR(m_pDreamOS->AddInteractionObject(m_pDreamOS->GetHand(hand::HAND_RIGHT)));

		pComposite = m_pDreamOS->AddComposite();
		pComposite->InitializeOBB();

		pView = pComposite->AddUIView(m_pDreamOS);
		pView->InitializeOBB();

		pScrollView = pView->AddUIScrollView();

		for (int i = 0; i < numButtons; i++) {
			pButtons.emplace_back(pView->MakeUIButton()); // ScrollView adds them
		}

		pScrollView->UpdateMenuButtons(pButtons);
		//pScrollView->SetPosition(point(0.0f, 0.0f, 5.0f));
		pScrollView->SetPosition(m_pDreamOS->GetCamera()->GetPosition() + point(0.0f, 1.0f, 0.0f));
		pScrollView->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, -(float)(M_PI_2), 0.0f));

		pLeftMallet = new UIMallet(m_pDreamOS);
		pRightMallet = new UIMallet(m_pDreamOS);

		pLeftMallet->Show();
		pRightMallet->Show();

		m_pDreamOS->AddInteractionObject(pLeftMallet->GetMalletHead());
		m_pDreamOS->AddInteractionObject(pRightMallet->GetMalletHead());

	Error:
		return r;
	};
	 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		RotationMatrix qOffset = RotationMatrix();
		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			auto pScrollView = pTestContext->pScrollView;
			auto& pLeftMallet = pTestContext->pLeftMallet;
			auto& pRightMallet = pTestContext->pRightMallet;

			hand *pHand = m_pDreamOS->GetHand(HAND_TYPE::HAND_LEFT);
			CN(pHand);
			qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

			if (pLeftMallet)
				pLeftMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * pLeftMallet->GetHeadOffset()));

			pHand = m_pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT);
			CN(pHand);

			qOffset = RotationMatrix();
			qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

			if (pRightMallet)
				pRightMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * pRightMallet->GetHeadOffset()));

			CR(pScrollView->Update());
		}
		Error:
			return r;
		};

	auto pUITest = AddTest(fnInitialize,
		fnUpdate,
		std::bind(&UIViewTestSuite::DefaultCallback, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::ResetTestCallback, this, std::placeholders::_1),
		pContext);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Basic test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UIViewTestSuite::AddTestDreamUIBar() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		CN(m_pDreamOS);
		UIStageProgram *pUIStageProgram = nullptr;
		CR(SetupUIStagePipeline(pUIStageProgram));

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

				CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
			}
			//*
			for (int i = -4; i < 5; i++) {
				pQuad = m_pDreamOS->MakeQuad(1.0f, 1.0f);
				CN(pQuad);
				pQuad->RotateXByDeg(90.0f);
				pQuad->SetPosition(i * 1.05f, 0.0f, 3.5f);
				CR(m_pDreamOS->AddObjectToUIGraph(pQuad));
			}
			//*/

			auto pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this, false);
			CN(pDreamUIBar);
			pDreamUIBar->SetFont(L"Basis_Grotesque_Pro.fnt");
			pDreamUIBar->SetUIStageProgram(pUIStageProgram);

			CR(m_pDreamOS->InitializeKeyboard());
			
			float radius = 0.015f;
			point ptcam = m_pDreamOS->GetCamera()->GetPosition();
			auto pSphere = m_pDreamOS->AddSphere(radius, 10, 10);
			pSphere->SetPosition(ptcam + point(0.0f, 1.0f, -0.5f));
			pSphere = m_pDreamOS->AddSphere(radius, 10, 10);
			pSphere->SetPosition(ptcam + point(0.0f, 1.0f, 0.5f));
			pSphere = m_pDreamOS->AddSphere(radius, 10, 10);
			pSphere->SetPosition(ptcam + point(0.5f, 1.0f, 0.0f));
			pSphere = m_pDreamOS->AddSphere(radius, 10, 10);
			pSphere->SetPosition(ptcam + point(-0.5f, 1.0f, 0.0f));
		}

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize,
		std::bind(&UIViewTestSuite::UpdateHandRay, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::DefaultCallback, this, std::placeholders::_1),
		std::bind(&UIViewTestSuite::ResetTestCallback, this, std::placeholders::_1),
		nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Basic test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}


RESULT UIViewTestSuite::AddTestKeyboardAngle() {
	RESULT r = R_PASS;
	struct TestContext : public Subscriber<SenseControllerEvent> {
		std::shared_ptr<text> pTextBoxText;
		std::shared_ptr<composite> pChildComposite;
		std::shared_ptr<font> pFont;
		std::shared_ptr<UIKeyboard> pKeyboard = nullptr;
		bool fIncreaseMalletAngle = false;
		bool fDecreaseMalletAngle = false;
		DreamOS *pDreamOS;	
		float malletAngle = 180.0f;
		UIMallet *pKLeftMallet = nullptr;
		UIMallet *pKRightMallet = nullptr;
		virtual RESULT Notify(SenseControllerEvent *event) override {
			RESULT r = R_PASS;
			SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;

			if (event->state.type == CONTROLLER_RIGHT) {
				if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
					fIncreaseMalletAngle = true;
				}
			}
			else if (event->state.type == CONTROLLER_LEFT) {
				if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
					fDecreaseMalletAngle = true;
				}
			}
			if (eventType == SENSE_CONTROLLER_MENU_UP) {
				//hardcoded values taken from DreamUIBar
				CR(pKeyboard->UpdateComposite(/*-0.23f,*/ -0.30f));
			}
		Error:
			return r;
		}
	};
	TestContext *pTestContext = new TestContext();

	double sTestTime = 10000.0;

	auto fnAddKeyboardAngle = [=](UIButton *pButton, void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		auto& pKeyboard = pTestContext->pKeyboard;
		float current = pKeyboard->GetAngle();
		if (current > 70.0f) {
			current = 25.0f;
		}
		else {
			if (pButton->GetContactPoint().x() > 0)
				current += 1.0f;
			else if (pButton->GetContactPoint().x() < 0)
				current -= 1.0f;
		}
		pKeyboard->SetSurfaceAngle(current);
		std::string strCurrentAngle = std::to_string(current);
		pTestContext->pTextBoxText->SetText(strCurrentAngle);
		CR(pTestContext->pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		
	Error:
		return r;
	};

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		pTestContext->pDreamOS = m_pDreamOS;
		auto& pMalletAngle = pTestContext->malletAngle;
		
		CN(m_pDreamOS);
		UIStageProgram *pUIStageProgram = nullptr;
		CR(SetupUIStagePipeline(pUIStageProgram));

		{

			pTestContext->pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);
			pTestContext->pKeyboard->ShowKeyboard();
//			pTestContext->pKLeftMallet = pTestContext->pKeyboard->GetLeftMallet();
//			pTestContext->pKRightMallet = pTestContext->pKeyboard->GetRightMallet();
			//m_pDreamOS->AddInteractionObject(pTestContext->pKLeftMallet->GetMalletHead());
			//m_pDreamOS->AddInteractionObject(pTestContext->pKRightMallet->GetMalletHead());
			//*
			composite *pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);
			pComposite->SetPosition(m_pDreamOS->GetCameraPosition() - point(0.0f, -1.5f, 0.6f));	//with hmd
			
			auto pView = pComposite->AddUIView(m_pDreamOS);
			CN(pView);
			auto& pUIButtonAngleIncrease = pView->AddUIButton();
			auto& pUIButtonAngleDecrease = pView->AddUIButton();
			CN(pUIButtonAngleIncrease);
			CN(pUIButtonAngleDecrease);
			pUIButtonAngleIncrease->SetPosition(point(0.5f, 0.0f, -0.1f));
			pUIButtonAngleDecrease->SetPosition(point(-0.5f, 0.0f, -0.1f));

			//Setup textbox
			pTestContext->pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
			CN(pTestContext->pFont);
			pTestContext->pFont->SetLineHeight(0.050f);
			{
				pTestContext->pTextBoxText = std::shared_ptr<text>(m_pDreamOS->MakeText(
					pTestContext->pFont,
					"hi",
					0.5f - 0.02f,
					.050,
					text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD));
				CN(pTestContext->pTextBoxText);
				pView->AddObject(pTestContext->pTextBoxText);
				pTestContext->pTextBoxText->SetPosition(point(0.0f, 0.0f, -.1f));
				pTestContext->pTextBoxText->RotateXByDeg(90.0f);
			}

			//interaction
			CR(pUIButtonAngleIncrease->RegisterToInteractionEngine(m_pDreamOS));
			CR(pUIButtonAngleDecrease->RegisterToInteractionEngine(m_pDreamOS));
			pUIButtonAngleDecrease->RegisterEvent(UIEventType::UI_SELECT_BEGIN, fnAddKeyboardAngle, pTestContext);
			pUIButtonAngleIncrease->RegisterEvent(UIEventType::UI_SELECT_BEGIN, fnAddKeyboardAngle, pTestContext);

			CR(m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_TRIGGER_DOWN, pTestContext));
			CR(m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_MENU_UP, pTestContext));
			//*/

			float radiusOfReferenceSpheres = 0.015f;
			point ptCamera = m_pDreamOS->GetCamera()->GetPosition();
			auto pSphere = m_pDreamOS->AddSphere(radiusOfReferenceSpheres, 10, 10);
			pSphere->SetPosition(ptCamera + point(0.0f, 1.0f, -0.5f));
			pSphere = m_pDreamOS->AddSphere(radiusOfReferenceSpheres, 10, 10);
			pSphere->SetPosition(ptCamera + point(0.0f, 1.0f, 0.5f));
			pSphere = m_pDreamOS->AddSphere(radiusOfReferenceSpheres, 10, 10);
			pSphere->SetPosition(ptCamera + point(0.5f, 1.0f, 0.0f));
			pSphere = m_pDreamOS->AddSphere(radiusOfReferenceSpheres, 10, 10);
			pSphere->SetPosition(ptCamera + point(-0.5f, 1.0f, 0.0f));
		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(m_pDreamOS);
		if (pTestContext->fDecreaseMalletAngle) {
			pTestContext->malletAngle--;
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		else if (pTestContext->fIncreaseMalletAngle) {
			pTestContext->malletAngle++;
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		if(pTestContext->fDecreaseMalletAngle || pTestContext->fIncreaseMalletAngle) {
			pTestContext->fIncreaseMalletAngle = false;
			pTestContext->fDecreaseMalletAngle = false;
			std::string strCurrentAngle = std::to_string(pTestContext->malletAngle);
			pTestContext->pTextBoxText->SetText(strCurrentAngle);
			float rotationAngle = (pTestContext->malletAngle * (float)(M_PI) / 180.0f);

			pTestContext->pKLeftMallet->SetHeadOffset(point(0.0f, sin(rotationAngle) / 5, cos(rotationAngle) / 5));
			pTestContext->pKRightMallet->SetHeadOffset(point(0.0f, sin(rotationAngle) / 5, cos(rotationAngle) / 5));
		}

	Error:
		return R_PASS;
	};

	auto fnTest = [&](void *pContext) {return R_PASS; };
	auto pUIViewTest = AddTest(fnInitialize, fnUpdate, fnTest, pTestContext);
	CN(pUIViewTest);

	pUIViewTest->SetTestName("Local UIView Test");
	pUIViewTest->SetTestDescription("Test to adjust Keyboard and Mallet angles");
	pUIViewTest->SetTestDuration(sTestTime);
	pUIViewTest->SetTestRepeats(1);
Error:
	return r;
}

RESULT UIViewTestSuite::AddTestCurvedTitle() {	// can adjust scroll view depth with index triggers
	RESULT r = R_PASS;
	struct TestContext : public Subscriber<SenseControllerEvent> {
		bool fRight = false;
		bool fLeft = false;
		composite* pComposite = nullptr;
		virtual RESULT Notify(SenseControllerEvent *event) override {
			RESULT r = R_PASS;
			SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;

			if (event->state.type == CONTROLLER_RIGHT) {
				if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
					fRight = true;
				}
			}
			else if (event->state.type == CONTROLLER_LEFT) {
				if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
					fLeft = true;
				}
			}
			return r;
		}
	};
	TestContext *pTestContext = new TestContext();
	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		CN(m_pDreamOS);
		UIStageProgram *pUIStageProgram = nullptr;
		CR(SetupUIStagePipeline(pUIStageProgram));

		{
			composite *pComposite = m_pDreamOS->AddComposite();
			pTestContext->pComposite = pComposite;
			pComposite->InitializeOBB();
			std::shared_ptr<UIView> pView = pComposite->AddUIView(m_pDreamOS);
			pView->InitializeOBB();
			std::shared_ptr<UIScrollView> pScrollView = pView->AddUIScrollView();
			std::vector<std::shared_ptr<UIButton>> pButtons = {};
			for (int i = 0; i < 4; i++) {	
				pButtons.emplace_back(pView->MakeUIButton());
			}
			
			texture *pTexturePlaceholder = m_pDreamOS->MakeTexture(L"menu-item-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			texture *pTextureDropbox = m_pDreamOS->MakeTexture(L"dropbox.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			texture *pTextureDrive = m_pDreamOS->MakeTexture(L"google-drive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			texture *pTextureCloud = m_pDreamOS->MakeTexture(L"cloud-storage.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

			pScrollView->UpdateMenuButtons(pButtons);
			
			pButtons[0]->GetSurface()->SetDiffuseTexture(pTextureCloud);
			pButtons[1]->GetSurface()->SetDiffuseTexture(pTextureDrive);
			pButtons[2]->GetSurface()->SetDiffuseTexture(pTextureDropbox);
			pButtons[3]->GetSurface()->SetDiffuseTexture(pTexturePlaceholder);

			pScrollView->SetPosition(m_pDreamOS->GetCamera()->GetPosition() + point(0.0f, -0.15f, -5.27f));

			auto pFlatContext = m_pDreamOS->Add<FlatContext>(1024, 1024, 2);
			CN(pFlatContext);
			pFlatContext->SetScale(vector(1.05f, 0.35f, 3.5f));	// x, z, y

			auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
			pFont->SetLineHeight(0.055f);
			auto pTitleText = std::shared_ptr<text>(m_pDreamOS->MakeText(
				pFont,
				"Share",
				//"This one is even longer to check out them curves",
				1.25,
				0.055,
				text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

			pFlatContext->AddObject(pTitleText);
			pFlatContext->RotateXByDeg(90.0f);
			pFlatContext->RenderToQuad(quad::CurveType::CIRCLE);

			m_pDreamOS->GetCamera()->SetPosition(point(0.0f, -1.5f, 0.50f));
			pFlatContext->SetPosition(m_pDreamOS->GetCamera()->GetPosition() - point(0.0f, -1.46f, .80f)); //with HMD
			//pFlatContext->SetPosition(m_pDreamOS->GetCamera()->GetPosition() - point(0.0f, 0.0f, .80f));
			CR(m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_TRIGGER_DOWN, pTestContext));
		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		if (pTestContext->fLeft) {
			point ptCurrent = pTestContext->pComposite->GetPosition(false);
			pTestContext->pComposite->SetPosition(ptCurrent - point(0.0f, 0.0f, 0.01f));
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		else if (pTestContext->fRight) {
			point ptCurrent = pTestContext->pComposite->GetPosition(false);
			pTestContext->pComposite->SetPosition(ptCurrent + point(0.0f, 0.0f, 0.01f));
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		if (pTestContext->fLeft || pTestContext->fRight) {
			pTestContext->fLeft = false;
			pTestContext->fRight = false;
		}
	Error:
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, pTestContext);
	CN(pUITest);
	CN(pTestContext);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Test to show curved Title");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UIViewTestSuite::AddTestDreamControlView() {	
	RESULT r = R_PASS;
	
	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::string strURL = "http://www.youtube.com";

		std::shared_ptr<UIKeyboard> pUIKeyboard = nullptr;

		CN(m_pDreamOS);
		UIStageProgram *pUIStageProgram = nullptr;
		CR(SetupUIStagePipeline(pUIStageProgram));

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

				CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
			}

			auto pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this, false);
			CN(pDreamUIBar);
			pDreamUIBar->SetFont(L"Basis_Grotesque_Pro.fnt");
			pDreamUIBar->SetUIStageProgram(pUIStageProgram);

			//CR(m_pDreamOS->InitializeKeyboard());
	
			pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);	// setup browser
			pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
			pDreamBrowser->SetDiagonalSize(10.0f);
			pDreamBrowser->SetURI(strURL);

			pUIKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);
			pUIKeyboard->SetVisible(true);

			auto& pDreamControlView = m_pDreamOS->LaunchDreamApp<DreamControlView>(this, true);
			
		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, nullptr);
	CN(pUITest);
	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Full Test of DreamControlView");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UIViewTestSuite::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;
	switch (pEvent->m_eventType) {
	case (UIEventType::UI_SELECT_ENDED): {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pEvent->m_pObj);

		if (pDimObj != nullptr) {
			pDimObj->RotateYByDeg(45.0f);
		}
	}
	//*
	case (UIEventType::UI_HOVER_BEGIN): {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pEvent->m_pObj);

		if (pDimObj != nullptr) {
			pDimObj->RotateZByDeg(45.0f);
		}
	} break;
	case (UIEventType::UI_HOVER_ENDED): {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pEvent->m_pObj);

		if (pDimObj != nullptr) {
			pDimObj->ResetRotation();
		}
	} break;
	}

//Error:
	return r;
}

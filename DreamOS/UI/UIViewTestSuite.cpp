#include "UIViewTestSuite.h"

#include "DreamOS.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamBrowser.h"

#include "UIView.h"
#include "UIButton.h"
#include "UIScrollView.h"

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
	hand *pHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

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

			hand *pHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_LEFT);
			CN(pHand);
			qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

			if (pLeftMallet)
				pLeftMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * pLeftMallet->GetHeadOffset()));

			pHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);
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
	struct TestContext {
		std::shared_ptr<text> pTextBoxText;
		std::shared_ptr<composite> pChildComposite;
		std::shared_ptr<font> pFont;
		std::shared_ptr<UIKeyboard> pKeyboard = nullptr;
		DreamOS *pDreamOS;	//lost in button function otherwise
		
		float malletAngle = 180.0f;
		UIMallet *pBLeftMallet = nullptr;
		UIMallet *pBRightMallet = nullptr;
		UIMallet *pKLeftMallet = nullptr;
		UIMallet *pKRightMallet = nullptr;
		RESULT addKeyboardAngle(void *pContext) {
			RESULT r = R_PASS;
			float current = pDreamOS->GetKeyboard()->GetAngle();
			if (current > 90.0f) {
				current = 15.0f;
			}
			else
				current += 1.0f;
			pKeyboard->SetSurfaceAngle(current);
			CR(pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
			return r;
		Error:
			return r;
		}
	};
	//static TestContext *pTestContext = new TestContext();
	TestContext *pTestContext = new TestContext();

	double sTestTime = 10000.0;
	float lineHeight = .027f;
	float numLines = 1.0f;
	float lineWidth = 0.5f;
	float currentMalletAngle = 0.0f;
	
	auto fnGetTestContext = [&]() {
		return pTestContext;
	};

	auto fnAddKeyboardAngle = [=](void *pContext) {
		RESULT r = R_PASS;
		//auto pTContext = reinterpret_cast<TestContext*>(pTestContext);
		//auto& pKeyboard = m_pDreamOS->GetKeyboard();
		TestContext	*pTestContext = fnGetTestContext();
		auto& pKeyboard = pTestContext->pKeyboard;
		float current = pKeyboard->GetAngle();
		if (current > 90.0f) {
			current = 15.0f;
		}
		else
			current += 1.0f;
		pKeyboard->SetSurfaceAngle(current);
		std::string strCurrentAngle = std::to_string(current);
		CR(pTestContext->pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		return r;
	Error:
		return r;
	};
	
	auto fnDoNothing = [&](void *pContext) {
		
		return R_PASS;
	};

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		auto& pChildComposite = pTestContext->pChildComposite;
		auto& pFont = pTestContext->pFont;
		auto& pTextBoxText = pTestContext->pTextBoxText;
		pTestContext->pDreamOS = m_pDreamOS;
		auto& pMalletAngle = pTestContext->malletAngle;
		
		CN(m_pDreamOS);
		UIStageProgram *pUIStageProgram = nullptr;
		CR(SetupUIStagePipeline(pUIStageProgram));

		{
			auto pCloudController = m_pDreamOS->GetCloudController();
			auto pCommandLineManager = CommandLineManager::instance();
			DEBUG_LINEOUT("Initializing Cloud Controller");
			CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");
			{
				std::string strUsername = pCommandLineManager->GetParameterValue("username");
				std::string strPassword = pCommandLineManager->GetParameterValue("password");
				std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");

				CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
			}

			auto& pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this, false);
			CN(pDreamUIBar);
			pDreamUIBar->SetFont(L"Basis_Grotesque_Pro.fnt");
			pDreamUIBar->SetUIStageProgram(pUIStageProgram);

			CR(m_pDreamOS->InitializeKeyboard());
			pTestContext->pKeyboard = m_pDreamOS->GetKeyboard();
			pTestContext->pBLeftMallet = pDreamUIBar->GetLeftMallet();
			pTestContext->pBRightMallet = pDreamUIBar->GetRightMallet();
			pTestContext->pKLeftMallet = m_pDreamOS->GetKeyboard()->GetLeftMallet();
			pTestContext->pKRightMallet = m_pDreamOS->GetKeyboard()->GetRightMallet();
			//*
			float menuHeight = pDreamUIBar->GetMenuHeight();
			float menuDepth = pDreamUIBar->GetMenuDepth();
			composite *pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);
			pChildComposite = pComposite->AddComposite();
			CN(pChildComposite);
			//CR(pComposite->InitializeOBB());
			//CR(tComposite->InitializeOBB());
			pComposite->SetPosition(m_pDreamOS->GetCameraPosition() - point(0.0f, -1.5f, 0.6f));	//with hmd
			pChildComposite->SetPosition(m_pDreamOS->GetCameraPosition() - point(0.0f, 0.0f, 2.1f));	//with hmd
			//pComposite->SetPosition(m_pDreamOS->GetCameraPosition() - point(0.0f, 0.0f, 0.6f));
			//tComposite->SetPosition(m_pDreamOS->GetCameraPosition() - point(0.0f, 0.0f, 4.6f));	
			
			auto pView = pComposite->AddUIView(m_pDreamOS);
			CN(pView);
			auto& pUIButtonAngleIncrease = pView->AddUIButton();
			auto& pUIButtonAngleDecrease = pView->AddUIButton();
			CN(pUIButtonAngleIncrease);
			CN(pUIButtonAngleDecrease);
			pUIButtonAngleIncrease->SetPosition(point(0.5f, 0.0f, -0.1f));
			pUIButtonAngleDecrease->SetPosition(point(-0.5f, 0.0f, -0.1f));

			//Setup textbox
			pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
			CN(pFont);
			pFont->SetLineHeight(lineHeight);
			{
				pChildComposite->RotateXByDeg(90.0f);

				pTextBoxText = std::shared_ptr<text>(m_pDreamOS->MakeText(
					pFont,
					"hi",
					lineWidth - 0.02f,
					//lineHeight * numLines,
					10.0f,
					text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD));
				CN(pTextBoxText);
				pComposite->AddObject(pTextBoxText);
			}

			//interaction
			CR(pUIButtonAngleIncrease->RegisterToInteractionEngine(m_pDreamOS));
			CR(pUIButtonAngleDecrease->RegisterToInteractionEngine(m_pDreamOS));
			//pUIButtonAngleDecrease->RegisterEvent(UIEventType::UI_SELECT_BEGIN, [&](void *pContext) {return pTestContext->addKeyboardAngle(pContext); });
			//pUIButtonAngleIncrease->RegisterEvent(UIEventType::UI_SELECT_BEGIN, [&](void *pContext) { return fnAddKeyboardAngle(pTestContext); });
			pUIButtonAngleDecrease->RegisterEvent(UIEventType::UI_SELECT_BEGIN, fnDoNothing);
			pUIButtonAngleIncrease->RegisterEvent(UIEventType::UI_SELECT_BEGIN, fnDoNothing);

			CR(m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_TRIGGER_DOWN, this));
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
			/*m_pDreamOS->GetKeyboard()->ShowKeyboard();
			m_pDreamOS->GetKeyboard()->UpdateComposite(ptCamera.y()+ 2.0f, ptCamera.z()-0.4f);*/
		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(m_pDreamOS);
		if (m_pDreamOS->GetKeyboard()->GetLeftMallet()->CheckAndCleanDirty()) {
			pTestContext->malletAngle--;
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		else if (m_pDreamOS->GetKeyboard()->GetRightMallet()->CheckAndCleanDirty()) {
			pTestContext->malletAngle++;
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		//*/
		//*
		if(currentMalletAngle != pTestContext->malletAngle) {
			currentMalletAngle = pTestContext->malletAngle;
			std::string strCurrentAngle = std::to_string(currentMalletAngle);
			pTestContext->pTextBoxText->SetText(strCurrentAngle);

			float rotationAngle = (pTestContext->malletAngle * (float)(M_PI) / 180.0f);

			pTestContext->pKLeftMallet->SetHeadOffset(point(0.0f, sin(rotationAngle) / 5, cos(rotationAngle) / 5));
			pTestContext->pKRightMallet->SetHeadOffset(point(0.0f, sin(rotationAngle) / 5, cos(rotationAngle) / 5));
			pTestContext->pBLeftMallet->SetHeadOffset(point(0.0f, sin(rotationAngle) / 5, cos(rotationAngle) / 5));
			pTestContext->pBRightMallet->SetHeadOffset(point(0.0f, sin(rotationAngle) / 5, cos(rotationAngle) / 5));
		}//*/

	Error:
		return R_PASS;
	};

	auto fnTest = [&](void *pContext) {return R_PASS; };
	auto pUIViewTest = AddTest(fnInitialize, fnUpdate, fnTest, pTestContext);
	CN(pUIViewTest);

	pUIViewTest->SetTestName("Local UIView Test");
	pUIViewTest->SetTestDescription("Basic test of UIView working locally");
	pUIViewTest->SetTestDuration(sTestTime);
	pUIViewTest->SetTestRepeats(1);
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
	case (UIEventType::UI_SELECT_BEGIN): {
		float current = m_pDreamOS->GetKeyboard()->GetAngle();
		if (current > 90.0f) {
			current = 15.0f;
		}
		else
			current += 1.0f;
		m_pDreamOS->GetKeyboard()->SetSurfaceAngle(current);
		m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1);
	} break;
	}

//Error:
	return r;
}

RESULT UIViewTestSuite::Notify(SenseControllerEvent *event) {
	RESULT r = R_PASS;
	SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;
	
	if (event->state.type == CONTROLLER_RIGHT) {
		if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
			m_pDreamOS->GetKeyboard()->GetRightMallet()->SetDirty();
		}
	}
	else if (event->state.type == CONTROLLER_LEFT) {
		if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
			m_pDreamOS->GetKeyboard()->GetLeftMallet()->SetDirty();
		}
	}
//Error:
	return r;
}

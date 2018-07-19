#include "MultiContentTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/opengl/OGLProgram.h"
#include "HAL/UIStageProgram.h"

#include "DreamShareView/DreamShareView.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamGarage/Dream2DMouseApp.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamTabView.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamSettingsApp.h"
#include "DreamGarage/DreamLoginApp.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "WebBrowser/WebBrowserController.h"

#include "Cloud/CloudController.h"
#include "Cloud/CloudControllerFactory.h"
#include "Cloud/HTTP/HTTPController.h"
#include "Cloud/WebRequest.h"
#include "Core/Utilities.h" 

#include "UI/UIFlatScrollView.h"
#include "UI/UIButton.h"
#include "UI/UIMenuItem.h"
#include "UI/UISpatialScrollView.h"

#include "Sandbox/CommandLineManager.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/font.h"
#include "Primitives/text.h"

#include <memory>

MultiContentTestSuite::MultiContentTestSuite(DreamOS *pDreamOS) :
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

MultiContentTestSuite::~MultiContentTestSuite() {
	// empty
}

RESULT MultiContentTestSuite::Initialize() {
	RESULT r = R_PASS;

	CN(m_pDreamOS);

	m_pTestTextureUser1 = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	m_pTestTextureUser2 = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));

	CN(m_pTestTextureUser1);
	CN(m_pTestTextureUser2);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTests() {
	RESULT r = R_PASS;

	//CR(AddTestRemoveObjects2());
	//CR(AddTestRemoveObjects());

	CR(AddTestLoginForms());

	CR(AddTestDreamSettingsApp());

	CR(AddTestChangeUIWidth());

	CR(AddTestAllUIObjects());
	
	CR(AddTestManyBrowsers());

	CR(AddTestMenuShader());

	CR(AddTestMenuMemory());
	
	CR(AddTestRemoveText());

	CR(AddTestActiveSource());
		
	CR(AddTestDreamTabView());

	CR(AddTestUserControlAreaLayout());

	CR(AddTestUserControlArea());

	CR(AddTestMultiPeerBrowser());

	CR(AddTestMultiPeerBasic());

Error:
	return r;
}

RESULT MultiContentTestSuite::SetupPipeline() {
	RESULT r = R_PASS;

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

	ProgramNode* pUIProgramNode;
	pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

	// Screen Quad Shader (opt - we could replace this if we need to)
	ProgramNode *pRenderScreenQuad;
	pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	// Connect Program to Display

	// Connected in parallel (order matters)
	// NOTE: Right now this won't work with mixing for example
	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
	//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//CR(pDestSinkNode->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestDreamTabView() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::shared_ptr<UIView> pViewContext;
		std::shared_ptr<UIFlatScrollView> pFlatScrollView;
		std::shared_ptr<quad> pRenderQuad;
		std::shared_ptr<UIButton> pTestButton = nullptr;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();
		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		auto pComposite = m_pDreamOS->AddComposite();

		pTestContext->pRenderQuad = std::shared_ptr<quad>(m_pDreamOS->MakeQuad(5.0f, 5.0f));
		m_pDreamOS->AddObjectToUIGraph(pTestContext->pRenderQuad.get());
		pTestContext->pRenderQuad->RotateXByDeg(90.0f);

		pTestContext->pViewContext = pComposite->AddUIView(m_pDreamOS);
		pTestContext->pFlatScrollView = pTestContext->pViewContext->MakeUIFlatScrollView();
		//pTestContext->pFlatScrollView->RotateXByDeg(90.0f);
		pTestContext->pFlatScrollView->SetRenderQuad(pTestContext->pRenderQuad, nullptr); // deprec

		auto pTexture = m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

		auto pScrollContext = pTestContext->pFlatScrollView;// ->GetRenderContext();

		auto pTestButton = pTestContext->pFlatScrollView->AddUIButton(0.5f, 0.5f);
		//pTestContext->pTestButton = pTestContext->pFlatScrollView->AddUIButton(0.5f, 0.5f);
		//pTestContext->pTestButton = pTestContext->pFlatScrollView->MakeUIButton(0.5f, 0.5f);
		//m_pDreamOS->AddObjectToUIGraph(pTestContext->pTestButton.get());
		//pTestButton->SetVisible(true);

		auto pQuad = pScrollContext->AddQuad(0.5f, 0.5f, point(-1.0f, 0.0f, 0.0f));
		//		pTestContext->pFlatScrollView->AddObject(pQuad);
				//pQuad->SetVisible(true);
		CN(pQuad);
		//pQuad->SetVertexColor(COLOR_RED);
		pQuad->SetDiffuseTexture(pTexture);
		//*
		//pQuad = MakeQuad
		pQuad = pScrollContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, -1.0f));
		CN(pQuad);
		//		pQuad->SetVertexColor(COLOR_GREEN);

				/*
				pQuad = pScrollContext->AddQuad(0.5f, 0.5f);
				pQuad->SetPosition(-1.0f, 0.0f, 0.0f);
				CN(pQuad);
				pQuad->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-url.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
				//*/
				//*
		pTestButton->SetPosition(point(-0.5f, 0.0f, 0.0f));
		//pTestContext->pTestButton->RotateXByDeg(-90.0f);
		//pTestContext->pTestButton->GetSurface()->SetPosition(point(-0.5f, 0.0f, 0.0f));
		pTestButton->GetSurface()->RotateXByDeg(-90.0f);
		//CN(pButton);
		pTestButton->GetSurface()->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-close.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		//*/

		pQuad = pScrollContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, 1.0f));
		CN(pQuad);
		//		pQuad->SetVertexColor(COLOR_WHITE);

		m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			pQuad.get(),
			pQuad->GetPosition() + point(0.0f, 0.0f, 2.0f),
			pQuad->GetOrientation(),
			pQuad->GetScale(),
			1.5,
			AnimationCurveType::LINEAR,
			AnimationFlags::AnimationFlags()
		);

		pTestContext->pFlatScrollView->SetVisible(true);

		quad *pTestQuad;
		pTestQuad = m_pDreamOS->AddQuad(0.5f, 0.5f);
		CN(pTestQuad);

		pTestQuad->SetPosition(point(1.0f, 0.0f, 0.0f));
		pTestQuad->RotateXByDeg(90.0f);
		pTestQuad->SetDiffuseTexture(pTexture);
		pTestQuad->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-url.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));

		//pTestContext->pFlatScrollView->GetRenderContext()->AddObject(pQuad);


		pTestContext->pFlatScrollView->GetRenderContext()->AddObject(pTestContext->pFlatScrollView);

		/*
		{
			auto pFlat = pTestContext->pFlatScrollView->GetRenderContext();
			//auto pFlat = pTestContext->pFlatScrollView;
			m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
				pFlat.get(),
				pFlat->GetPosition() + point(0.0f, 0.0f, 2.0f),
				pFlat->GetOrientation(),
				pFlat->GetScale(),
				2.0,
				AnimationCurveType::LINEAR,
				AnimationFlags::AnimationFlags()
			);
		}
		//*/

		//*
		{
			auto pButton = pTestButton;

			m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
				pButton.get(),
				pButton->GetPosition() + point(-1.0f, 0.0f, 0.0f),
				pButton->GetOrientation(),
				pButton->GetScale(),
				2.0,
				AnimationCurveType::LINEAR,
				AnimationFlags::AnimationFlags()
			);
		}
		//*/
	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		pTestContext->pFlatScrollView->Update();

		return R_PASS;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi-browser");
	pNewTest->SetTestDescription("Multi browser, will allow a net of users to share a chrome browser");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestRemoveObjects() {
	RESULT r = R_PASS;
	
	double sTestTime = 10000.0f;
	int nRepeats = 1;

	struct TestContext {

		composite *pComposite = nullptr;
		std::shared_ptr<UIView> pView = nullptr;
		DreamOS *pDreamOS = nullptr; 

		double msLastSent = 0.0;
		double msTimeDelay = 500.0;
		bool fObject = true;

		RESULT TestAddObject() {
			if (pComposite != nullptr) {

				pView = pComposite->AddUIView(pDreamOS);
				auto pButton = pView->AddUIButton();
				//auto pText = pDreamOS->MakeText();

			}
			return R_PASS;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		pTestContext->pComposite = m_pDreamOS->AddComposite();
		pTestContext->pDreamOS = m_pDreamOS;
		pTestContext->TestAddObject();
		//CN(pTestContext);

	//Error:
		return r;
	};
	
	auto fnUpdate = [&](void *pContext) {
		
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
		if (msTimeNow - pTestContext->msLastSent > pTestContext->msTimeDelay) {
			if (pTestContext->fObject) {
				CR(m_pDreamOS->RemoveObject(pTestContext->pView.get()));
				pTestContext->pComposite->RemoveChild(pTestContext->pView);
				pTestContext->fObject = false;
			}
			else {
				pTestContext->TestAddObject();

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

RESULT MultiContentTestSuite::AddTestRemoveText() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0f;
	int nRepeats = 1;

	struct TestContext {

		composite *pComposite = nullptr;
		std::shared_ptr<text> pLabel = nullptr;
		DreamOS *pDreamOS = nullptr;
		std::shared_ptr<font> pFont;

		double msLastSent = 0.0;
		double msTimeDelay = 500.0;
		bool fObject = false;

		RESULT TestAddObject() {
			//if (pComposite != nullptr) {

				pLabel = std::shared_ptr<text>(pDreamOS->MakeText(
				pFont,
				"", 
				0.225,
				0.0703125, 
				text::flags::WRAP | text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

//				pComposite->AddObject(pLabel);
				pDreamOS->AddObject(pLabel.get());

				pLabel->RotateXByDeg(-90.0f);

			//}
			return R_PASS;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		pTestContext->pDreamOS = m_pDreamOS;

		pTestContext->pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);

	//Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {

		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
		if (msTimeNow - pTestContext->msLastSent > pTestContext->msTimeDelay) {
			if (pTestContext->fObject) {
			//	pTestContext->pComposite->RemoveChild(pTestContext->pLabel);
				CR(m_pDreamOS->RemoveAllObjects());
				//pTestContext->pLabel.reset();
				pTestContext->pLabel = nullptr;
				pTestContext->fObject = false;
			}
			else {
				pTestContext->TestAddObject();

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

RESULT MultiContentTestSuite::AddTestRemoveObjects2() {
	RESULT r = R_PASS;
	
	double sTestTime = 10000.0f;
	int nRepeats = 1;

	struct TestContext {

		composite *pComposite = nullptr;
		std::shared_ptr<UIView> pView = nullptr;
		std::shared_ptr<UIMenuItem> pButton = nullptr;
		std::shared_ptr<text> pLabel = nullptr;
		DreamOS *pDreamOS = nullptr; 
		std::shared_ptr<DreamUserControlArea> pUserControlArea;

		double msLastSent = 0.0;
		double msTimeDelay = 500.0;
		bool fObject = false;
		bool fFirst = true;

		RESULT TestAddObject() {
			if (pComposite != nullptr) {

				pView = pComposite->AddUIView(pDreamOS);
//				auto pQuad = pView->AddQuad(1.0f, 1.0f);
//				pQuad->RotateXByDeg(90.0f);
				//*
				pButton = pView->AddUIMenuItem();
				//CN(pButton);

				auto pDreamUIBar = pUserControlArea->m_pDreamUIBar;

				pLabel = std::shared_ptr<text>(pDreamOS->MakeText(
				pDreamUIBar->m_pFont,
				"", 
				0.225,
				0.0703125, 
				text::flags::WRAP | text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

				pButton->AddObject(pLabel);

				/*
				auto iconFormat = IconFormat();
				iconFormat.pTexture = pDreamUIBar->m_pDefaultThumbnail.get();

				auto labelFormat = LabelFormat();
				labelFormat.strLabel = "Label ";
				labelFormat.pFont = pDreamUIBar->m_pFont;
				labelFormat.pBgTexture = pDreamUIBar->m_pMenuItemBg.get();

				pButton->Update(iconFormat, labelFormat);
				//*/

			}
			return R_PASS;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		pTestContext->pDreamOS = m_pDreamOS;
		//pTestContext->TestAddObject();
		//CN(pTestContext);

	//Error:
		return r;
	};
	
	auto fnUpdate = [&](void *pContext) {
		
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		if (pTestContext->fFirst) {
			auto pDreamUIBar = pTestContext->pUserControlArea->m_pDreamUIBar;
			pTestContext->fFirst = false;
		}

		std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
		if (msTimeNow - pTestContext->msLastSent > pTestContext->msTimeDelay) {
			if (pTestContext->fObject) {
				CR(m_pDreamOS->RemoveObject(pTestContext->pView.get()));
				pTestContext->pLabel->ClearChildren();
				pTestContext->pButton->RemoveChild(pTestContext->pLabel);
				CR(m_pDreamOS->RemoveObject(pTestContext->pButton.get()));
				CR(m_pDreamOS->RemoveObject(pTestContext->pLabel.get()));
				pTestContext->pComposite->RemoveChild(pTestContext->pView);

				pTestContext->pLabel = nullptr;
				pTestContext->pButton = nullptr;
				pTestContext->pView = nullptr;
				pTestContext->fObject = false;
			}
			else {
				pTestContext->TestAddObject();

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

RESULT MultiContentTestSuite::AddTestLoginForms() {
	RESULT r = R_PASS;
	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public DOSObserver {
		std::shared_ptr<DreamLoginApp> pFormApp = nullptr;
		std::shared_ptr<DreamSettingsApp> pSettingsApp = nullptr;
		std::shared_ptr<DreamUserApp> pUserApp = nullptr;
		std::shared_ptr<DreamUserControlArea> pUserControlArea = nullptr;
		bool fFirst = true;

		// login logic information
		bool fFirstLogin = true;
		bool fHasCreds = false;
		std::string strRefreshToken;
		std::string	strAccessToken;

		virtual RESULT HandleDOSMessage(std::string& strMessage) override {
			if (strMessage == "DreamSettingsApp.OnSuccess") {
				if (fFirstLogin) {
					// TODO: Show sign up form
				//	pFormApp->UpdateWithNewForm();
					pFormApp->Show();
				}
				else {
					// TOSO: Show sign in form
					pFormApp->Show();
				}
			}

			return R_PASS;
		}

	} *pTestContext = new TestContext();


	auto fnInitialize = [&](void *pContext) {

		RESULT r = R_PASS;

		CR(SetupPipeline());

		light *pLight;
		//pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(1.0f, -1.0f, -1.0f));
		m_pDreamOS->AddQuad(1.0f, 1.0f);

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		//*
		if (pTestContext->pUserControlArea == nullptr) {
			pTestContext->pUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
			pTestContext->pUserControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this);
			pTestContext->pUserControlArea->SetDreamUserApp(pTestContext->pUserApp);

			pTestContext->pFormApp = m_pDreamOS->LaunchDreamApp<DreamLoginApp>(this, false);
			pTestContext->pSettingsApp = m_pDreamOS->LaunchDreamApp<DreamSettingsApp>(this, false);

			pTestContext->pUserApp->GetComposite()->SetPosition(m_pDreamOS->GetCamera()->GetPosition() + point(0.0f, -0.2f, -0.5f));


			//pTestContext->pFormApp->GetComposite()->SetVisible(true, false);
			pTestContext->pFormApp->UpdateWithNewForm("https://www.develop.dreamos.com/forms/account/signup");
			//pTestContext->pFormApp->Show();
			//pTestContext->pSettingsApp->GetComposite()->SetVisible(true, false);
			pTestContext->pSettingsApp->UpdateWithNewForm("https://www.develop.dreamos.com/forms/settings");
			pTestContext->pSettingsApp->GetComposite()->SetVisible(false, false);
			//pTestContext->pSettingsApp->Show();

			//pTestContext->pFormApp->SetLaunchDate();
		}
		//*/

		if (pTestContext->pFormApp != nullptr) {
			auto pForm = pTestContext->pSettingsApp;
			if (pForm->m_pFormView != nullptr) {
				pForm->GetComposite()->SetVisible(true, false);
				/*
				pForm->GetComposite()->SetVisible(true, false);
				if (!pForm->m_pFormView->GetViewQuad()->IsVisible()) {
					pTestContext->pFormApp->Show();
				}

				pTestContext->fFirstLogin = pForm->IsFirstLaunch();
				if (!pTestContext->fFirstLogin) {
					pTestContext->fHasCreds = pForm->HasStoredCredentials(pTestContext->strRefreshToken, pTestContext->strAccessToken);
				}
				//*/

				if (!pForm->m_pFormView->GetViewQuad()->IsVisible()) {
					pForm->Show();
				}
			}
		}

		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestMenuShader() {
	RESULT r = R_PASS;
	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::shared_ptr<DreamUserControlArea> pUserControlArea;
		bool fFirst = true;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {

		RESULT r = R_PASS;

		SetupPipeline();

		std::shared_ptr<EnvironmentAsset> pEnvAsset = nullptr;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;
		CN(pControlArea);

		m_pDreamOS->AddObjectToInteractionGraph(pControlArea->GetComposite());

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = pTestContext->pUserControlArea;

		if (pTestContext->fFirst) {
			pTestContext->fFirst = false;
			auto pDreamUIBar = pTestContext->pUserControlArea->m_pDreamUIBar;
			std::vector<std::shared_ptr<UIButton>> pButtons;

			//pDreamUIBar->m_pScrollView
			// setup fake menu
			for (int i = 0; i < 8; i++) {

				auto pButton = pDreamUIBar->m_pView->MakeUIMenuItem(pDreamUIBar->m_pScrollView->GetWidth(), pDreamUIBar->m_pScrollView->GetWidth() * 9.0f / 16.0f);
				CN(pButton);

				auto iconFormat = IconFormat();
				iconFormat.pTexture = pDreamUIBar->m_pDefaultThumbnail.get();

				auto labelFormat = LabelFormat();
				labelFormat.strLabel = "Label " + std::to_string(i);
				labelFormat.pFont = pDreamUIBar->m_pFont;
				labelFormat.pBgTexture = pDreamUIBar->m_pMenuItemBg.get();

				pButton->Update(iconFormat, labelFormat);

				pButtons.emplace_back(pButton);
			}

			pDreamUIBar->m_pScrollView->GetTitleText()->SetText("Testing");
			pDreamUIBar->SetUIStageProgram(m_pUIProgramNode);

			CR(pDreamUIBar->m_pScrollView->UpdateMenuButtons(pButtons));

			//pDreamUIBar->ResetAppComposite();
			pTestContext->pUserControlArea->ResetAppComposite();
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
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;

}

RESULT MultiContentTestSuite::AddTestDreamSettingsApp() {
	RESULT r = R_PASS;
	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext 
	{
		std::shared_ptr<DreamUserApp> pUserApp = nullptr;
		std::shared_ptr<DreamSettingsApp> pSettingsApp = nullptr;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;

		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager = nullptr;
		std::shared_ptr<texture> pTestTexture = nullptr;

		quad *pBrowserQuad = nullptr;

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		std::string strURL = "https://twitch.tv";

		CR(SetupPipeline());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		pTestContext->pUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		//pTestContext->pUserApp->ResetAppComposite();
		//*
		pTestContext->pSettingsApp = m_pDreamOS->LaunchDreamApp<DreamSettingsApp>(this, false);
		CR(pTestContext->pSettingsApp->Show());
		//*/


		pTestContext->pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		pTestContext->pDreamBrowser->InitializeWithBrowserManager(pTestContext->pUserApp->GetBrowserManager(), strURL);
		//pTestContext->pDreamBrowser->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
		CNM(pTestContext->pDreamBrowser, "Failed to create dream browser");

		pTestContext->pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pTestContext->pDreamBrowser->SetDiagonalSize(10.0f);

		pTestContext->pDreamBrowser->SetURI(strURL);


		pTestContext->pBrowserQuad = m_pDreamOS->AddQuad(3.0f, 3.0f * 9.0f / 16.0f);
		CN(pTestContext->pBrowserQuad);
		pTestContext->pBrowserQuad->RotateXByDeg(90.0f);
		pTestContext->pBrowserQuad->RotateZByDeg(180.0f);

		pTestContext->pUserApp->GetComposite()->SetPosition(m_pDreamOS->GetCamera()->GetPosition() + point(0.0f, 0.5f, -0.5f));

		pTestContext->pTestTexture = std::shared_ptr<texture>(m_pDreamOS->MakeTexture((wchar_t*)(L"client-loading-1366-768.png"), texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		auto pTexture = pTestContext->pDreamBrowser->GetSourceTexture();
		auto pFormView = pTestContext->pSettingsApp->m_pFormView;
		//pFormView->SetViewQuadTexture(pTexture);
		//pFormView->SetViewQuadTexture(pTestContext->pTestTexture);
		//pTestContext->pSettingsApp->InitializeSettingsForm("https://twitch.tv");

		pTestContext->pBrowserQuad->SetDiffuseTexture(pTexture.get());

	//Error:
		return r;
	};
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestChangeUIWidth() {
	RESULT r = R_PASS;
	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public Subscriber<SenseControllerEvent>,
		public CloudController::UserObserver
	{
		std::shared_ptr<DreamUserControlArea> pUserControlArea = nullptr;
		UserController* pUserControllerProxy = nullptr;
		CloudController *pCloudController = nullptr;

		stereocamera *pCamera = nullptr;;
		bool fFirst = true;

		bool fDirty = false;
		bool fLeft = false;
		bool fRight = false;

		float m_height = 0.0f;
		float m_depth = 0.0f;
		float m_scale = 1.0f;

		std::wstring wstrHardwareID;

		virtual RESULT Notify(SenseControllerEvent *pEvent) override {
			RESULT r = R_PASS;

			if (pEvent->type == SENSE_CONTROLLER_MENU_UP && pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
				pUserControllerProxy->RequestSetSettings(wstrHardwareID,"HMDType.OculusRift", m_height, m_depth, m_scale);
			}
			else if (pEvent->type == SENSE_CONTROLLER_PAD_MOVE) {
				float diff = pEvent->state.ptTouchpad.y() * 0.015f;
				if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
					if (pUserControlArea != nullptr && !fFirst) {
						float currentHeight = pUserControlArea->m_pDreamUserApp->m_pAppBasis->GetPosition().y();
						pUserControlArea->SetViewHeight(currentHeight + diff);
						m_height += diff;
					}
				}
				else {
					//TODO
					point ptCamera = pCamera->GetEyePosition(EYE_MONO);
				//	pCamera->SetPosition(point(ptCamera.x(), ptCamera.y() + height, ptCamera.z()));
					pCamera->SetHMDAdjustedPosition(point(ptCamera.x(), ptCamera.y(), diff + ptCamera.z()));
					m_depth += diff;
				}
			}
			else if (pEvent->type == SENSE_CONTROLLER_TRIGGER_DOWN) {// && pEvent->state.triggerRange < 0.5f) {
				if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
					m_scale = pUserControlArea->GetViewScale() + 0.003f;
					fLeft = true;
				}
				else {
					m_scale = pUserControlArea->GetViewScale() - 0.003f;
					fRight = true;
				}
				pUserControlArea->ScaleViewWidth(m_scale);
			}

			else if (pEvent->type == SENSE_CONTROLLER_TRIGGER_UP) {
				if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
					fLeft = false;
				}
				else {
					fRight = false;
				}
			}
			//else if (pEvent->type == SENSE_CONTROLLER_)
		//Error:
			return r;
		};

		virtual RESULT OnGetSettings(float height, float depth, float scale) override {
			RESULT r = R_PASS;

			pUserControlArea->m_pDreamUserApp->GetKeyboard()->Show();
			m_height = height;
			m_depth = depth;
			m_scale = scale;

			float currentHeight = pUserControlArea->m_pDreamUserApp->m_pAppBasis->GetPosition().y();
			pUserControlArea->SetViewHeight(currentHeight + m_height);
			
			point ptCamera = pCamera->GetEyePosition(EYE_MONO);
			pCamera->SetHMDAdjustedPosition(point(ptCamera.x(), ptCamera.y(), m_depth + ptCamera.z()));

			pUserControlArea->ScaleViewWidth(m_scale);

		//Error:
			return r;
		}
		virtual RESULT OnSetSettings() override {
			return R_PASS;
		}

		//TODO: to update this, extend environment controller observer
		/*
		virtual RESULT OnSettings(std::string strURL) override {
			RESULT r = R_PASS;

			pUserControlArea->m_pDreamUserApp->GetKeyboard()->Show();

			//TODO: make browser in testing release

//		Error:
			return r;
		}
		//*/
		virtual RESULT OnLogin() override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnLogout() override {
			return R_NOT_IMPLEMENTED;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pCamera = m_pDreamOS->GetCamera();
		pControlArea->SetUIProgramNode(m_pUIProgramNode);

		CN(pTestContext);
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_PAD_MOVE, pTestContext);
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_MOVE, pTestContext);
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_DOWN, pTestContext);
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_UP, pTestContext);
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_MENU_UP, pTestContext);

		pTestContext->pUserControlArea = pControlArea;
		CN(pControlArea);
		
		m_pDreamOS->InitializeCloudController();
		pTestContext->pCloudController = m_pDreamOS->GetCloudController();
		pTestContext->pCloudController->Start("jason_test1@dreamos.com", "nightmare", 168);
		pTestContext->pCloudController->RegisterUserObserver(pTestContext);
		pTestContext->pUserControllerProxy = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

		m_pDreamOS->AddObjectToInteractionGraph(pControlArea->GetComposite());	

		pTestContext->wstrHardwareID = m_pDreamOS->GetHardwareID();
	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = pTestContext->pUserControlArea;

		if (pTestContext->fFirst) {

			CR(pTestContext->pUserControllerProxy->RequestGetSettings(m_pDreamOS->GetHardwareID(),"HMDType.OculusRift"));
//			CR(pTestContext->pUserControlArea->m_pDreamUserApp->GetKeyboard()->Show());		

			pTestContext->fFirst = false;

		}

//*
		if (pTestContext->fLeft) {
			pTestContext->pUserControlArea->ScaleViewWidth(pTestContext->pUserControlArea->GetViewScale() + 0.003f);
		}
		if (pTestContext->fRight) {
			pTestContext->pUserControlArea->ScaleViewWidth(pTestContext->pUserControlArea->GetViewScale() - 0.003f);
		}
		//*/

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
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestAllUIObjects() {
	RESULT r = R_PASS;
	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::vector<std::string> strURIs;
		std::shared_ptr<DreamUserControlArea> pUserControlArea;
		std::shared_ptr<DreamBrowser> pBrowser1;
		std::shared_ptr <DreamBrowser> pBrowser2;
		std::vector<std::shared_ptr<DreamBrowser>> pDreamBrowsers;
		std::shared_ptr<CEFBrowserManager> pWebBrowserManager;
		bool fFirst = true;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		std::shared_ptr<EnvironmentAsset> pEnvAsset = nullptr;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;
		CN(pControlArea);
		
		m_pDreamOS->AddObjectToInteractionGraph(pControlArea->GetComposite());	

		///*
		pTestContext->pBrowser1 = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		//pTestContext->pBrowser1->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, "https://www.develop.dreamos.com/forms/settings");
		//pTestContext->pBrowser1->SetURI("https://www.develop.dreamos.com/forms/settings");
//		pTestContext->pBrowser1->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, "twitch.tv");
		pTestContext->pBrowser1->SetURI("twitch.tv");
		//pTestContext->pBrowser1->InitializeWithParent(pControlArea.get());

		pTestContext->pBrowser2 = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
//		pTestContext->pBrowser2->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, "www.nyt.com");
		pTestContext->pBrowser2->SetURI("www.nyt.com");
		//pTestContext->pBrowser2->InitializeWithParent(pControlArea.get());

		//pControlArea->GetComposite()->SetPosition(0.0f, -0.125f, 4.6f);
		//pControlArea->GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * (float)M_PI / 180.0f, 0.0f, 0.0f)));
		pControlArea->m_fFromMenu = true;
		//*/

		/*
		pTestContext->strURIs = {
			"www.nyt.com",
			"www.dreamos.com",
			"en.wikipedia.org/wiki/Tropical_house",
			"www.nyt.com",
			"www.dreamos.com",
			"en.wikipedia.org/wiki/Tropical_house",
			"www.livelovely.com",
			"www.twitch.tv"
		} ;

		for (int i = 0; i < pTestContext->strURIs.size(); i++) {
			pTestContext->pDreamBrowsers.emplace_back(m_pDreamOS->LaunchDreamApp<DreamBrowser>(this));
			pTestContext->pDreamBrowsers[i]->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, pTestContext->strURIs[i]);
			pTestContext->pDreamBrowsers[i]->SetURI(pTestContext->strURIs[i]);
		}
		//*/

//		auto pDreamUIBar = 
//		m_Start();
//		m_pDreamOS->GetCloudController()->Login();

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = pTestContext->pUserControlArea;

		if (pTestContext->fFirst) {
			pTestContext->pUserControlArea->SetActiveSource(pTestContext->pBrowser1);
			pTestContext->pUserControlArea->m_pDreamTabView->AddContent(pTestContext->pBrowser2);
			
			//for (auto pBrowser : pTestContext->pDreamBrowsers) {
			//	pTestContext->pUserControlArea->m_pDreamTabView->AddContent(pBrowser);
			//}
			
			pTestContext->fFirst = false;

//			pTestContext->pUserControlArea->m_pDreamUIBar->ShowRootMenu();
//			pTestContext->pUserControlArea->m_pDreamUIBar->ShowApp();
			//*
			auto pDreamUIBar = pTestContext->pUserControlArea->m_pDreamUIBar;
			std::vector<std::shared_ptr<UIButton>> pButtons;

			//pDreamUIBar->m_pScrollView
			// setup fake menu
			for (int i = 0; i < 4; i++) {

				auto pButton = pDreamUIBar->m_pView->MakeUIMenuItem(pDreamUIBar->m_pScrollView->GetWidth(), pDreamUIBar->m_pScrollView->GetWidth() * 9.0f / 16.0f);
				CN(pButton);

				auto iconFormat = IconFormat();
				iconFormat.pTexture = pDreamUIBar->m_pDefaultThumbnail.get();

				auto labelFormat = LabelFormat();
				labelFormat.strLabel = "Label " + i;
				labelFormat.pFont = pDreamUIBar->m_pFont;
				labelFormat.pBgTexture = pDreamUIBar->m_pMenuItemBg.get();

				pButton->Update(iconFormat, labelFormat);

				pButtons.emplace_back(pButton);
			}

			pDreamUIBar->m_pScrollView->GetTitleText()->SetText("Testing");

			CR(pDreamUIBar->m_pScrollView->UpdateMenuButtons(pButtons));

			pDreamUIBar->ResetAppComposite();
			pTestContext->pUserControlArea->ResetAppComposite();

			pTestContext->pUserControlArea->m_pDreamUserApp->GetKeyboard()->Show();		
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
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestMenuMemory() {
	RESULT r = R_PASS;
	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::vector<std::string> strURIs;
		std::shared_ptr<DreamUserControlArea> pUserControlArea;
		std::shared_ptr<CEFBrowserManager> pWebBrowserManager;

		bool fFirst = true;

		double msLastSent = 0.0;
		double msTimeDelay = 1000.0;

		RESULT CreateFakeMenu() {
			RESULT r = R_PASS;
			auto pDreamUIBar = pUserControlArea->m_pDreamUIBar;
			std::vector<std::shared_ptr<UIButton>> pButtons;

			// setup fake menu
			for (int i = 0; i < 4; i++) {

				auto pButton = pDreamUIBar->m_pView->MakeUIMenuItem();
				CN(pButton);

				auto iconFormat = IconFormat();
				iconFormat.pTexture = pDreamUIBar->m_pDefaultThumbnail.get();

				auto labelFormat = LabelFormat();
				labelFormat.strLabel = "Label " + i;
				labelFormat.pFont = pDreamUIBar->m_pFont;
				labelFormat.pBgTexture = pDreamUIBar->m_pMenuItemBg.get();

				pButton->Update(iconFormat, labelFormat);

				pButtons.emplace_back(pButton);
			}

			pDreamUIBar->m_pScrollView->GetTitleText()->SetText("Testing");

			CR(pDreamUIBar->m_pScrollView->UpdateMenuButtons(pButtons));
		Error:
			return r;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		std::shared_ptr<EnvironmentAsset> pEnvAsset = nullptr;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;
		CN(pControlArea);
		
		m_pDreamOS->AddObjectToInteractionGraph(pControlArea->GetComposite());	
		//pControlArea->GetComposite()->SetPosition(0.0f, -0.125f, 4.6f);
		//pControlArea->GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * (float)M_PI / 180.0f, 0.0f, 0.0f)));
		pControlArea->m_fFromMenu = true;

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = pTestContext->pUserControlArea;

		if (pTestContext->fFirst) {
			pTestContext->fFirst = false;

			CR(pTestContext->CreateFakeMenu());
			pTestContext->pUserControlArea->ResetAppComposite();
		}
		else {
			std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
			float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
			if (msTimeNow - pTestContext->msLastSent > pTestContext->msTimeDelay) {
				CR(pTestContext->CreateFakeMenu());
				pTestContext->msLastSent = msTimeNow;
			}
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
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestActiveSource() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::vector<std::string> strURIs;
		std::shared_ptr<DreamUserControlArea> pUserControlArea;
		std::shared_ptr<DreamBrowser> pBrowser1;
		std::shared_ptr <DreamBrowser> pBrowser2;
		std::vector<std::shared_ptr<DreamBrowser>> pDreamBrowsers;
		std::shared_ptr<CEFBrowserManager> pWebBrowserManager;
		double msLastSent = 0.0;
		double msTimeDelay = 5000.0;
		bool fSwitch = false;
		bool fFirst = true;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		std::shared_ptr<EnvironmentAsset> pEnvAsset = nullptr;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;
		CN(pControlArea);
		
		m_pDreamOS->AddObjectToInteractionGraph(pControlArea->GetComposite());	

		pTestContext->pBrowser1 = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
//		pTestContext->pBrowser1->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, "https://www.youtube.com/watch?v=OPV3D7f3bHY&t=340s");
		pTestContext->pBrowser1->SetURI("https://www.youtube.com/watch?v=OPV3D7f3bHY&t=340s");
		//pTestContext->pBrowser2->InitializeWithParent(pControlArea.get());

		pTestContext->pBrowser2 = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		
		//pTestContext->pBrowser2->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, "www.twitch.tv");
		//pTestContext->pBrowser2->SetURI("www.twitch.tv");

//		pTestContext->pBrowser2->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, "https://www.youtube.com/watch?v=IP-iKQn8hWw");
		pTestContext->pBrowser2->SetURI("https://www.youtube.com/watch?v=IP-iKQn8hWw");

		////pTestContext->pBrowser1->InitializeWithParent(pControlArea.get());

		pControlArea->GetComposite()->SetPosition(0.0f, -0.125f, 4.6f);
		pControlArea->GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * (float)M_PI / 180.0f, 0.0f, 0.0f)));
		pControlArea->m_fFromMenu = true;

		pTestContext->strURIs = {
			"www.nyt.com",
			"www.dreamos.com",
			"en.wikipedia.org/wiki/Tropical_house",
			"www.nyt.com",
			"www.dreamos.com",
			"en.wikipedia.org/wiki/Tropical_house",
			"www.livelovely.com",
			"www.twitch.tv"
		} ;

		std::chrono::steady_clock::duration tNow;
		tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		
		float msTimeNow;
		msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

		pTestContext->msLastSent = msTimeNow;

		/*
		for (int i = 0; i < pTestContext->strURIs.size(); i++) {

			pTestContext->pDreamBrowsers.emplace_back(m_pDreamOS->LaunchDreamApp<DreamBrowser>(this));
			pTestContext->pDreamBrowsers[i]->InitializeWithBrowserManager(pControlArea->m_pWebBrowserManager, pTestContext->strURIs[i]);
			pTestContext->pDreamBrowsers[i]->SetURI(pTestContext->strURIs[i]);
			//pTestContext->pDreamBrowsers[i]->InitializeWithParent(pControlArea.get());
		}
		//*/

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = pTestContext->pUserControlArea;


		if (pTestContext->fFirst) {
			pTestContext->pUserControlArea->SetActiveSource(pTestContext->pBrowser1);
			pTestContext->pUserControlArea->m_pDreamTabView->AddContent(pTestContext->pBrowser2);
			
			/*
			for (auto pBrowser : pTestContext->pDreamBrowsers) {
				pTestContext->pUserControlArea->m_pDreamTabView->AddContent(pBrowser);
//				pBrowser->InitializeWithParent(pControlArea.get());
			}
			//*/
			
			pTestContext->fFirst = false;
		}
		else {
			
			return R_PASS;

			std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
			float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
			if (msTimeNow - pTestContext->msLastSent > pTestContext->msTimeDelay) {
				pTestContext->msTimeDelay = 500.0f;
				pTestContext->msLastSent = msTimeNow;
				int randAction = std::rand() % 10;

				if (randAction <= 7) {
					// select random tab
					DEBUG_LINEOUT("SELECT");
					auto tabButtons = pTestContext->pUserControlArea->m_pDreamTabView->m_tabButtons;
					if (tabButtons.size() > 0) {
						int randIndex = std::rand() % tabButtons.size();
						auto pButton = tabButtons[randIndex];

						pTestContext->pUserControlArea->m_pDreamTabView->m_fForceContentFocus = true;
						pTestContext->pUserControlArea->m_pDreamTabView->SelectTab(pButton.get(), nullptr);
					}

				}
				/*
				else if (randAction == 9) {
					DEBUG_LINEOUT("OPEN");
					int randIndex = std::rand() % pTestContext->strURIs.size();

					auto pNewBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
					pNewBrowser->InitializeWithBrowserManager(pTestContext->pUserControlArea->m_pWebBrowserManager, pTestContext->strURIs[randIndex]);
					pNewBrowser->SetURI(pTestContext->strURIs[randIndex]);
					pNewBrowser->InitializeWithParent(pTestContext->pUserControlArea.get());
					pTestContext->pDreamBrowsers.emplace_back(pNewBrowser);
					pTestContext->pUserControlArea->m_pDreamTabView->AddContent(pNewBrowser);
						
				}
				//*/

				/*
				else if (randAction == 8) {
					DEBUG_LINEOUT("CLOSE");
					auto tabButtons = pTestContext->pUserControlArea->m_pDreamTabView->m_tabButtons;
					if (tabButtons.size() > 0) {
						pTestContext->pUserControlArea->CloseActiveAsset();
					}
				}
				//*/

			}
		}
		return R_PASS;
	};
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi Content Active Source");
	pNewTest->SetTestDescription("Multi Content, swapping active source");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestUserControlArea() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::vector<std::string> strURIs;
	//	std::vector<std::shared_ptr<DreamBrowser>> pDreamBrowsers;
	//	std::vector<std::shared_ptr<quad>> pBrowserQuads;
		std::shared_ptr<DreamUserControlArea> pUserControlArea;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		SetupPipeline();

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;
		CN(pControlArea);

//		pControlArea->SendURL("")
//		pUserControlArea->m_pA

		//pControlArea->GetComposite()->SetPosition(0.0f, -0.125f, 4.6f);
		//pControlArea->GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * -(float)M_PI / 180.0f, 0.0f, 0.0f)));


	Error:
		return r;
	};
	auto fnUpdate = [&](void *pContext) {

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		//pTestContext->pUserControlArea->Update();

		//for (int i = 0; i < pTestContext->strURIs.size(); i++) {
		//	pTestContext->pBrowserQuads[i]->SetDiffuseTexture(pTestContext->pDreamBrowsers[i]->GetScreenTexture().get());
		//}
		return R_PASS;
	};
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi-browser");
	pNewTest->SetTestDescription("Multi browser, will allow a net of users to share a chrome browser");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestUserControlAreaLayout() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::vector<std::string> strURIs;
		std::vector<std::shared_ptr<DreamBrowser>> pDreamBrowsers;
		std::vector<std::shared_ptr<quad>> pBrowserQuads;
		std::shared_ptr<DreamUserControlArea> pUserControlArea;
	} *pTestContext = new TestContext();
	
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		//No HMD
		float diagonalSize = 6.0f;
		//float diagonalSize = 0.6f;

		float aspectRatio = ((float)1366 / (float)768);
		float castWidth = std::sqrt(((aspectRatio * aspectRatio) * (diagonalSize * diagonalSize)) / (1.0f + (aspectRatio * aspectRatio)));
		float castHeight = std::sqrt((diagonalSize * diagonalSize) / (1.0f + (aspectRatio * aspectRatio)));

		//TODO: move these values to the proper app
		// test values based off of castWidth
		float borderWidth =	1.0323f * castWidth;
		float borderHeight = (0.594624) * castWidth; // 0.6237f
		
		float toolbarButtonWidth = 0.0645f * castWidth;
		float toolbarButtonHeight = 0.0645f * castWidth;

		float toolbarURLWidth = 0.5484f * castWidth;
		float toolbarURLHeight = 0.0655f * castWidth;

		float tabBarBorderWidth = 0.2962f * castWidth;
		float tabBarBorderHeight = 0.675269f * castWidth;

		float tabBarWindowWidth = 0.2640f * castWidth;
		float tabBarWindowHeight = 0.148387f * castWidth;

		float spaceSize = 0.016129 * castWidth;
		float fakeSpaceSize = 0.0164875 * castWidth;

		vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();

		point ptOrigin = point(0.0f, 0.0f, 0.0f);

		SetupPipeline();

		/*
		auto pControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this, false);
		pTestContext->pUserControlArea = pControlArea;
		pControlArea->GetComposite()->SetPosition(0.0f, -0.125f, 4.6f);
		pControlArea->GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * -(float)M_PI / 180.0f, 0.0f, 0.0f)));

		auto pWebBrowserManager = pTestContext->pUserControlArea->m_pWebBrowserManager;
		//*/

		//*
		auto pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		pWebBrowserManager->Initialize();
		//*/
		// starts to break down as the test approaches 20 browsers
		pTestContext->strURIs = {
			"www.nyt.com",
			"www.dreamos.com",
			"en.wikipedia.org/wiki/Tropical_house",
			"www.livelovely.com",
			"www.twitch.tv"
		} ;

		// Control View background
		auto pControlBackground = m_pDreamOS->MakeQuad(borderWidth, borderHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pControlBackground);
		pControlBackground->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-main-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pControlBackground->SetPosition(point(0.0f, 0.0f, -0.0005f));

		//TODO: move to Tabs app
		auto pTabBackground = m_pDreamOS->MakeQuad(tabBarBorderWidth, tabBarBorderHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pTabBackground);
		pTabBackground->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-list-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pTabBackground->SetPosition(point(borderWidth / 2.0f + spaceSize + tabBarBorderWidth / 2.0f, (borderHeight - tabBarBorderHeight) / 2, -0.0005f));

		//TODO: make control bar app
		point ptBarLeft = point(-borderWidth / 2.0f, -borderHeight / 2.0f - spaceSize - (toolbarButtonHeight / 2.0f), 0.0f);
		auto pBackButton = m_pDreamOS->MakeQuad(toolbarButtonWidth, toolbarButtonHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pBackButton);
		pBackButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-back.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pBackButton->SetPosition(ptBarLeft + point(toolbarButtonWidth / 2.0f, 0.0f, 0.0f));

		auto pForwardButton = m_pDreamOS->MakeQuad(toolbarButtonWidth, toolbarButtonHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pForwardButton);
		pForwardButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-forward.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pForwardButton->SetPosition(ptBarLeft + point(3 * toolbarButtonWidth / 2.0f + spaceSize, 0.0f, 0.0f));

		auto pCloseButton = m_pDreamOS->MakeQuad(toolbarButtonWidth, toolbarButtonHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pCloseButton);
		pCloseButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-close.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pCloseButton->SetPosition(ptBarLeft + point(5 * toolbarButtonWidth / 2.0f + 2*spaceSize, 0.0f, 0.0f));
		
		auto pURLButton = m_pDreamOS->MakeQuad(toolbarURLWidth, toolbarURLHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pURLButton);
		pURLButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-url.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		point ptURL = point(0.0f, ptBarLeft.y(), 0.0f);
		pURLButton->SetPosition(ptURL);

		point ptBarRight = ptURL + point(toolbarURLWidth / 2.0f + spaceSize, 0.0f, 0.0f);

		auto pShareButton = m_pDreamOS->MakeQuad(toolbarButtonWidth, toolbarButtonHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pShareButton);
		pShareButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pShareButton->SetPosition(ptBarRight+ point(toolbarButtonWidth / 2.0f, 0.0f, 0.0f));

		auto pOpenButton = m_pDreamOS->MakeQuad(toolbarButtonWidth, toolbarButtonHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pOpenButton);
		pOpenButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-open.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pOpenButton->SetPosition(ptBarRight + point(3*toolbarButtonWidth / 2.0f + spaceSize, 0.0f, 0.0f));

		auto pMinimizeButton = m_pDreamOS->MakeQuad(toolbarButtonWidth, toolbarButtonHeight, 1, 1, nullptr, vNormal);
		m_pDreamOS->AddObjectToUIGraph(pMinimizeButton);
		pMinimizeButton->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"control-view-minimize.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		pMinimizeButton->SetPosition(ptBarRight+ point(5*toolbarButtonWidth / 2.0f + 2*spaceSize, 0.0f, 0.0f));

		/*
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pControlBackground));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pTabBackground));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pBackButton));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pForwardButton));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pCloseButton));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pURLButton));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pShareButton));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pOpenButton));
		pControlArea->GetComposite()->AddObject(std::shared_ptr<quad>(pMinimizeButton));
		//*/


		point ptTabBarPosition = point(pTabBackground->GetPosition().x(), castHeight / 2.0f - tabBarWindowHeight / 2.0f, 0.0f);

		for (int i = 0; i < pTestContext->strURIs.size(); i++) {

			pTestContext->pDreamBrowsers.emplace_back(m_pDreamOS->LaunchDreamApp<DreamBrowser>(this));
			pTestContext->pDreamBrowsers[i]->InitializeWithBrowserManager(pWebBrowserManager, "about:blank");
			pTestContext->pDreamBrowsers[i]->SetURI(pTestContext->strURIs[i]);

			if (i == 0) {
				pTestContext->pBrowserQuads.emplace_back(m_pDreamOS->AddQuad(castWidth, castHeight, 1, 1, nullptr, vNormal));
				pTestContext->pBrowserQuads[i]->SetPosition(ptOrigin);
			}
			else {
				pTestContext->pBrowserQuads.emplace_back(m_pDreamOS->AddQuad(tabBarWindowWidth, tabBarWindowHeight, 1, 1, nullptr, vNormal));
				pTestContext->pBrowserQuads[i]->SetPosition(ptTabBarPosition - point(0.0f, ((i - 1)*(tabBarWindowHeight + fakeSpaceSize)), 0.0f));
			}
			//pControlArea->GetComposite()->AddObject(pTestContext->pBrowserQuads[i]);
			pTestContext->pBrowserQuads[i]->SetMaterialAmbient(0.90f);
			pTestContext->pBrowserQuads[i]->FlipUVVertical();
		}

//	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		//pTestContext->pUserControlArea->Update();

		for (int i = 0; i < pTestContext->strURIs.size(); i++) {
			pTestContext->pBrowserQuads[i]->SetDiffuseTexture(pTestContext->pDreamBrowsers[i]->GetSourceTexture().get());
		}
		return R_PASS;
	};
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi-browser");
	pNewTest->SetTestDescription("Multi browser, will allow a net of users to share a chrome browser");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestManyBrowsers() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::vector<std::string> strURIs;
		std::vector<std::shared_ptr<DreamBrowser>> pDreamBrowsers;
		std::vector<std::shared_ptr<quad>> pBrowserQuads;
		std::shared_ptr<CEFBrowserManager> pWebBrowserManager;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		float diagonalSize = 2.0f;
		float aspectRatio = ((float)1366 / (float)768);
		float castWidth = std::sqrt(((aspectRatio * aspectRatio) * (diagonalSize * diagonalSize)) / (1.0f + (aspectRatio * aspectRatio)));
		float castHeight = std::sqrt((diagonalSize * diagonalSize) / (1.0f + (aspectRatio * aspectRatio)));

		vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();
		CR(SetupPipeline());

		pTestContext->pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		CN(pTestContext->pWebBrowserManager);
		CR(pTestContext->pWebBrowserManager->Initialize());
		pTestContext->pWebBrowserManager->Update();

		// starts to break down as the test approaches 20 browsers
		pTestContext->strURIs = {
			"www.twitch.tv",
			"www.youtube.com",
			"www.google.com",
			"www.trello.com",
			"www.slack.com",
			"www.cnn.com",
			"www.nyt.com",
			"www.fivethirtyeight.com",
			"www.washingtonpost.com",
			"www.spotify.com",
			"www.amazon.com",
			/*
			"www.bandcamp.com",
			"www.messenger.com",
			"www.theindependentsf.com",
			"www.thechapelsf.com",
			"www.bottomofthehill.com",
			"www.dreamos.com",
			"mail.google.com",
			"facebook.com",
			"www.reddit.com"
			//*/
		} ;

		for (int i = 0; i < pTestContext->strURIs.size(); i++) {

			pTestContext->pDreamBrowsers.emplace_back(m_pDreamOS->LaunchDreamApp<DreamBrowser>(this));
			pTestContext->pDreamBrowsers[i]->InitializeWithBrowserManager(pTestContext->pWebBrowserManager, "about:blank");
			pTestContext->pDreamBrowsers[i]->SetURI(pTestContext->strURIs[i]);
		}
		for (int i = 0; i < pTestContext->strURIs.size(); i++) {
			/*
			WebRequest webRequest;
			std::string strEnvironmentAssetURL = pTestContext->strURIs[i];
			std::wstring wstrAssetURL = util::StringToWideString("https://" + strEnvironmentAssetURL + "/");
			CR(webRequest.SetURL(wstrAssetURL));
			CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
			pTestContext->pDreamBrowsers[i]->LoadRequest(webRequest);
			//*/

			pTestContext->pBrowserQuads.emplace_back(std::shared_ptr<quad>(m_pDreamOS->AddQuad(castWidth, castHeight, 1, 1, nullptr, vNormal)));
			pTestContext->pBrowserQuads[i]->SetMaterialAmbient(0.90f);
			pTestContext->pBrowserQuads[i]->FlipUVVertical();
			pTestContext->pBrowserQuads[i]->SetPosition(point(((i%5)*castWidth * 1.05f)-4.0f, 2.0f - ((i/5)*castHeight * 1.05f), 0.0f));

		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);

		//pTestContext->pWebBrowserManager->Update();
		for (int i = 0; i < pTestContext->strURIs.size(); i++) {
			pTestContext->pBrowserQuads[i]->SetDiffuseTexture(pTestContext->pDreamBrowsers[i]->GetSourceTexture().get());
		}
		return R_PASS;
	};
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi-browser");
	pNewTest->SetTestDescription("Multi browser, will allow a net of users to share a chrome browser");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestMultiPeerBasic() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public CloudController::PeerConnectionObserver {

		CloudController *pCloudController = nullptr;

		// PeerConnectionObserver
		virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
			DEVENV_LINEOUT("OnNewPeerConnection");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnNewSocketConnection(int seatPosition) {
			DEVENV_LINEOUT("OnNewSocketConnection");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) {
			DEVENV_LINEOUT("OnPeerConnectionClosed");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDreamMessage) {
			DEVENV_LINEOUT("OnDataMessage");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) {
			DEVENV_LINEOUT("OnDataStringMessage");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
			//DEVENV_LINEOUT(L"OnAudioData");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) {
			DEVENV_LINEOUT("OnDataChannel");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) {
			DEVENV_LINEOUT("OnAudioChannel");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			//DEVENV_LINEOUT(L"OnVideoFrame");

			return R_NOT_HANDLED;
		}

	} *pTestContext = new TestContext();


	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		DOSLOG(INFO, "[WebRTCTestingSuite] Multipeer Test Initializing ... ");

		CR(SetupPipeline());

		m_pTestQuad = std::shared_ptr<quad>(m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f)));
		m_pDreamOS->AddObjectToInteractionGraph(m_pTestQuad.get());

		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), INTERACTION_EVENT_SELECT_DOWN, this);
		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), ELEMENT_INTERSECT_BEGAN, this);
		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), ELEMENT_COLLIDE_MOVED, this);
		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), ELEMENT_COLLIDE_ENDED, this);

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		m_pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this, true);
		m_pDreamShareView->Show();

		m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		//m_pDreamShareView->ShowCast

		// Command Line Manager
		CommandLineManager *pCommandLineManager;
		pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		/*
		// Cloud Controller
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(m_pDreamOS->GetCloudController()->Initialize(), "Failed to initialize cloud controller");

		CRM(m_pDreamOS->GetCloudController()->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");
		//*/

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			std::string strUsername = "test";
			m_strID =  pCommandLineManager->GetParameterValue("testval");
			strUsername += m_strID;
			strUsername += "@dreamos.com";

			long environmentID = 170;
			std::string strPassword = "nightmare";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", strPassword));

			CRM(m_pDreamOS->GetCloudController()->Start(strUsername, strPassword, environmentID), "Failed to log in");

			if (m_strID == "1") {
				//m_pTestTexture = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
				m_pTestTexture = m_pTestTextureUser1;
			}
			else if (m_strID == "2") {
				//m_pTestTexture = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
				m_pTestTexture = m_pTestTextureUser2;
			}

			m_pDreamShareView->SetCastingTexture(m_pTestTexture);
			m_pDreamShareView->ShowCastingTexture();
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");

	Error:
		return r;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	// Add the test
	//auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi-browser");
	pNewTest->SetTestDescription("Multi browser, will allow a net of users to share a chrome browser");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestMultiPeerBrowser() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public CloudController::PeerConnectionObserver {

		CloudController *pCloudController = nullptr;

		// PeerConnectionObserver
		virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
			DEVENV_LINEOUT("OnNewPeerConnection");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnNewSocketConnection(int seatPosition) {
			DEVENV_LINEOUT("OnNewSocketConnection");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) {
			DEVENV_LINEOUT("OnPeerConnectionClosed");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDreamMessage) {
			DEVENV_LINEOUT("OnDataMessage");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) {
			DEVENV_LINEOUT("OnDataStringMessage");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
			//DEVENV_LINEOUT(L"OnAudioData");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) {
			DEVENV_LINEOUT("OnDataChannel");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) {
			DEVENV_LINEOUT("OnAudioChannel");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			//DEVENV_LINEOUT(L"OnVideoFrame");

			return R_NOT_HANDLED;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		DOSLOG(INFO, "[WebRTCTestingSuite] Multipeer Test Initializing ... ");

		CR(SetupPipeline());

		m_pTestQuad = std::shared_ptr<quad>(m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f)));
		m_pDreamOS->AddObjectToInteractionGraph(m_pTestQuad.get());

		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), INTERACTION_EVENT_SELECT_DOWN, this);
		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), ELEMENT_INTERSECT_BEGAN, this);
		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), ELEMENT_COLLIDE_MOVED, this);
		m_pDreamOS->RegisterEventSubscriber(m_pTestQuad.get(), ELEMENT_COLLIDE_ENDED, this);
		
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		m_pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this, true);
		m_pDreamShareView->Show();

		m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);

		m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		//m_pDreamShareView->ShowCast

		// Command Line Manager
		CommandLineManager *pCommandLineManager;
		pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		/*
		// Cloud Controller
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(m_pDreamOS->GetCloudController()->Initialize(), "Failed to initialize cloud controller");

		CRM(m_pDreamOS->GetCloudController()->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");
		//*/

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			std::string strUsername = "test";
			m_strID =  pCommandLineManager->GetParameterValue("testval");
			strUsername += m_strID;
			strUsername += "@dreamos.com";

			long environmentID = 170;
			std::string strPassword = "nightmare";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", strPassword));

			CRM(m_pDreamOS->GetCloudController()->Start(strUsername, strPassword, environmentID), "Failed to log in");

			if (m_strID == "1") {
				m_strURL = "https://www.youtube.com/watch?v=5vZ4lCKv1ik";
				//m_pTestTexture = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
				//m_pTestTexture = m_pTestTextureUser1;
			}
			else if (m_strID == "2") {
				//m_pTestTexture = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
				//m_pTestTexture = m_pTestTextureUser2;
				m_strURL = "https://www.youtube.com/watch?v=8ulEMXUNyRo";
			}

			//m_pDreamShareView->SetCastingTexture(m_pTestTexture);
			//m_pDreamShareView->ShowCastingTexture();
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");

	Error:
		return r;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	// Add the test
	//auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Multi-browser");
	pNewTest->SetTestDescription("Multi browser, will allow a net of users to share a chrome browser");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT MultiContentTestSuite::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	if (pEvent->m_pObject == m_pTestQuad.get() || m_fTestQuadActive) {
		switch (pEvent->m_eventType) {
			case ELEMENT_INTERSECT_BEGAN: {
				m_fTestQuadActive = true;
			} break;

			case ELEMENT_INTERSECT_ENDED: {
				m_fTestQuadActive = false;
			} break;

			case INTERACTION_EVENT_SELECT_DOWN: {
				if (!m_pDreamShareView->IsStreaming()) {
					if (m_pDreamShareView->m_fReceivingStream) {
						CR(m_pDreamOS->UnregisterVideoStreamSubscriber(m_pDreamShareView.get()));
					}
					m_pDreamShareView->m_fReceivingStream = false;
					/*
					if (m_strID == "1") {
						m_pTestTextureUser1 = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
						m_pTestTexture = m_pTestTextureUser1;
					}
					else if (m_strID == "2") {
						m_pTestTextureUser2 = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
						m_pTestTexture = m_pTestTextureUser2;
					}
					//*/
					m_pDreamBrowser->SetURI(m_strURL);
					m_pDreamShareView->SetCastingTexture(m_pTestTexture);
					m_pDreamShareView->ShowCastingTexture();

					m_pDreamShareView->SetStreamingState(false);
					CR(m_pDreamShareView->BroadcastDreamShareViewMessage(DreamShareViewMessage::type::REQUEST_STREAMING_START));
					m_pDreamShareView->SetStreamingState(true);
				}

			} break;

		}
	}
Error:
	return r;
}

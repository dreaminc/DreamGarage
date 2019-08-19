#include "UITestSuite.h"

#include "os/DreamOS.h"

#include "modules/PhysicsEngine/CollisionManifold.h"
#include "modules/InteractionEngine/InteractionObjectEvent.h"

#include "modules/AnimationEngine/AnimationCurve.h"
#include "modules/AnimationEngine/AnimationItem.h"

#include "apps/DreamContentViewApp/DreamContentViewApp.h"
#include "apps/DreamBrowserApp/DreamBrowserApp.h"
#include "apps/DreamUIBarApp/DreamUIBarApp.h"

#include "webbrowser/CEFBrowser/CEFBrowserManager.h"

#include "cloud/WebRequest.h"
#include "cloud/Environment/EnvironmentAsset.h"
#include "cloud/Menu/MenuNode.h"
#include "cloud/HTTP/HTTPController.h"
#include "cloud/WebRequest.h"

#include "ui/UIMenuItem.h"
#include "ui/UIKeyboard.h"	// TODO: Keyboard should be a module
#include "ui/UIKeyboardLayout.h"
#include "ui/UIEvent.h"
#include "ui/UIView.h"
#include "ui/UIButton.h"

#include "pipeline/ProgramNode.h"
#include "pipeline/SinkNode.h"
#include "pipeline/SourceNode.h"

#include "sandbox/CommandLineManager.h"

#include "Core/Utilities.h"

#include "core/text/font.h"
#include "core/text/text.h"

#include "core/primitives/framebuffer.h"
#include "core/hand/hand.h"

#include "scene/ObjectStoreNode.h"
#include "scene/CameraNode.h"

UITestSuite::UITestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("ui", pDreamOS)
{
	RESULT r = R_PASS;

	CN(m_pDreamUIBar);
	CR(Initialize());
	 
	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT UITestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestUIView());

	// TODO: This test is broken
	CR(AddTestBrowserRequestWithMenuAPI());

	CR(AddTestFlatContextCompositionQuads());
	
	CR(AddTestFont());

	CR(AddTestBrowserURL());

	CR(AddTestSharedContentView());

	//CR(AddTestBrowserRequest());

	CR(AddTestUIMenuItem());

	//CR(AddTestBrowserRequestWithMenuAPI());

	//CR(AddTestBrowserRequest());

	//CR(AddTestKeyboard());

	//CR(AddTestInteractionFauxUI());

Error:
	return r;
}


RESULT UITestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	CNM(m_pDreamOS, "DreamOS handle is not set");

	m_pDreamOS->SetGravityState(false);

	CR(SetupPipeline());

	light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

	/*
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	m_pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	point sceneOffset = point(90, -5, -25);
	float sceneScale = 0.1f;
	vector sceneDirection = vector(0.0f, 0.0f, 0.0f);
	*/

	/*
	m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\env.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);

	composite* pRiver = m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\river.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);

	m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\clouds.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
	//*/

	/*
	for (int i = 0; i < SenseControllerEventType::SENSE_CONTROLLER_INVALID; i++) {
		CR(m_pDreamOS->RegisterSubscriber((SenseControllerEventType)(i), this));
	}

	for (int i = 0; i < SenseMouseEventType::SENSE_MOUSE_INVALID; i++) {
		CR(m_pDreamOS->RegisterSubscriber((SenseMouseEventType)(i), this));
	}
	//*/

Error:
	return r;
}

/*
RESULT UITestSuite::SetupPipeline(std::string strRenderProgramName) {
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

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}
*/

RESULT UITestSuite::SetupPipeline(std::string strRenderProgramName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderProgramName);
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
	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("minimal_texture", PIPELINE_FLAGS::PASSTHRU);
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

RESULT UITestSuite::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	CR(pMenuNode->PrintMenuNode());

	if (pMenuNode->NumSubMenuNodes() > 0) {
		auto pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		for (auto &pSubMenuNode : pMenuNode->GetSubMenuNodes()) {
			std::string strScope = pSubMenuNode->GetScope();
			std::string strPath = pSubMenuNode->GetPath();
			std::string strTitle = pSubMenuNode->GetTitle();

			if (pSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
				if (strTitle == "Share" || 
					strTitle == "File" || 
					strTitle == "Google Drive" || 
					strTitle == "Golden" ||
					strTitle == "People") 
				{
					CRM(pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle), "Failed to request sub menu");
					return r;
				}
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {
				auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");

				return r;
			}
		}
	}

Error:
	return r;
}

RESULT UITestSuite::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	//https://api.develop.dreamos.com/environment-asset/{id}/file
	
	if (m_pDreamBrowser != nullptr) {
		//CR(m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset));
		//m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset);
		WebRequest webRequest;

		std::wstring strEnvironmentAssetURI = util::StringToWideString(pEnvironmentAsset->GetURI());

		webRequest.SetURL(strEnvironmentAssetURI);
		//webRequest.SetURL(L"http://www.youtube.com");

		auto pUserControllerProxy = dynamic_cast<UserControllerProxy*>(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
		CN(pUserControllerProxy);
		std::string strTokenValue = "Bearer " + pUserControllerProxy->GetUserToken();
		std::wstring wstrTokenValue = util::StringToWideString(strTokenValue);

		CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
		CR(webRequest.AddRequestHeader(L"Authorization", wstrTokenValue));
		//CR(webRequest.AddRequestHeader(L"Authorization", L"Tokenz"));

		// NOTE: this is kind of working, data is clearly being sent but there's
		// no real support for form/file etc yet
		// This is not yet needed
		// TODO: Break this out into a separate UI suite (Browser/CEF)
		//CR(webRequest.AddPostDataElement(L"post data element"));

		CR(m_pDreamBrowser->LoadRequest(webRequest));
	}
	

Error:
	return r;
}

RESULT UITestSuite::AddTestFlatContextCompositionQuads() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::shared_ptr<FlatContext> pFlatContext;
		quad *pRenderQuad;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		CN(m_pDreamOS);

		{
			TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
			///*
			//auto pFlatContext = m_pDreamOS->AddFlatContext();
			auto pComposite = m_pDreamOS->MakeComposite();
			auto pFlatContext = pComposite->AddFlatContext();
			pTestContext->pFlatContext = pFlatContext;
			//auto pFlatContext = pComposite->MakeFlatContext();
			CN(pFlatContext);

			auto pRenderQuad = m_pDreamOS->AddQuad(6.0f, 5.0f);
			pTestContext->pRenderQuad = pRenderQuad;

			pFlatContext->SetIsAbsolute(true);
			pFlatContext->SetAbsoluteBounds(pRenderQuad->GetWidth(), pRenderQuad->GetHeight());
			
			//pFlatContext->InitializeOBB();
			pFlatContext->InitializeBoundingQuad();

//			auto pComposite = m_pDreamOS->MakeComposite();
			auto pView = pComposite->AddUIView(m_pDreamOS);
			auto pButton = pView->AddUIButton();
			pButton->GetSurface()->SetVertexColor(COLOR_YELLOW);
			pButton->SetPosition(point(-1.0f, 0.0f, 0.0f));

			m_pDreamOS->AddObject(pButton.get());
			//pButton->RotateXByDeg(180.0f);
			//pFlatContext->AddObject(pButton);
			auto pSphere = m_pDreamOS->MakeSphere(0.5f, 10, 10);
			pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));

			pFlatContext->AddObject(std::shared_ptr<sphere>(pSphere));

			//auto pComposite2 = m_pDreamOS->AddComposite();
			auto pComposite2 = m_pDreamOS->MakeComposite();
			//pComposite2->RotateXByDeg(90.0f);
			auto pCQuad = pComposite2->AddQuad(0.5f, 0.5f);
			//pCQuad->SetPosition(point(-1.0f, 0.0f, -1.0f));
			pComposite2->SetPosition(point(-1.0f, 0.0f, -1.0f));

//			pFlatContext->AddObject(std::shared_ptr<composite>(pComposite2));
			pFlatContext->AddChild(std::shared_ptr<composite>(pComposite2));

			auto pQuad = pFlatContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, 1.0f));
			CN(pQuad);
			pQuad->SetVertexColor(COLOR_BLUE);

			m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
				pQuad.get(),
				pQuad->GetPosition() + point(0.0f, 0.0f, 2.0f),
				pQuad->GetOrientation(),
				pQuad->GetScale(),
				2.0,
				AnimationCurveType::LINEAR,
				AnimationFlags::AnimationFlags()
			);

			/*
			pQuad = pFlatContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, 0.0f));
			CN(pQuad);
			pQuad->SetVertexColor(COLOR_GREEN);

			m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
				pQuad.get(),
				pQuad->GetPosition(),
				pQuad->GetOrientation(),
				pQuad->GetScale() * 2.0f ,
				2.0,
				AnimationCurveType::LINEAR,
				AnimationFlags::AnimationFlags()
			);
			//*/

			pQuad = pFlatContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, -1.0f));
			CN(pQuad);
			pQuad->SetVertexColor(COLOR_RED);

			/*
			pQuad = pFlatContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, -2.0f));
			CN(pQuad);
			pQuad->SetVertexColor(COLOR_YELLOW);
			//*/

			pQuad = pFlatContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.0f, -3.0f));
			CN(pQuad);
			pQuad->SetVertexColor(COLOR_WHITE);

			m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
				pQuad.get(),
				pQuad->GetPosition() - point(0.0f, 0.0f, 2.0f),
				pQuad->GetOrientation(),
				pQuad->GetScale(),
				2.0,
				AnimationCurveType::LINEAR,
				AnimationFlags::AnimationFlags()
			);

			//pFlatContext->SetBounds(0.5f, 2.0f);
			//pFlatContext->GetOff

			pFlatContext->RotateXByDeg(90.0f);
			//*/

//			pFlatContext->RenderToTexture();

			auto pTexture = pFlatContext->GetFramebuffer()->GetColorTexture();
			//*
			//auto pRenderQuad = m_pDreamOS->AddQuad(2.0f, 2.0f);
			pRenderQuad->RotateXByDeg(90.0f);

//			pFlatContext->SetIsAbsolute(false);
			//pFlatContext->SetBounds(pRenderQuad->GetWidth(), pRenderQuad->GetHeight());
			pFlatContext->RenderToQuad(pRenderQuad, 0.0f, 0.0f);
			//pRenderQuad->SetDiffuseTexture(pTexture);
			//pQuad->SetColor(COLOR_BLUE);

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

		CR(r);
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		pTestContext->pFlatContext->RenderToQuad(pTestContext->pRenderQuad, 0.0f, 0.0f);

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

	auto pUITest = AddTest("flatcontextcomposition", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Flat context composition test");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestUIMenuItem() {
	RESULT r = R_PASS;
	
	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		{
			auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
			auto pComposite = m_pDreamOS->MakeComposite();
			auto pView = pComposite->AddUIView(m_pDreamOS);
			auto pMenuItem = pView->AddUIMenuItem();
			m_pDreamOS->AddObjectToUIGraph(pComposite);

			IconFormat i = IconFormat();
			LabelFormat l = LabelFormat();

			l.pFont = pFont;
			l.strLabel = "testing";

			texture* pPNG = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"icons_600\\icon_png_600.png");
			i.pTexture = pPNG;

			pMenuItem->Update(i, l);
			pMenuItem->SetPosition(point(0.0f, 0.0f, 2.0));
			//pMenuItem->SetOrientation()
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

	auto pUITest = AddTest("uimenuitem", fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS->GetCloudController());
	CN(pUITest);

	pUITest->SetTestDescription("Test for UI menu item");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: Move this test somewhere else.  Should text have it's own test suite?
RESULT UITestSuite::AddTestFont() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	
	struct TestContext {
		text *pText = nullptr;
		std::chrono::system_clock::time_point timeLastUpdate; 
	} *pTestContext = new TestContext();

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		float lineHeight = 0.35f;

		CN(m_pDreamOS);

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		{
			auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
			pFont->SetLineHeight(lineHeight);
			CN(pFont);

			// Fit to Scale
			//*
			auto pText = m_pDreamOS->AddText(pFont, "", lineHeight * 5.0f, lineHeight * 4.0f , text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD);
			//auto pText = m_pDreamOS->AddText(pFont, "", lineHeight * 5.0f, lineHeight, text::flags::TRAIL_ELLIPSIS);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, 0.0f, 0.0f));

			pTestContext->pText = pText;
			pTestContext->timeLastUpdate = std::chrono::system_clock::now();
			//*/

			/*
			auto pText = m_pDreamOS->AddText(pFont, "abcdefghijklmnopqrstuvwxyz", lineHeight * 5.0f, lineHeight * 2.6f, text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD);
			//auto pText = m_pDreamOS->AddText(pFont, "abc def ghi jkl mno pqr stu vwx yz", lineHeight * 5.0f, lineHeight * 4.0f, text::flags::WRAP );
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, 0.0f, 0.0f));
			//*/

			//pText->SetText("testing this \nthing");

			//*
			pText = m_pDreamOS->AddText(pFont, "Testing this \nthing", 0.6f, text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD);
			CN(pText);
			//pText->RenderToQuad();
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(-3.0f, 2.0f, 0.0f));
			
			pText->SetText("testing this \nthing");

			///*
			// Size to fit
			// Note this sets the line height by way of font - this teases at future settings, 
			// but right now it's avoiding adding MORE constructor paths / vars to this creation path
			pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.0f, 0.6f, text::flags::SCALE_TO_FIT | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(2.0f, -2.0f, 0.0f));
			pText->SetText("testing this thing");

			pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.0f, 0.6f, text::flags::NONE | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(2.0f, 2.0f, 0.0f));
			pText->SetText("testing this thing");

			pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.1f, 0.6f, text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(-3.0f, -2.0f, 0.0f));
			pText->SetText("testing this thing");

			pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.0f, 0.6f, text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(-3.0f, 0.0f, 0.0f));
			pText->SetText("testing this thing");

			pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.0f, 0.6f, text::flags::WRAP | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(3.0f, 0.0f, 0.0f));
			pText->SetText("testing this thing");

			pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.3f, 0.6f, text::flags::LEAD_ELLIPSIS | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, -2.0f, 0.0f));
			pText->SetText("testing this thing");

			pText = m_pDreamOS->AddText(pFont, "password", 1.3f, 0.6f, text::flags::PASSWORD | text::flags::RENDER_QUAD);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, -1.2f, 0.0f));

			//expect "stestsword"
			pText = m_pDreamOS->AddText(pFont, "password", 1.7f, 0.6f, text::flags::USE_CURSOR | text::flags::RENDER_QUAD);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, 1.2f, 0.0f));
			pText->SetCursorIndex(3);
			pText->AddCharacter("t");
			pText->AddCharacter("e");
			pText->AddCharacter("s");
			pText->AddCharacter("t");

			pText->SetCursorIndex(2);
			pText->RemoveCharacter();
			pText->RemoveCharacter();
			pText->RemoveCharacter();
			
			// expect "passwordt"
			pText = m_pDreamOS->AddText(pFont, "password", 2.0f, 0.6f, text::flags::RENDER_QUAD);
			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(0.0f, 2.2f, 0.0f));

			// cursor index is not used while the USE_CURSOR flag is unset
			pText->SetCursorIndex(3);
			pText->AddCharacter("t");
			pText->AddCharacter("e");
			pText->AddCharacter("s");
			pText->AddCharacter("t");
			
			pText->SetCursorIndex(2);
			pText->RemoveCharacter();
			pText->RemoveCharacter();
			pText->RemoveCharacter();
			//*/

			// Layout
			/*
			auto pLayout = new UIKeyboardLayout();
			CN(pLayout);
			CR(pLayout->CreateQWERTYLayout());

			texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"Key-Dark-1024.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
			CN(pColorTexture1);
			pLayout->SetKeyTexture(pColorTexture1);

			auto pText = m_pDreamOS->AddText(pFont, pLayout, 0.25f, text::flags::RENDER_QUAD);
			//auto pText = m_pDreamOS->AddText(pFont, pLayout, .025f, 0.025f, 0.01f, 0.01f);
			CN(pText);
			pText->RotateXByDeg(90.0f);
			//*/

			// background
			/*
			//auto pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.0f, 0.6f, text::flags::WRAP | text::flags::RENDER_QUAD);
			auto pText = m_pDreamOS->AddText(pFont, "Testing this thing", 1.0f, 0.6f, text::flags::WRAP);
			CN(pText);

			texture *pColorTexture1 = m_pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
			//pText->SetBackgroundColor(COLOR_BLUE);
			pText->SetBackgroundColorTexture(pColorTexture1);

			pText->RenderToQuad();

			pText->RotateXByDeg(90.0f);
			pText->SetPosition(point(3.0f, 0.0f, 0.0f));
			*/

			//pQuad = AddQuad(slsld)
			//pText->GetColorTexture()

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

		/*
		static char c = 'a';
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		if (pTestContext->pText != nullptr && 
			std::chrono::duration_cast<std::chrono::seconds>(timeNow - pTestContext->timeLastUpdate).count() >= 1) 
		{
			std::string strText = pTestContext->pText->GetText();
			strText += c++;

			pTestContext->pText->SetText(strText);

			pTestContext->timeLastUpdate = timeNow;
		}
		*/

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

	auto pUITest = AddTest("font", fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS->GetCloudController());
	CN(pUITest);

	pUITest->SetTestDescription("Font creation test");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: This should be moved into it's own test suite for browser / API or both
RESULT UITestSuite::AddTestBrowserRequestWithMenuAPI() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::string strURL = "http://www.youtube.com";

		CN(m_pDreamOS);

		// Create the Browser
		m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(m_pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		m_pDreamBrowser->SetDiagonalSize(10.0f);

		m_pDreamBrowser->SetPosition(point(0.0f, 1.0f, 0.0f));

		// Cloud Controller
		CNM(m_pDreamOS->GetCloudController(), "CloudController is not initialzed, check config flags");
		CloudController *pCloudController = reinterpret_cast<CloudController*>(m_pDreamOS->GetCloudController());

		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		MenuControllerProxy *pMenuControllerProxy = nullptr;
		CN(pContext);
		CN(pCloudController);
		CN(pCommandLineManager);

		// For later
		m_pCloudController = pCloudController;

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");

		// Log in 
		{
			std::string strUsername = pCommandLineManager->GetParameterValue("username");
			std::string strPassword = pCommandLineManager->GetParameterValue("password");
			std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");

			CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
		}

		//CR(pCloudController->RegisterEnvironmentAssetCallback(std::bind(&UITestSuite::HandleOnEnvironmentAsset, this, std::placeholders::_1)));
		CR(pCloudController->RegisterEnvironmentObserver(this));
		
		Sleep(1000);

		// Set up menu stuff
		DEBUG_LINEOUT("Requesting Menu");
		pMenuControllerProxy = (MenuControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		CRM(pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");
		CRM(pMenuControllerProxy->RequestSubMenu("", "", "menu"), "Failed to request sub menu");

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

	auto pUITest = AddTest("browserrequest", fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of browser working with a web request");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: Browser probably needs it's own test suite
RESULT UITestSuite::AddTestBrowserURL() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		//std::string strURL = "http://www.youtube.com";
		std::string strURL = "https://www.youtube.com/watch?v=K0igLdIH-Zc";

		WebRequest webRequest;

		CN(m_pDreamOS);		

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

	auto pUITest = AddTest("browserurl", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of browser working with a URL");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestBrowserRequest() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::string strURL = "http://www.youtube.com";

		WebRequest webRequest;

		CN(m_pDreamOS);

		// Create the Shared View App
		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(10.0f);

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		//pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
		//pDreamBrowser->SetURI(strURL);

		//webRequest.SetURL(L"http://httpbin.org/get");
		//CR(webRequest.SetURL(L"http://www.cnn.com"));
		//webRequest.SetURL(L"https://placehold.it/350x150");
		webRequest.SetURL(L"http://placehold.it/350x150/A00AAA/000000");

		CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
		CR(webRequest.AddRequestHeader(L"Authorization", L"Bearer "));

		// NOTE: this is kind of working, data is clearly being sent but there's
		// no real support for form/file etc yet
		// This is not yet needed
		// TODO: Break this out into a separate UI suite (Browser/CEF)
		//CR(webRequest.AddPostDataElement(L"post data element"));

		//CR(pDreamBrowser->LoadRequest(webRequest));

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

	auto pUITest = AddTest("browserwebrequest", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of browser working with a web request");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestUIView() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		{
			auto pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);
			pComposite->InitializeOBB();

			auto pView = pComposite->AddUIView(m_pDreamOS);
			CN(pView);
			pView->InitializeOBB();
			pView->SetPosition(point(0.0f, 1.0f, 2.0f));
			
			auto pQuad = pView->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector());
			//pQuad->SetPosition(point(0.0f, 1.0f, 2.0f));

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

	auto fnUpdate = [&](void *pContext) {
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
		
	Error:
		return r;
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

	auto pUITest = AddTest("uiview", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UITestSuite::AddTestSharedContentView() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamContentView> pDreamContentView = nullptr;

		CN(m_pDreamOS);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the Shared View App
		pDreamContentView = m_pDreamOS->LaunchDreamApp<DreamContentView>(this);
		CNM(pDreamContentView, "Failed to create dream content view");

		// Set up the view
		pDreamContentView->SetParams(point(0.0f), 5.0f, DreamContentView::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
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

	Error:
		return r;
	};

	auto pUITest = AddTest("sharedcontentview", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: What is this test exactly?
RESULT UITestSuite::AddTestInteractionFauxUI() {
	RESULT r = R_PASS;

	struct TestContext {
		DimRay *pRay = nullptr;
		composite *pComposite = nullptr;
	};

	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pChildComposite = nullptr;
		std::shared_ptr<composite> pChildItemComposite = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;

		pTestContext->pRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));
		CN(pTestContext->pRay);


		m_pSphere1 = m_pDreamOS->AddSphere(0.02f, 10, 10);
		m_pSphere2 = m_pDreamOS->AddSphere(0.02f, 10, 10);

		// Create Faux UI here

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		pTestContext->pComposite = pComposite;
		pComposite->InitializeOBB();

		//pComposite->SetMass(1.0f);

		// Layer
		pChildComposite = pComposite->AddComposite();
		CN(pChildComposite);
		CR(pChildComposite->InitializeOBB());

		// Quads
		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(20.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(10.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
		//pChildItemComposite->RotateYByDeg(0.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(1.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(-10.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(2.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(-20.0f);

		// Move Composite
		pChildComposite->SetPosition(point(0.0f, 0.0f, -4.0f));

		pComposite->SetPosition(point(0.0f, 1.5f, 6.0f));

		// Add composite to interaction
		//CR(m_pDreamOS->AddInteractionObject(pComposite));

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		hand* pRightHand = m_pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT);

		if (pRightHand != nullptr && pTestContext->pRay != nullptr) {
			pTestContext->pRay->SetPosition(pRightHand->GetPosition());
			pTestContext->pRay->SetOrientation(pRightHand->GetHandState().qOrientation);

			point p0 = pRightHand->GetPosition();
			//GetLookVector
			quaternion q = pRightHand->GetHandState().qOrientation;
			q.Normalize();

			vector v = q.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();
			vector v2 = vector(-v.x(), -v.y(), v.z());
			vector vHandLook = RotationMatrix(q) * vector(0.0f, 0.0f, -1.0f);

			ray rcast = ray(p0, vHandLook);

			CollisionManifold manifold = pTestContext->pComposite->Collide(rcast);

			if (manifold.NumContacts() > 0) {
				int numContacts = manifold.NumContacts();

				if (numContacts > 2)
					numContacts = 2;
				for (int i = 0; i < numContacts; i++) {
					sphere *pSphere = (i == 0) ? m_pSphere1 : m_pSphere2;

					if (pSphere != nullptr) {
						pSphere->SetVisible(true);
						pSphere->SetPosition(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}

			//m_pDreamOS->UpdateInteractionPrimitive(rcast);

		}
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest("fauxui", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("UI Basic Testing Environment");
	pUITest->SetTestDuration(10000.0);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

// TODO: fix with new console 
RESULT UITestSuite::Notify(SenseControllerEvent *event) {
	RESULT r = R_PASS;

	SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;
	//OVERLAY_DEBUG_SET("event", "none");

	if (event->state.type == CONTROLLER_RIGHT) {
		if (eventType == SENSE_CONTROLLER_TRIGGER_MOVE) {
			//OVERLAY_DEBUG_SET("event", "trigger move");
		}
		else if (eventType == SENSE_CONTROLLER_PAD_MOVE) {
			//OVERLAY_DEBUG_SET("event", "pad move");
		}

		else if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
			//OVERLAY_DEBUG_SET("event", "trigger down");
		}

		// TODO:  soon this code will be replaced with api requests, 
		// as opposed to accessing the hard coded local data structures
		else if (eventType == SENSE_CONTROLLER_TRIGGER_UP) {
			//OVERLAY_DEBUG_SET("event", "trigger up");
//			CR(m_pDreamUIBar->HandleSelect());
		}
		else if (eventType == SENSE_CONTROLLER_MENU_UP) {
			//OVERLAY_DEBUG_SET("event", "menu up");
//			CR(m_pDreamUIBar->HandleMenuUp());
		}
	}
	else if (eventType == SENSE_CONTROLLER_GRIP_DOWN) {
		//OVERLAY_DEBUG_SET("event", "grip down");
	}
	else if (eventType == SENSE_CONTROLLER_GRIP_UP) {
		//OVERLAY_DEBUG_SET("event", "grip up");
	}
	else if (eventType == SENSE_CONTROLLER_MENU_DOWN) {
		//OVERLAY_DEBUG_SET("event", "menu down");
	}
//Error:
	return r;
}

RESULT UITestSuite::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;
	CR(r);
Error:
	return r;
}

RESULT UITestSuite::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

//Error:
	return r;
}

RESULT UITestSuite::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->m_eventType) {
	case (UIEventType::UI_EVENT_INTERSECT_BEGAN): {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pEvent->m_pObj);
		
		if (pDimObj != nullptr) {
			pDimObj->RotateZByDeg(45.0f);
		}
	} break;
	case (UIEventType::UI_EVENT_INTERSECT_ENDED): {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pEvent->m_pObj);

		if (pDimObj != nullptr) {
			pDimObj->ResetRotation();
		}
	} break;
	}

//Error:
	return r;
}


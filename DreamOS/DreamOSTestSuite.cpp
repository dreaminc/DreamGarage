#include "DreamOSTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/UIStageProgram.h"
#include "HAL/EnvironmentProgram.h"

#include "DreamTestingApp.h"
#include "DreamUserApp.h"
#include "UI\UIKeyboard.h"
#include "DreamGarage\DreamUIBar.h"
#include "DreamControlView\DreamControlView.h"
#include "DreamGarage\DreamDesktopDupplicationApp\DreamDesktopApp.h"
#include "DreamShareView\DreamShareView.h"
#include "DreamGarage\DreamDesktopDupplicationApp\DreamDesktopApp.h"

#include "DreamGarage\DreamBrowser.h"
#include "DreamGarage\Dream2DMouseApp.h"
#include "WebBrowser\WebBrowserController.h"
#include "WebBrowser\CEFBrowser/CEFBrowserManager.h"

#include <chrono>

#include <windows.h>
#include <windowsx.h>
#include "DDCIPCMessage.h"

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

	CR(AddTestDreamLogger());

	CR(AddTestMeta());

	CR(AddTestDreamBrowser());
	
	CR(AddTestDreamDesktop());

	CR(AddTestDreamOS());

	// Casting tests

	CR(AddTestBasicBrowserCast());

	CR(AddTestDreamShareView());

	CR(AddTestDreamOS());

	CR(AddTestUserApp());

	CR(AddTestCaptureApp());

	CR(AddTestDreamApps());

	CR(AddTestUIKeyboard());

	CR(AddTestDreamUIBar());

	CR(AddTestCaptureApp());


Error:
	return r;
}

RESULT DreamOSTestSuite::SetupPipeline(std::string strRenderProgramName) {
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

	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT DreamOSTestSuite::SetupDreamAppPipeline() {
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

	auto pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

	if (m_pDreamOS->GetHMD() != nullptr) {
		if (m_pDreamOS->GetHMD()->GetDeviceType() == HMDDeviceType::META) {
			pEnvironmentNode->SetIsAugmented(true);
		}
	}

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	/*
	ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	//*/

	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Connect output as pass-thru to internal blend program
	//CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	//*/
	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

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

WebBrowserPoint DreamOSTestSuite::GetRelativeBrowserPointFromContact(point ptIntersectionContact) {
	WebBrowserPoint webPt;
	webPt.x = 0;
	webPt.y = 0;
/*
	ptIntersectionContact.w() = 1.0f;

	// First apply transforms to the ptIntersectionContact
	point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;

	//m_pTestSphereRelative->SetPosition(ptAdjustedContact);

	float width = GetWidth();
	float height = GetHeight();

	float posX = ptAdjustedContact.x();
	float posY = ptAdjustedContact.y();
	float posZ = ptAdjustedContact.z();

	// TODO: This is a bit of a hack, should be a better way (this won't account for the quad normal, only orientation
	// so it might get confused - technically this should never actually happen otherwise since we can force a dimension
	if (std::abs(posZ) > std::abs(posY)) {
		posY = posZ;
	}

	posX /= width / 2.0f;
	posY /= height / 2.0f;

	posX = (posX + 1.0f) / 2.0f;
	posY = (posY + 1.0f) / 2.0f;  // flip it

	// TODO: push into WebBrowserController
	webPt.x = posX * 1366;
	webPt.y = 768 - (posY * 768);

	//ptAdjustedContact.Print("adj");
	//DEBUG_LINEOUT("%d %d", webPt.x, webPt.y);
//*/
	return webPt;
}

// InteractionObjectEvent
// Note that all of this will only occur if we're in testing mode
//*
RESULT DreamOSTestSuite::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	/*
#ifdef _USE_TEST_APP
	bool fUpdateMouse = false;

	//m_pPointerCursor->SetPosition(pEvent->m_ptContact[0]);

	if (pEvent->m_pObject == m_pTestQuad.get() || m_fTestQuadActive) {
		return HandleTestQuadInteractionEvents(pEvent);
	}
#endif

	switch (pEvent->m_eventType) {
#ifdef _USE_TEST_APP
		case ELEMENT_INTERSECT_BEGAN: {
			//if (m_pBrowserQuad->IsVisible()) {
				m_pPointerCursor->SetVisible(true);

				WebBrowserMouseEvent webBrowserMouseEvent;

				webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

				CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

				m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
				m_fBrowserActive = true;

				fUpdateMouse = true;
			//}
		} break;

		case ELEMENT_INTERSECT_ENDED: {
			m_pPointerCursor->SetVisible(false);

			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, true));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
			m_fBrowserActive = false;

			fUpdateMouse = true;
		} break;

		case ELEMENT_INTERSECT_MOVED: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;

			fUpdateMouse = true;
		} break;
#endif

		case INTERACTION_EVENT_SELECT_UP: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			bool fMouseUp = (pEvent->m_eventType == INTERACTION_EVENT_SELECT_UP);

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			webBrowserMouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

			CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;

			//// Determine focused node
			//CR(m_pWebBrowserController->GetFocusedNode());

		} break;

		case INTERACTION_EVENT_SELECT_DOWN: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			bool fMouseUp = (pEvent->m_eventType == INTERACTION_EVENT_SELECT_UP);

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			webBrowserMouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

			CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;

		case INTERACTION_EVENT_WHEEL: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;

			int deltaX = 0;
			int deltaY = pEvent->m_value * m_scrollFactor;

			CR(m_pWebBrowserController->SendMouseWheel(webBrowserMouseEvent, deltaX, deltaY));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;

		// Keyboard
		// TODO: Should be a "typing manager" in between?
		// TODO: haven't seen any issues with KEY_UP being a no-op
		case INTERACTION_EVENT_KEY_UP: break;
		case INTERACTION_EVENT_KEY_DOWN: {

#ifdef _USE_TEST_APP
			if ((pEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) && (pEvent->m_value == SVK_RETURN)) {
				if (m_fReceivingStream) {
					CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
					m_fReceivingStream = false;
				}

				SetStreamingState(false);

				// TODO: May not be needed, if not streaming no video is actually being transmitted
				// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
				//CR(GetDOS()->GetCloudController()->StartVideoStreaming(m_browserWidth, m_browserHeight, 30, PIXEL_FORMAT::BGRA));

				//CR(BroadcastDreamBrowserMessage(DreamShareViewMessage::type::PING));
				CR(BroadcastDreamBrowserMessage(DreamShareViewMessage::type::REQUEST_STREAMING_START));

				SetStreamingState(true);
			}
#endif

			/*
			bool fKeyDown = (pEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN);
			std::string strURL = "";

			char chKey = (char)(pEvent->m_value);
			m_strEntered.UpdateString(chKey);

			if (pEvent->m_value == SVK_RETURN) {
				SetVisible(true);

				std::string strScope = m_strScope;
				std::string strTitle = "website";
				std::string strPath = strURL;
				auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(m_pEnvironmentControllerProxy->RequestOpenAsset(m_strScope, strPath, strTitle), "Failed to share environment asset");
			}

			//CR(m_pWebBrowserController->SendKeyEventChar(chKey, fKeyDown));

		} break;
	}

#ifdef _USE_TEST_APP
	// First point of contact
	if (fUpdateMouse) {
		//if (pEvent->m_ptContact[0] != GetDOS()->GetInteractionEngineProxy()->GetInteractionRayOrigin()) {
			//m_pPointerCursor->SetOrigin(pEvent->m_ptContact[0]);
			point ptIntersectionContact = pEvent->m_ptContact[0];
			ptIntersectionContact.w() = 1.0f;

			point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;
			m_pPointerCursor->SetOrigin(ptAdjustedContact);
		//}
	}
#endif
//*/

	CR(r);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestMeta() {
	RESULT r = R_PASS;

	struct TestContext : public Subscriber<SenseControllerEvent> {
		std::shared_ptr<DreamUserApp> m_pUserApp = nullptr;

		virtual RESULT Notify(SenseControllerEvent *event) override {
			RESULT r = R_PASS;
			if (event->type == SENSE_CONTROLLER_META_CLOSED) {
				//hardcoded values taken from DreamUIBar
				if (m_pUserApp != nullptr) {
					auto pLeftMallet = m_pUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
					auto pRightMallet = m_pUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
					auto pLeftHand = m_pUserApp->GetHand(HAND_TYPE::HAND_LEFT);
					auto pRightHand = m_pUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
					if (event->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT && pLeftHand != nullptr) {
						if (event->state.fClosed == false) {
						//	pLeftMallet->GetH
						//	pLeftMallet->GetMalletHead()->SetScale(1.5f);
							//pLeftHand->SetScale(1.5f);
							pLeftHand->GetModel(HAND_TYPE::HAND_LEFT)->SetScale(0.02f);
						}
						else {
							//pLeftHand->SetScale(1.0f);
							pLeftHand->GetModel(HAND_TYPE::HAND_LEFT)->SetScale(0.03f);
							//pLeftMallet->GetMalletHead()->SetScale(1.0f);
						}
					//	pLeftMallet->
					}
					if (event->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT && pRightHand != nullptr) {
						if (event->state.fClosed == false) {
						//	pLeftMallet->GetH
						//	pLeftMallet->GetMalletHead()->SetScale(1.5f);
							pRightHand->SetScale(1.0f);
						}
						else {
							pRightHand->SetScale(1.5f);
							//pLeftMallet->GetMalletHead()->SetScale(1.0f);
						}

					}
				}
			}
		//Error:
			return r;
		}
	};
	TestContext *pTestContext = new TestContext();

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->m_pUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		//m_pDreamOS->RegisterEventSubscriber()
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_META_CLOSED, pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
		pModel->SetScale(0.02f);
	
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

		//pTestContext->m_pUserApp->Update();
		//pTestContext->m_pUserApp->SetHasOpenApp(true);

//		pTestContext->m_pUserApp->GetMallet(HAND_TYPE::HAND_LEFT)->Show();
//		pTestContext->m_pUserApp->GetMallet(HAND_TYPE::HAND_RIGHT)->Show();

		//pTestContext->m_pUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);


Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamUIBar() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());
		{
			auto pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this);
			//CN(pDreamUIBar);	// still fails because it needs a user
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamBrowser() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	struct TestContext {
		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;
		std::shared_ptr<Dream2DMouseApp> m_pDream2DMouse = nullptr;
		quad *m_pBrowserQuad = nullptr;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		std::string strURL = "https://www.youtube.com/watch?v=YqzHvcwJmQY?autoplay=1";
		
		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		//std::string strURL = "http://ncu.rcnpv.com.tw/Uploads/20131231103232738561744.pdf";

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Create the 2D Mouse App
		pTestContext->m_pDream2DMouse = m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		CNM(pTestContext->m_pDream2DMouse, "Failed to create dream 2D mouse app");


		pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		CN(pTestContext->m_pWebBrowserManager);
		CR(pTestContext->m_pWebBrowserManager->Initialize());

		// Create the Shared View App
		pTestContext->m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		pTestContext->m_pDreamBrowser->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
		CNM(pTestContext->m_pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		pTestContext->m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pTestContext->m_pDreamBrowser->SetDiagonalSize(10.0f);

		pTestContext->m_pDreamBrowser->SetURI(strURL);

		pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(3.0f, 3.0f);
		CN(pTestContext->m_pBrowserQuad);
		pTestContext->m_pBrowserQuad->RotateXByDeg(90.0f);
		pTestContext->m_pBrowserQuad->RotateZByDeg(180.0f);

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

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->m_pBrowserQuad->SetDiffuseTexture(pTestContext->m_pDreamBrowser->GetSourceTexture().get());

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestUIKeyboard() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext : public Subscriber<SenseControllerEvent> {
		std::shared_ptr<UIKeyboard> pKeyboard = nullptr;

		virtual RESULT Notify(SenseControllerEvent *event) override {
			RESULT r = R_PASS;
			if (event->type == SENSE_CONTROLLER_MENU_UP) {
				//hardcoded values taken from DreamUIBar
				CR(pKeyboard->UpdateComposite(-0.30f));
			}
		Error:
			return r;
		}
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(Initialize());
		CR(SetupDreamAppPipeline());

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		pTestContext->pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);
		pTestContext->pKeyboard->ShowKeyboard();
		CR(m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_MENU_UP, pTestContext));

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamLogger() {
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

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		DOSLOG(INFO, "AddTestDreamLogger DOSLOG (EHM) Test");
		DreamLogger::instance()->Log(DreamLogger::Level::INFO, "AddTestDreamLogger DreamLogger instance Test");
		m_pDreamOS->Log(DreamLogger::Level::INFO, "AddTestDreamLogger log test via m_pDOS");

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

// 		const int kMaxCallers = 62;
// 		void* callers[kMaxCallers];
// 		int count = ::CaptureStackBackTrace(0, kMaxCallers, callers, nullptr);
// 
// 		for (int i = 0; i < count; i++)
// 			printf("*** %d called from %016I64X\n", i, (unsigned __int64)callers[

		// We want to force a "crash here" so we can get the track in the log
		CBM(0, "Crashing logging test to see what happens");

	Error:
		return r;
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

	pUITest->SetTestName("Logging Test");
	pUITest->SetTestDescription("Basic logging test which will spin up a few SPD logs and test out the system");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

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

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

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

RESULT DreamOSTestSuite::AddTestCaptureApp() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	struct TestContext {

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		//CN(m_pDreamOS);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);

		CR(SetupPipeline());

		{
			std::vector<UID> pTestUIDs = m_pDreamOS->GetAppUID("UIKeyboard");
			CB(pTestUIDs.size() == 1);
			UID keyboardUID = pTestUIDs[0];

			auto pTestApp1 = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this).get();
			auto pTestHandle = m_pDreamOS->CaptureApp(keyboardUID, pTestApp1);
			CB(pTestHandle != nullptr);

			auto pTestApp2 = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this).get();
			auto pTestHandleFail = m_pDreamOS->CaptureApp(keyboardUID, pTestApp2);
			CB(pTestHandleFail == nullptr);

			auto pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(pTestHandle);

			CR(pKeyboardHandle->Show());

			CR(m_pDreamOS->ReleaseApp(pKeyboardHandle, keyboardUID, pTestApp1));

			CB(pKeyboardHandle->Show() == R_FAIL);

			pTestHandle = m_pDreamOS->CaptureApp(keyboardUID, pTestApp1);
			CB(pTestHandle != nullptr);
			CR(m_pDreamOS->ReleaseApp(pTestHandle, keyboardUID, pTestApp1));
		}

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

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

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

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

// A test that includes all the basic UI apps in a functional state.
// User, ControlView, Keyboard, Browser, UIBar
RESULT DreamOSTestSuite::AddTestDreamOS() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext {
		std::shared_ptr<DreamUserApp> pUser = nullptr;
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamControlView> pDreamControlView = nullptr;
		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<DreamUIBar> pDreamUIBar = nullptr;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());
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
				long environmentID = 168;

				CRM(pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");
				CRM(pCloudController->Start(false), "Failed to Start Cloud Controller");

			}
		}
		pDreamControlView = m_pDreamOS->LaunchDreamApp<DreamControlView>(this, false);
		CN(pDreamControlView);

		// UIKeyboard App
		CR(m_pDreamOS->InitializeKeyboard());
		pTestContext->pUser = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		CN(pTestContext->pUser);

		CR(pTestContext->pUser->SetHand(m_pDreamOS->GetHand(HAND_TYPE::HAND_LEFT)));
		CR(pTestContext->pUser->SetHand(m_pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT)));

		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(9.0f);
		pDreamBrowser->SetPosition(point(0.0f, 2.0f, -2.0f));

		pDreamBrowser->SetVisible(false);

		pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBar>(this, false);
		CN(pDreamUIBar);
		CR(pDreamUIBar->SetUIStageProgram(m_pUIProgramNode));

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local UIView Test");
	pUITest->SetTestDescription("Full test of uiview working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestDreamShareView() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

	struct TestTimingContext {
		double m_msStart;
		std::shared_ptr<DreamShareView> pDreamShareView;
	};

	TestTimingContext *pTestContext = new TestTimingContext();
	pTestContext->m_msStart = msNow;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());
		{
			std::shared_ptr<DreamShareView> pDreamShareView = nullptr;

			auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);
			pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this);
			pDreamShareView->Show();

			auto pCastTexture = m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			pDreamShareView->SetCastingTexture(std::shared_ptr<texture>(pCastTexture));

			pTestContext->pDreamShareView = pDreamShareView;
		}

	Error:
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
		auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);

		//auto pDreamShareViewHandle = dynamic_cast<DreamShareViewHandle*>(m_pDreamOS->RequestCaptureAppUnique("DreamShareView", this));

		double diff = msNow - pTestContext->m_msStart;
		int mod = ((int)diff / 500) % 2;
		if (mod == 0) {
			pTestContext->pDreamShareView->ShowLoadingTexture();
		}
		else {
			pTestContext->pDreamShareView->ShowCastingTexture();
		}

		//m_pDreamOS->RequestReleaseAppUnique(pDreamShareViewHandle, this);
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DreamOSTestSuite::AddTestBasicBrowserCast() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

	struct TestTimingContext {
		double m_msStart;
		std::shared_ptr<DreamShareView> pDreamShareView;
	};

	TestTimingContext *pTestContext = new TestTimingContext();
	pTestContext->m_msStart = msNow;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<Dream2DMouseApp> pDream2DMouse = nullptr;

		std::string strURL = "http://www.youtube.com";

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		//std::string strURL = "http://ncu.rcnpv.com.tw/Uploads/20131231103232738561744.pdf";

		CN(m_pDreamOS);

		CR(SetupDreamAppPipeline());
		{
			std::shared_ptr<DreamShareView> pDreamShareView = nullptr;

			auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);
			pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this);
			pDreamShareView->Show();

			auto pCastTexture = m_pDreamOS->MakeTexture(L"website.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			pDreamShareView->SetCastingTexture(std::shared_ptr<texture>(pCastTexture));

			pTestContext->pDreamShareView = pDreamShareView;

			// Create the Shared View App
			pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
			CNM(pDreamBrowser, "Failed to create dream browser");

			// Set up the view
			//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
			pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
			pDreamBrowser->SetDiagonalSize(10.0f);
			pDreamBrowser->SetPosition(point(4.0f, 0.0f, 0.0f));
		}

		pDreamBrowser->SetURI(strURL);

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

		auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);

		//auto pDreamShareViewHandle = dynamic_cast<DreamShareViewHandle*>(m_pDreamOS->RequestCaptureAppUnique("DreamShareView", this));

		double diff = msNow - pTestContext->m_msStart;
		int mod = ((int)diff / 500) % 2;
		/*
		if (mod == 0) {
			pTestContext->pDreamShareView->ShowLoadingTexture();
		}
		else {
			pTestContext->pDreamShareView->ShowCastingTexture();
		}
		//*/

		//m_pDreamOS->RequestReleaseAppUnique(pDreamShareViewHandle, this);
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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT DreamOSTestSuite::AddTestDreamDesktop() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext {
		std::shared_ptr<DreamDesktopApp> pDreamDesktop = nullptr;
		std::shared_ptr<DreamUserControlArea> pDreamUserControlArea = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;
		texture* pTexture = nullptr;
		bool once = false;
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		SetupDreamAppPipeline();
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		{
			//std::shared_ptr<EnvironmentAsset> pEnvAsset = nullptr;
			pTestContext->pDreamDesktop = m_pDreamOS->LaunchDreamApp<DreamDesktopApp>(this);
			CNM(pTestContext->pDreamDesktop, "Failed to create dream desktop");

			//pTestContext->pDreamUserControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this);
			//pTestContext->pDreamUserControlArea->AddEnvironmentAsset(pEnvAsset);
			//pTestContext->pDreamUserControlArea->SetActiveSource(pTestContext->pDreamDesktop);	
			//pTestContext->pDreamUserControlArea->GetComposite()->SetPosition(m_pDreamOS->GetCameraPosition() + point(0.0f, 1.5f, -.3f));
			
			
			auto pComposite = m_pDreamOS->AddComposite();
			pComposite->InitializeOBB();

			auto pView = pComposite->AddUIView(m_pDreamOS);
			pView->InitializeOBB();

			pTestContext->pQuad = pView->AddQuad(.938f * 4.0, .484f * 4.0, 1, 1, nullptr, vector::kVector());
			pTestContext->pQuad->SetPosition(0.0f, 0.0f, 0.0f);
			pTestContext->pQuad->FlipUVVertical();

			int pxWidth = 1920;
			int pxHeight = 1080;

			m_pDataBuffer_n = pxHeight*pxWidth*4;
			m_pDataBuffer = (unsigned char*)malloc(m_pDataBuffer_n);

			pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, PIXEL_FORMAT::BGRA, 4, m_pDataBuffer, (int)m_pDataBuffer_n);

			m_pDataBuffer_n = 0;
			pTestContext->pQuad->SetDiffuseTexture(pTestContext->pTexture);

			/*
			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			HWND desktopHWND = FindWindow(NULL, L"DreamDesktopDuplication");
			if (desktopHWND == NULL) {
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				PathManager* pPathManager = PathManager::instance();
				std::wstring wstrDreamPath;
				pPathManager->GetDreamPath(wstrDreamPath);

				std::wstring wstrPathfromDreamPath = L"\\Project\\Windows\\DreamOS\\x64\\Testing\\DreamDesktopCapture.exe";
				std::wstring wstrFullpath = wstrDreamPath + wstrPathfromDreamPath;
				const wchar_t *wPath = wstrFullpath.c_str();
				std::vector<wchar_t> vwszLocation(wstrFullpath.begin(), wstrFullpath.end());
				vwszLocation.push_back(0);
				LPWSTR strLPWlocation = vwszLocation.data();

				if (!CreateProcess(strLPWlocation,
					L" /output 1",	// Command line
					nullptr,           // Process handle not inheritable
					nullptr,           // Thread handle not inheritable
					false,          // Set handle inheritance to FALSE
					0,              // No creation flags
					nullptr,           // Use parent's environment block
					nullptr,           // Use parent's starting directory
					&si,            // Pointer to STARTUPINFO structure
					&pi)            // Pointer to PROCESS_INFORMATION structure
					)
				{
					DEBUG_LINEOUT("CreateProcess failed (%d). \n", GetLastError());
					r = R_FAIL;
				}

				while (desktopHWND == NULL) {
					desktopHWND = FindWindow(NULL, L"DreamDesktopDuplication");
				}
			}

			DWORD desktopPID;
			GetWindowThreadProcessId(desktopHWND, &desktopPID);

			HWND dreamHWND = FindWindow(NULL, L"Dream Testing");
			if (dreamHWND == NULL) {
				MessageBox(dreamHWND, L"Unable to find the Dream window",
					L"Error", MB_ICONERROR);
				return r;
			}

			DDCIPCMessage ddcMessage;
			ddcMessage.m_msgType = DDCIPCMessage::type::START;
			COPYDATASTRUCT desktopCDS;

			desktopCDS.dwData = (unsigned long)ddcMessage.m_msgType;
			desktopCDS.cbData = sizeof(ddcMessage);
			desktopCDS.lpData = &ddcMessage;

			SendMessage(desktopHWND, WM_COPYDATA, (WPARAM)(HWND)dreamHWND, (LPARAM)(LPVOID)&desktopCDS);
			DWORD dwError = GetLastError();
			if (dwError != NO_ERROR) {
				MessageBox(dreamHWND, L"error sending message", L"error", MB_ICONERROR);
			}
			else {
				DEBUG_LINEOUT("Message sent");
			}
			//*/

			// Wait until child process exits.
			//WaitForSingleObject(pi.hProcess, INFINITE);

			// Close process and thread handles.
			//CloseHandle(pi.hProcess);
			//CloseHandle(pi.hThread);
		}

	Error:
		return r;
	};	// Test Code (this evaluates the test upon completion)

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CBR(m_pDataBuffer_n != 0, R_SKIPPED);
		CN(pTestContext);
		if (!pTestContext->once) {
			//CR(pTestContext->pDreamDesktop->OnDesktopFrame((int)m_pDataBuffer_n, m_pDataBuffer, m_pxHeight, m_pxWidth));
			pTestContext->pTexture->Update(m_pDataBuffer, m_pxWidth, m_pxHeight, PIXEL_FORMAT::BGRA);
			// pTestContext->once = true;
			if (m_pDataBuffer) {
				free(m_pDataBuffer);
				m_pDataBuffer = nullptr;
				m_pDataBuffer_n = 0;
			}

			//pTestContext->once = true;
		}

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

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);
	pUITest->SetTestName("Local Dream Desktop Test");
	pUITest->SetTestDescription("Dream Desktop working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

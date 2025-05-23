#include "DOSTestSuite.h"

#include <chrono>
#include "memory"                              // for shared_ptr
#include "vcruntime_new.h"                     // for operator new, operator delete
#include "vcruntime_string.h"                  // for memcpy
#include <stddef.h>                            // for size_t
#include <WinUser.h>                           // for SendMessage

#include "os/DreamOS.h"

#include "pipeline/ProgramNode.h"
#include "pipeline/Pipeline.h"             // for Pipeline
#include "pipeline/SinkNode.h"
#include "pipeline/SourceNode.h"

#include "sandbox/CommandLineManager.h"        // for CommandLineManager
#include "sandbox/CredentialManager.h"         // for CredentialManager, CredentialManager::type, CredentialManager::type::CREDENTIAL_GENERIC

#include "test/TestObject.h"                   // for TestObject::TestDescriptor, TestObject

#include "hal/UIStageProgram.h"

// TODO: NO OGL at this level
#include "hal/ogl/OGLProgramReflection.h"
#include "hal/ogl/OGLProgramRefraction.h"
#include "hal/ogl/OGLProgramSkybox.h"
#include "hal/ogl/OGLProgramWater.h"
#include "HAL/ogl/OGLProgramSkyboxScatter.h"
#include "HAL/ogl/OGLProgramScreenFade.h"
#include "hal/ogl/OGLProgram.h"             // for OGLProgram
#include "hal/EnvironmentProgram.h"  // for EnvironmentProgram
#include "hal/HALImp.h"                        // for HALImp
#include "HAL/SkyboxScatterProgram.h"

#include "core/dimension/DimObj.h"                 // for DimObj

#include "core/model/model.h"            // for model
#include "core/hand/hand.h"

#include "core/primitives/quad.h"                   // for quad
#include "core/primitives/quaternion.h"             // for quaternion
#include "core/primitives/ray.h"                    // for ray
#include "core/primitives/sphere.h"                 // for sphere
#include "core/primitives/texture.h"                // for texture, texture::type, texture::type::TEXTURE_2D
#include "core/primitives/vector.h"                 // for vector
#include "core/primitives/volume.h"                 // for volume

#include "scene/ObjectStoreNode.h"
#include "scene/CameraNode.h"

#include "apps/DreamBrowserApp/DreamBrowserApp.h"
#include "apps/DreamEnvironmentApp/DreamEnvironmentApp.h"
#include "apps/DreamUIBarApp/DreamUIBarApp.h"
#include "apps/DreamTestingApp/DreamTestingApp.h"
#include "apps/DreamDesktopDuplicationApp/DreamDesktopApp.h"
#include "apps/DreamShareViewApp/DreamShareViewApp.h"

#include "modules/DreamSoundSystem/DreamSoundSystem.h"      // for DreamSoundSystem, DreamSoundSystem::MIXDOWN_TARGET, DreamSoundSystem::observer

#include "webbrowser/WebBrowserController.h"
#include "webbrowser/CEFBrowser/CEFBrowserManager.h"

#include "sound/AudioPacket.h"

#include "cloud/CloudController.h"             // for CloudController

#include "sense/SenseController.h"             // for SenseControllerEvent, ControllerState, SenseControllerEventType::SENSE_CONTROLLER_MENU_UP, SenseControllerEventType::SENSE_CONTROLLER_META_CLOSED

class CEFBrowserManager;
class DOMNode;
class DimRay;
class Dream2DMouseApp;
class DreamContentSource;
class DreamDesktopApp;
class DreamEnvironmentApp;
class DreamPeerApp;
class DreamShareView;
class DreamTestingApp;
class DreamUserApp;
class DreamUserControlAreaApp;
class DreamVCamApp;
class MenuControllerProxy;
class NamedPipeClient;
class NamedPipeServer;
class SinkNode;
class SoundBuffer;
class UIKeyboard;
class light;
class user;
struct InteractionObjectEvent;

DOSTestSuite::DOSTestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("dreamos", pDreamOS)
{
	// 
}

RESULT DOSTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestDreamObjectModule());
	
	CR(AddTestDreamSoundSystem());

	CR(AddTestDreamBrowser());

	CR(AddTestDreamVCam());
	
	CR(AddTestDreamDesktop());

	CR(AddTestNamedPipes());

	CR(AddTestModuleManager());
	
	CR(AddTestGamepadCamera());

	CR(AddTestDreamLogger());

	CR(AddTestEnvironmentSeating());

	CR(AddTestEnvironmentSwitching());
	
	CR(AddTestDreamOS());

	CR(AddTestDreamUIBar());

	CR(AddTestMeta());	

	// Casting tests

	CR(AddTestBasicBrowserCast());

	CR(AddTestDreamShareView());

	CR(AddTestUserApp());

	CR(AddTestDreamApps());

	CR(AddTestUIKeyboard());

	CR(AddTestCaptureApp());

Error:
	return r;
}

RESULT DOSTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	CNM(m_pDreamOS, "DreamOS handle is not set");

Error:
	return r;
}

RESULT DOSTestSuite::SetupPipeline(std::string strRenderProgramName) {
	RESULT r = R_PASS;

	ProgramNode* pRenderProgramNode = nullptr;
	ProgramNode* pReferenceGeometryProgram = nullptr;
	ProgramNode* pSkyboxProgram = nullptr;
	ProgramNode* pUIProgramNode = nullptr;

	ProgramNode *pRenderScreenQuad = nullptr;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	pRenderProgramNode = pHAL->MakeProgramNode(strRenderProgramName);
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	//TODO: Matrix node
	//	CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

	// Connect output as pass-thru to internal blend program
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

	pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());
	{
		light *pLight = m_pDreamOS->AddLight(LIGHT_SPOT, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
	}

Error:
	return r;
}

/*
RESULT DreamOSTestSuite::SetupDreamAppPipeline() {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();
	HMD* pHMD = m_pDreamOS->GetHMD();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	//CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode;
	pRenderProgramNode = pHAL->MakeProgramNode("standard");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	EnvironmentProgram* pEnvironmentNode;
	pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

	if (pHMD != nullptr) {
		pEnvironmentNode->SetIsAugmented(pHMD->IsARHMD());
	}

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram;
	pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	//ProgramNode* pSkyboxProgram;
	//pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	//CN(pSkyboxProgram);
	//CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	//CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	//CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	ProgramNode* pUIProgramNode;
	pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Connect output as pass-thru to internal blend program
	//CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
	m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

	// Screen Quad Shader (opt - we could replace this if we need to)
	ProgramNode *pRenderScreenQuad;
	pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);

	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	// Connect Program to Display
	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

Error:
	return r;
}
*/

WebBrowserPoint DOSTestSuite::GetRelativeBrowserPointFromContact(point ptIntersectionContact) {
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
RESULT DOSTestSuite::Notify(InteractionObjectEvent *pEvent) {
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


// TODO: Do we need this anymore? 
// TODO: Should we create a VR/AR HMD test suite
RESULT DOSTestSuite::AddTestMeta() {
	RESULT r = R_PASS;

	struct TestContext : public Subscriber<SenseControllerEvent> {
		std::shared_ptr<DreamUserApp> m_pUserApp = nullptr;

		virtual RESULT Notify(SenseControllerEvent *event) override {
			RESULT r = R_PASS;
			if (event->type == SENSE_CONTROLLER_META_CLOSED) {
				//hardcoded values taken from DreamUIBar
				if (m_pUserApp != nullptr) {
					auto pLeftHand = m_pUserApp->GetHand(HAND_TYPE::HAND_LEFT);
					auto pRightHand = m_pUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
					if (event->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT && pLeftHand != nullptr) {
						if (event->state.fClosed == false) {
							pLeftHand->SetScale(1.5f);
						}
						else {
							pLeftHand->SetScale(1.0f);
						}
					}
					if (event->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT && pRightHand != nullptr) {
						if (event->state.fClosed == false) {
							pRightHand->SetScale(1.0f);
						}
						else {
							pRightHand->SetScale(1.5f);
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

		//CR(SetupDreamAppPipeline());
		CR(SetupPipeline());

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->m_pUserApp = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		//m_pDreamOS->RegisterEventSubscriber()
		m_pDreamOS->RegisterSubscriber(SENSE_CONTROLLER_META_CLOSED, pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		model *pModel;
		pModel = m_pDreamOS->AddModel(L"\\face4\\untitled.obj");
		CN(pModel);

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

	auto pUITest = AddTest("metahmd", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Set up of the meta HMD");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);


Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamUIBar() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		//CR(SetupDreamAppPipeline());
		CR(SetupPipeline());


		{
			auto pDreamUIBar = m_pDreamOS->LaunchDreamApp<DreamUIBarApp>(this);
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

	auto pUITest = AddTest("dreamuibar", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Testing the DreamUIBar app");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamBrowser() {
	RESULT r = R_PASS;

	TestObject::TestDescriptor testDescriptor;

	testDescriptor.strTestName = "dreambrowser";
	testDescriptor.strTestDescription = "Testing of Dream Browser App";
	testDescriptor.sDuration = 6000.0f;
	testDescriptor.nRepeats = 1;

	struct TestContext {
		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		std::shared_ptr<DreamBrowserApp> m_pDreamBrowserApp = nullptr;
		std::shared_ptr<Dream2DMouseApp> m_pDream2DMouse = nullptr;
		quad *m_pBrowserQuad = nullptr;
	};
	testDescriptor.pContext = (void*)(new TestContext());

	testDescriptor.fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		//std::string strURL = "https://www.youtube.com/watch?v=YqzHvcwJmQY?autoplay=1";
		//std::string strURL = "https://twitch.tv";
		std::string strURL = "https://www.youtube.com/watch?v=JzqumbhfxRo&t=27s";
		//std::string strURL = "https://www.youtube.com/watch?v=B9mEIZ3qMTw";

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		//std::string strURL = "http://ncu.rcnpv.com.tw/Uploads/20131231103232738561744.pdf";

		CN(m_pDreamOS);

		//CR(SetupDreamAppPipeline());
		CR(SetupPipeline());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		// Create the 2D Mouse App
		//pTestContext->m_pDream2DMouse = m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		//CNM(pTestContext->m_pDream2DMouse, "Failed to create dream 2D mouse app");

		pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		CN(pTestContext->m_pWebBrowserManager);
		CR(pTestContext->m_pWebBrowserManager->Initialize());
			
		// This presents a timing issue if it works 
		pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(4.8f, 2.7f);
		CN(pTestContext->m_pBrowserQuad);
		pTestContext->m_pBrowserQuad->FlipUVHorizontal();
		pTestContext->m_pBrowserQuad->RotateXByDeg(90.0f);
		pTestContext->m_pBrowserQuad->RotateZByDeg(180.0f);
	
		// Create the Shared View App
		pTestContext->m_pDreamBrowserApp = m_pDreamOS->LaunchDreamApp<DreamBrowserApp>(this);
		pTestContext->m_pDreamBrowserApp->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
		CNM(pTestContext->m_pDreamBrowserApp, "Failed to create dream browser");
		
		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		//pTestContext->m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		//pTestContext->m_pDreamBrowser->SetDiagonalSize(10.0f);
		
		pTestContext->m_pDreamBrowserApp->SetURI(strURL);

	Error:
		return R_PASS;
	};

	// Update Code
	testDescriptor.fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->m_pBrowserQuad->SetDiffuseTexture(pTestContext->m_pDreamBrowserApp->GetSourceTexture());

	Error:
		return r;
	};

	// Reset Code
	testDescriptor.fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(testDescriptor);
	CN(pUITest);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestCaptureApp() {
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
			// TODO: re-enable if handles are used again
			//UID keyboardUID;

			auto pTestApp1 = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this).get();
			//auto pTestHandle = m_pDreamOS->CaptureApp(keyboardUID, pTestApp1);
			//CB(pTestHandle != nullptr);

			auto pTestApp2 = m_pDreamOS->LaunchDreamApp<DreamTestingApp>(this).get();
			//auto pTestHandleFail = m_pDreamOS->CaptureApp(keyboardUID, pTestApp2);
			//CB(pTestHandleFail == nullptr);

			//pTestHandle = m_pDreamOS->CaptureApp(keyboardUID, pTestApp1);
			//CB(pTestHandle != nullptr);
			//CR(m_pDreamOS->ReleaseApp(pTestHandle, keyboardUID, pTestApp1));
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

	auto pUITest = AddTest("captureapp", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Test capture app functionality");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestUIKeyboard() {
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

		//CR(SetupDreamAppPipeline());
		CR(SetupPipeline());

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		pTestContext->pKeyboard = m_pDreamOS->LaunchDreamApp<UIKeyboard>(this);
		pTestContext->pKeyboard->Show();
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

	auto pUITest = AddTest("uikeyboard", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Test the UI keyboard");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamLogger() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;
	//const int numTests = 5;

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamTestingApp> pDreamTestApps[5];// = { nullptr };

		CN(m_pDreamOS);

		CR(SetupPipeline("blinnphong"));

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		sphere *pSphere;
		pSphere = m_pDreamOS->AddSphere(1.0f, 20, 20);

		DOSLOG(INFO, "AddTestDreamLogger DOSLOG (EHM) Test with %s string", "TESTING STRING");

		DreamLogger::instance()->Log(DreamLogger::Level::INFO, "AddTestDreamLogger DreamLogger instance Test: %s", "TESTING STRING");

		m_pDreamOS->Log(DreamLogger::Level::INFO, "AddTestDreamLogger log test via m_pDOS: %s", "TESTING STRING");

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
		CBM(0, "Crashing logging test to see what happens: %s", "TESTING STRING");

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

	auto pUITest = AddTest("logger", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Basic logging test which will spin up a few SPD logs and test out the system");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestNamedPipes() {
	RESULT r = R_PASS;

	double sTestTime = 5000.0f;
	int nRepeats = 1;
	//const int numTests = 5;

	struct TestContext {
		texture *pTexture = nullptr;
		unsigned char *pBuffer = nullptr;

		std::shared_ptr<NamedPipeServer> pNamedPipeServer = nullptr;
		std::shared_ptr<NamedPipeClient> pNamedPipeClient1 = nullptr;
		std::shared_ptr<NamedPipeClient> pNamedPipeClient2 = nullptr;

		RESULT HandleClientPipeMessage(void *pBuffer, size_t pBuffer_n) {
			RESULT r = R_PASS;

			char *pszMessage = (char *)(pBuffer);
			CN(pszMessage);

			DEBUG_LINEOUT("HandleClientPipeMessage: %s", pszMessage);

		Error:
			return r;
		}

		RESULT HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n) {
			RESULT r = R_PASS;

			char *pszMessage = (char *)(pBuffer);
			CN(pszMessage);

			DEBUG_LINEOUT("HandleServerPipeMessage: %s", pszMessage);

		Error:
			return r;
		}

	} *pTestContext = new TestContext();

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		int msBuffer = 50;

		CN(m_pDreamOS);

		CR(SetupPipeline());

		{
			auto pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
			CN(pLight);

			sphere *pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
			CN(pSphere);

			pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
			CN(pTestContext->pTexture);

			// Set up named pipe server
			pTestContext->pNamedPipeServer = m_pDreamOS->MakeNamedPipeServer(L"dreamvcampipe");
			CN(pTestContext->pNamedPipeServer);
			CR(pTestContext->pNamedPipeServer->RegisterMessageHandler(std::bind(&TestContext::HandleServerPipeMessage, pTestContext, std::placeholders::_1, std::placeholders::_2)));
			CR(pTestContext->pNamedPipeServer->Start());

			///*
			// This form of IPC isn't designed to be same process
			// so we add a bit of delay to accommodate for problematic event handling / timing errors
			// that would normally be fixed with a mutex for cross-thread sync
			std::this_thread::sleep_for(std::chrono::milliseconds(msBuffer));

			// Set up named pipe clients
			/*
			pTestContext->pNamedPipeClient1 = m_pDreamOS->MakeNamedPipeClient(L"dreamvcampipe");
			CN(pTestContext->pNamedPipeClient1);
			CR(pTestContext->pNamedPipeClient1->RegisterMessageHandler(std::bind(&TestContext::HandleClientPipeMessage, pTestContext, std::placeholders::_1, std::placeholders::_2)));
			CR(pTestContext->pNamedPipeClient1->Start());
			
			std::this_thread::sleep_for(std::chrono::milliseconds(msBuffer));

			pTestContext->pNamedPipeClient2 = m_pDreamOS->MakeNamedPipeClient(L"dreamvcampipe");
			CN(pTestContext->pNamedPipeClient2);
			CR(pTestContext->pNamedPipeClient2->RegisterMessageHandler(std::bind(&TestContext::HandleClientPipeMessage, pTestContext, std::placeholders::_1, std::placeholders::_2)));
			CR(pTestContext->pNamedPipeClient2->Start());

			// just to be careful
			std::this_thread::sleep_for(std::chrono::milliseconds(msBuffer));
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

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		static int count = 0;

		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
		static std::chrono::system_clock::time_point lastUpdateTimeResetClient = std::chrono::system_clock::now();

		{
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

			// This sort of emulates 24 FPS or so
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 1000) {
				std::string strTestMessage = "testing: " + std::to_string(count++);
				pTestContext->pNamedPipeServer->SendMessage((void*)(strTestMessage.c_str()), sizeof(char) * strTestMessage.size());
				lastUpdateTime = timeNow;
			}

			// Will disconnect client 2 after 5 seconds
			if (std::chrono::duration_cast<std::chrono::seconds>(timeNow - lastUpdateTimeResetClient).count() > 5) {
				if (pTestContext->pNamedPipeClient2 != nullptr) {
					pTestContext->pNamedPipeClient2 = nullptr;
				}
				lastUpdateTimeResetClient = timeNow;
			}
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

		// TODO: Kill apps

	Error:
		return r;
	};

	auto pUITest = AddTest("namedpipes", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Test the named pipe server capabilities");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamObjectModule() {
	RESULT r = R_PASS;

	TestObject::TestDescriptor testDescriptor;
	testDescriptor.strTestName = "dreamobjectmodule";
	testDescriptor.strTestDescription = "Test object creation / destruction using the object module";
	testDescriptor.sDuration = 100.0f;
	
	float radius = 0.2f;
	int factor = 10;
	float paddingRatio = 0.10f;

	struct TestContext {
		model *pModel = nullptr;

		// COMMANDMENT: Thou Shall Not Have Member Templates in a Local Class 
		RESULT OnSphereReady(DimObj *pDimObj, void *pContext) {
			RESULT r = R_PASS;

			point ptOrigin;
			sphere *pObj = dynamic_cast<sphere*>(pDimObj);
			CN(pObj);

			CN(pDreamOS);

			if (pContext != nullptr) {
				memcpy(&ptOrigin, (point*)(pContext), sizeof(point));

				delete pContext;
				pContext = nullptr;
			}

			pObj->SetPosition(ptOrigin);

			CRM(pDreamOS->AddObject(pObj), "Failed to add async sphere");

		Error:
			return r;
		}

		RESULT OnModelReady(DimObj *pDimObj, void *pContext) {
			RESULT r = R_PASS;

			point ptOrigin;
			model *pObj = dynamic_cast<model*>(pDimObj);
			CN(pObj);

			CN(pDreamOS);

			if (pContext != nullptr) {
				memcpy(&ptOrigin, (point*)(pContext), sizeof(point));

				delete pContext;
				pContext = nullptr;
			}

			pObj->SetPosition(ptOrigin);
			pObj->SetScale(0.025f);
			pObj->RotateYByDeg(180.0f);
			pObj->SetVisible(false);

			pModel = pObj;

			CRM(pDreamOS->AddObject(pObj), "Failed to add async sphere");

		Error:
			return r;
		}

		RESULT OnVolumeReady(DimObj *pDimObj, void *pContext) {
			RESULT r = R_PASS;

			point ptOrigin;
			volume *pObj = dynamic_cast<volume*>(pDimObj);
			CN(pObj);

			CN(pDreamOS);

			if (pContext != nullptr) {
				memcpy(&ptOrigin, (point*)(pContext), sizeof(point));

				delete pContext;
				pContext = nullptr;
			}

			pObj->SetPosition(ptOrigin);

			CRM(pDreamOS->AddObject(pObj), "Failed to add async sphere");

		Error:
			return r;
		}

		RESULT OnQuadReady(DimObj *pDimObj, void *pContext) {
			RESULT r = R_PASS;

			point ptOrigin;
			quad *pObj = dynamic_cast<quad*>(pDimObj);
			CN(pObj);

			CN(pDreamOS);

			if (pContext != nullptr) {
				memcpy(&ptOrigin, (point*)(pContext), sizeof(point));

				delete pContext;
				pContext = nullptr;
			}

			pObj->SetPosition(ptOrigin);
			pObj->RotateXByDeg(90.0f);

			CRM(pDreamOS->AddObject(pObj), "Failed to add async sphere");

			// Kick off new texture load for this quad
			
			CR(pDreamOS->LoadTexture(std::bind(&TestContext::OnQuadTextureReady, this, std::placeholders::_1, std::placeholders::_2),
				(void*)(pObj), texture::type::TEXTURE_2D, L"cobblestone_color.png"));

		Error:
			return r;
		}

		RESULT OnQuadTextureReady(texture *pTexture, void *pContext) {
			RESULT r = R_PASS;

			quad *pObj = (quad*)(pContext);
			CN(pObj);

			CN(pTexture);

			CR(pObj->SetDiffuseTexture(pTexture));

		Error:
			return r;
		}

		DreamOS *pDreamOS = nullptr;

	};
	testDescriptor.pContext = new TestContext();

	testDescriptor.fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("environment"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		pTestContext->pDreamOS = m_pDreamOS;

		{
			light *pLight;
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));

			point *pPtOrigin = new point(2.0f, -2.5f, 0.0f);
			//CR(m_pDreamOS->MakeModel(std::bind(&TestContext::OnModelReady, pTestContext, std::placeholders::_1, std::placeholders::_2),
			//	(void*)(pPtOrigin), L"dreamos:\\Assets\\model\\avatar\\3\\head.fbx"));

			//CR(m_pDreamOS->MakeModel(std::bind(&TestContext::OnModelReady, pTestContext, std::placeholders::_1, std::placeholders::_2),
			//	(void*)(pPtOrigin), L"dreamos:\\Assets\\model\\environment\\2\\environment.fbx"));

			model *pModel = m_pDreamOS->AddModel(L"dreamos:\\Assets\\model\\environment\\3\\environment.fbx");
			CN(pModel);
			pModel->SetPosition(*pPtOrigin);
			pModel->SetScale(0.025f);
			pModel->RotateYByDeg(180.0f);

			// Test the creation of an arbitrarily large number of spheres
			for (int i = 0; i < factor; i++) {
				for (int j = 0; j < factor; j++) {
					for (int k = 0; k < factor; k++) {
						float xPos = ((float)i - ((float)(factor - 1) / 2.0f)) * (1.0 + paddingRatio) * (radius * 2.0f);
						float yPos = ((float)j - ((float)(factor - 1) / 2.0f)) * (1.0 + paddingRatio) * (radius * 2.0f);
						float zPos = ((float)k - ((float)(factor - 1) / 2.0f)) * (1.0 + paddingRatio) * (radius * 2.0f);

						pPtOrigin = new point(xPos, yPos, zPos);
						CN(pPtOrigin);

						/* 
						// sphere
						CR(m_pDreamOS->MakeSphere(std::bind(&TestContext::OnSphereReady, pTestContext, std::placeholders::_1, std::placeholders::_2), 
							(void*)(pPtOrigin), radius, 10, 10));
						*/

						/*
						// volume
						CR(m_pDreamOS->MakeVolume(std::bind(&TestContext::OnVolumeReady, pTestContext, std::placeholders::_1, std::placeholders::_2),
							(void*)(pPtOrigin), radius * 2.0f, radius * 2.0f, radius * 2.0f));
						//*/

						/*
						// quad
						CR(m_pDreamOS->MakeQuad(std::bind(&TestContext::OnQuadReady, pTestContext, std::placeholders::_1, std::placeholders::_2),
							(void*)(pPtOrigin), radius * 2.0f, radius * 2.0f));
						//*/
					}
				}
			}

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp);
			CR(pDreamGamepadApp->SetCamera(m_pDreamOS->GetCamera(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));

		}

	Error:
		return r;
	};

	// Update Code
	testDescriptor.fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			if (pTestContext->pModel != nullptr) {
				if (pTestContext->pModel->IsModelLoaded() == true) {
					pTestContext->pModel->SetVisible(true);
				}
			}
		}

	Error:
		return r;
	};

	auto pUITest = AddTest(testDescriptor);
	CN(pUITest);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamSoundSystem() {
	RESULT r = R_PASS;

	std::string strTestName = "dreamsoundsystem";
	std::string strTestDescription = "Testing playing a sound by way of the sound system module";

	double sTestTime = 20.0f;
	int nRepeats = 1;
	float radius = 2.0f;

	struct TestContext : 
		public DreamSoundSystem::observer,
		public DreamBrowserObserver
	{

		sphere *pSphere = nullptr;

		RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
			RESULT r = R_PASS;

			//// Simply pushes the capture buffer to the render buffer
			//if (pSoundClient != nullptr) {
			//	CR(pSoundClient->PushMonoAudioBufferToRenderBuffer(numFrames, pCaptureBuffer));
			//}

			CR(r);

		Error:
			return r;
		}

		// DreamBrowserObserver
		virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) { 
			RESULT r = R_PASS;

			if (m_pParentDOS != nullptr) {
				int numFrames = pendingAudioPacket.GetNumFrames();
				
				//AudioPacket alteredPendingAudioPacket = pendingAudioPacket;
				//
				////alteredPendingAudioPacket.SetSamplingRate(44100);
				////alteredPendingAudioPacket.SetNumFrames(441);
				//
				//CRM(m_pParentDOS->PushAudioPacketToMixdown(numFrames, alteredPendingAudioPacket), "Failed to push packet to sound system");

				CRM(m_pParentDOS->PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET::LOCAL_BROWSER_0, numFrames, pendingAudioPacket), "Failed to push packet to sound system");
			}

		Error:
			return r;
		};

		virtual RESULT UpdateControlBarText(std::string& strTitle) { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT UpdateAddressBarSecurity(bool fSecure) override { return R_NOT_IMPLEMENTED_WARNING; }
		virtual RESULT UpdateAddressBarText(std::string& strURL) override { return R_NOT_IMPLEMENTED_WARNING; }

		virtual RESULT UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) { return R_NOT_IMPLEMENTED_WARNING; };

		virtual RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT HandleIsInputFocused(bool fIsInputFocused, DreamContentSource *pContext) { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT HandleLoadEnd() { return R_NOT_IMPLEMENTED_WARNING; };

		virtual RESULT HandleDreamFormSuccess() { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT HandleDreamFormCancel() { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) { return R_NOT_IMPLEMENTED_WARNING; };

		virtual RESULT HandleCanTabNext(bool fCanNext) { return R_NOT_IMPLEMENTED_WARNING; };
		virtual RESULT HandleCanTabPrevious(bool fCanPrevious) { return R_NOT_IMPLEMENTED_WARNING; };

		virtual std::string GetCertificateErrorURL() { return std::string(""); };
		virtual std::string GetLoadErrorURL() { return std::string(""); };

		// Browser
		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		
		std::shared_ptr<DreamBrowserApp> m_pDreamBrowserSource = nullptr;
		quad *m_pBrowserSourceQuad = nullptr;

		std::shared_ptr<DreamBrowserApp> m_pDreamBrowserDest = nullptr;
		quad *m_pBrowserDestQuad = nullptr;
		
		DreamOS *m_pParentDOS = nullptr;

	} *pTestContext = new TestContext();

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		std::string strURLSource = "https://www.youtube.com/watch?v=Ic4xAuIkoFE";

		std::string strURLDest = "https://online-voice-recorder.com/";
		//std::string strURLDest = "https://www.podcastinsights.com/online-mic-test/";

		CN(m_pDreamOS);

		CR(SetupPipeline("standard"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Need for the push packet to mixdown
		pTestContext->m_pParentDOS = m_pDreamOS;

		{
			light *pLight;
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));

			point ptPosition = point(0.0f, 0.0f, -radius);
			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
			CN(pTestContext->pSphere);
			pTestContext->pSphere->SetPosition(ptPosition);

			//// Open a sound file
			auto pNewSoundFile = m_pDreamOS->LoadSoundFile(L"95BPMPiano01.wav", SoundFile::type::WAVE);
			CN(pNewSoundFile);

			// Should work without sound client
			//CR(m_pDreamOS->RegisterSoundSystemObserver(pTestContext));

			//CR(m_pDreamOS->PlaySoundFile(pNewSoundFile));
			//CR(m_pDreamOS->LoopSoundFile(pNewSoundFile));

			///* Source Browser
			// Set up Browser to test the mix down code (timing)
			pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
			CN(pTestContext->m_pWebBrowserManager);
			CR(pTestContext->m_pWebBrowserManager->Initialize());

			// This presents a timing issue if it works 
			pTestContext->m_pBrowserSourceQuad = m_pDreamOS->AddQuad(4.8f, 2.7f);
			CN(pTestContext->m_pBrowserSourceQuad);
			pTestContext->m_pBrowserSourceQuad->FlipUVHorizontal();
			pTestContext->m_pBrowserSourceQuad->RotateXByDeg(90.0f);
			pTestContext->m_pBrowserSourceQuad->RotateZByDeg(180.0f);
			pTestContext->m_pBrowserSourceQuad->translateX(-2.5f);
			pTestContext->m_pBrowserSourceQuad->SetMaterialAmbient(1.0f);

			// Create the Shared View App
			pTestContext->m_pDreamBrowserSource = m_pDreamOS->LaunchDreamApp<DreamBrowserApp>(this);
			pTestContext->m_pDreamBrowserSource->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURLSource);
			CNM(pTestContext->m_pDreamBrowserSource, "Failed to create source dream browser");
			CR(pTestContext->m_pDreamBrowserSource->SetForceObserverAudio(true));
			CRM(pTestContext->m_pDreamBrowserSource->RegisterObserver(pTestContext), "Failed to set browser observer");

			pTestContext->m_pDreamBrowserSource->SetURI(strURLSource);
			//*/

			/* Destination (named pipe) Browser
			// Set up Browser to test the mix down code (timing)

			// This presents a timing issue if it works 
			pTestContext->m_pBrowserDestQuad = m_pDreamOS->AddQuad(4.8f, 2.7f);
			CN(pTestContext->m_pBrowserDestQuad);
			pTestContext->m_pBrowserDestQuad->FlipUVHorizontal();
			pTestContext->m_pBrowserDestQuad->RotateXByDeg(90.0f);
			pTestContext->m_pBrowserDestQuad->RotateZByDeg(180.0f);
			pTestContext->m_pBrowserDestQuad->translateX(2.5f);
			pTestContext->m_pBrowserDestQuad->SetMaterialAmbient(1.0f);

			// Create the Shared View App
			pTestContext->m_pDreamBrowserDest = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
			pTestContext->m_pDreamBrowserDest->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURLSource);
			CNM(pTestContext->m_pDreamBrowserDest, "Failed to create destination dream browser");
			//CR(pTestContext->m_pDreamBrowserDest->SetForceObserverAudio(true));
			//CRM(pTestContext->m_pDreamBrowserDest->RegisterObserver(pTestContext), "Failed to set browser observer");

			pTestContext->m_pDreamBrowserDest->SetURI(strURLDest);
			//*/
		}

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		CR(r);

	Error:
		return r;
	};

	// Update Code
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{
			if (pTestContext->m_pBrowserSourceQuad != nullptr) {
				pTestContext->m_pBrowserSourceQuad->SetDiffuseTexture(pTestContext->m_pDreamBrowserSource->GetSourceTexture());
			}

			if (pTestContext->m_pBrowserDestQuad != nullptr) {
				pTestContext->m_pBrowserDestQuad->SetDiffuseTexture(pTestContext->m_pDreamBrowserDest->GetSourceTexture());
			}
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

	auto pUITest = AddTest(strTestName, fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription(strTestDescription);
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamVCam() {
	RESULT r = R_PASS;

	double sTestTime = 500000.0f;
	int nRepeats = 1;
	//const int numTests = 5;

	struct TestContext {
		// VCam
		texture *pTexture = nullptr;
		unsigned char *pBuffer = nullptr;
		std::shared_ptr<DreamVCamApp> pDreamVCam = nullptr;
		ProgramNode* pRenderNode = nullptr;
		ProgramNode* pEndAuxNode = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;

		// Browser
		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		std::shared_ptr<DreamBrowserApp> m_pDreamBrowser = nullptr;
		quad *m_pBrowserQuad = nullptr;
	} *pTestContext = new TestContext();

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Set up the pipeline

		std::string strURL = "https://www.webcamtests.com";

		ProgramNode* pRenderProgramNode = nullptr;
		ProgramNode* pReferenceGeometryProgram = nullptr;
		ProgramNode* pSkyboxProgram = nullptr;
		ProgramNode* pUIProgramNode = nullptr;
		ProgramNode *pRenderScreenQuad = nullptr;

		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		// TODO: This test doesn't actually need an aux node 
		// webcam won't be mirrored to window after all
		//SinkNode *pAuxSinkNode;
		//pAuxSinkNode = pPipeline->GetAuxiliarySinkNode();
		//CNM(pAuxSinkNode, "Aux sink node isn't set");

		CN(m_pDreamOS);

		CR(pHAL->MakeCurrentContext());

		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		/*
		// Aux
		
		CameraNode* pAuxCamera;
		pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(1280, 720, 60));
		CN(pAuxCamera);
		CB(pAuxCamera->incRefCount());

		pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));

		// Reference Geometry Shader Program
		pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));
		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));
		//*/

		// Don't actually need to hook up the AUX node 
		//// Connect to aux (we will likely need to reproduce the pipeline)
		//if (pAuxSinkNode != nullptr) {
		//	CR(pAuxSinkNode->ConnectToInput("camera", pAuxCamera->Output("stereocamera")));
		//	CR(pAuxSinkNode->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));
		//}

		//CR(pDestSinkNode->ConnectToAllInputs(pSkyboxProgram->Output("output_framebuffer")));

		// Hook up a texture
		{
			auto pTestContext = reinterpret_cast<TestContext*>(pContext);
			CN(pTestContext);

			/*
			// Set up Browser to test
			pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
			CN(pTestContext->m_pWebBrowserManager);
			CR(pTestContext->m_pWebBrowserManager->Initialize());

			// This presents a timing issue if it works 
			pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(4.8f, 2.7f);
			CN(pTestContext->m_pBrowserQuad);
			pTestContext->m_pBrowserQuad->RotateXByDeg(90.0f);
			pTestContext->m_pBrowserQuad->RotateZByDeg(180.0f);
			pTestContext->m_pBrowserQuad->SetMaterialAmbient(1.0f);

			// Create the Shared View App
			pTestContext->m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
			pTestContext->m_pDreamBrowser->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
			CNM(pTestContext->m_pDreamBrowser, "Failed to create dream browser");

			pTestContext->m_pDreamBrowser->SetURI(strURL);
			//*/

			pTestContext->pRenderNode = pRenderProgramNode;
			pTestContext->pEndAuxNode = pSkyboxProgram;

			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
			CN(pLight);

			sphere *pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
			CN(pSphere);

			// Create the VCam		
			pTestContext->pDreamVCam = m_pDreamOS->LaunchDreamModule<DreamVCamApp>(this);
			CNM(pTestContext->pDreamVCam, "Failed to create dream virtual camera");
			pTestContext->pDreamVCam->InitializePipeline();

			//pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"Brick_1280x720.jpg");
			//CN(pTestContext->pTexture);

			//OGLTexture *pOGLTexture = dynamic_cast<OGLTexture*>(pTestContext->pTexture);
			//CN(pOGLTexture);
			//CR(pOGLTexture->EnableOGLPBOPack());

			//CRM(pTestContext->pDreamVCam->SetSourceTexture(pTestContext->pTexture), "Failed to set source texture for Dream VCam");

			// Only the render node actually has a frame buffer
			OGLProgram *pOGLProgram = dynamic_cast<OGLProgram*>(pRenderProgramNode);
			CN(pOGLProgram);
			
			//CRM(pTestContext->pDreamVCam->SetSourceTexture(pOGLProgram->GetOGLFramebufferColorTexture()), 
				//"Failed to set source texture for Dream VCam");
			/*
			{
				auto pComposite = m_pDreamOS->AddComposite();
				pComposite->InitializeOBB();

				auto pView = pComposite->AddUIView(m_pDreamOS);
				pView->InitializeOBB();

				pTestContext->pQuad = pView->AddQuad(.938f * 4.0, .484f * 4.0, 1, 1, nullptr, vector::kVector());
				pTestContext->pQuad->SetPosition(0.0f, 0.0f, 0.0f);
				pTestContext->pQuad->FlipUVVertical();
				pTestContext->pQuad->SetDiffuseTexture(pOGLProgram->GetOGLFramebufferColorTexture());
			}
			//*/
			auto pDreamGamepadCamera = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			//CR(pDreamGamepadCamera->SetCamera(pAuxCamera));
			CR(pDreamGamepadCamera->SetCamera(pTestContext->pDreamVCam->GetCameraNode(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));
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

		int count = 0;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		if (pTestContext->m_pBrowserQuad != nullptr) {
			pTestContext->m_pBrowserQuad->SetDiffuseTexture(pTestContext->m_pDreamBrowser->GetSourceTexture());
		}

		/* 
		// Now done in the module
		{
			static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

			if (pTestContext->pDreamVCam != nullptr && pTestContext->pRenderNode != nullptr) {

				std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

				// Approximately 30 FPS - 30 ms per frame is a bit faster
				if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 41) {

					// Only the render node actually has a frame buffer
					OGLProgram *pOGLProgram = dynamic_cast<OGLProgram*>(pTestContext->pRenderNode);
					CN(pOGLProgram);

					OGLTexture *pOGLTexture = dynamic_cast<OGLTexture*>(pOGLProgram->GetOGLFramebufferColorTexture());
					CN(pOGLTexture);
					
					if (pOGLTexture->IsOGLPBOPackEnabled()) {
						CR(pOGLTexture->EnableOGLPBOPack());
					}
					pTestContext->pQuad->SetDiffuseTexture(pOGLTexture);

					pTestContext->pDreamVCam->UnsetSourceTexture();
					CRM(pTestContext->pDreamVCam->SetSourceTexture(pOGLTexture),
						"Failed to set source texture for Dream VCam");


					CR(pTestContext->pEndAuxNode->RenderNode(count++));

					lastUpdateTime = timeNow;
				}
			}
		}
		//*/

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

	auto pUITest = AddTest("vcam", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Testing the dream virtual camera module");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestModuleManager() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;
	//const int numTests = 5;

	class DreamTestingModule : public DreamModule<DreamTestingModule> {
		friend class DreamModuleManager;

	public:
		DreamTestingModule(DreamOS *pDreamOS, void *pContext = nullptr) :
			DreamModule<DreamTestingModule>(pDreamOS, pContext)
		{
			// empty
		}

		~DreamTestingModule() {
			Shutdown();
		}

		virtual RESULT InitializeModule(void *pContext = nullptr) {
			RESULT r = R_PASS;

			SetName("DreamSoundSystem");
			SetModuleDescription("The Dream System Module");

			// nullptr is optional, but added to the test for completeness
			CR(StartModuleProcess(nullptr));	

		Error:
			return r;
		}
		virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override {
			return R_PASS;
		}

		virtual RESULT Update(void *pContext = nullptr) override {
			RESULT r = R_PASS;

			//CR(Print(std::to_string(m_testingValue)));

		Error:
			return r;
		}

		virtual RESULT Shutdown(void *pContext = nullptr) override {
			return R_PASS;
		}

		RESULT SetTestingValue(int i) {
			m_testingValue = i;
			return R_PASS;
		}

		virtual RESULT ModuleProcess(void *pContext) override { 
			RESULT r = R_PASS;

			while (true) {
				DEBUG_LINEOUT("module %d: count %d", m_testingValue, m_count++);

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

		Error:
			return r;
		}

	protected:
		static DreamTestingModule* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr) {
			DreamTestingModule *pDreamModule = new DreamTestingModule(pDreamOS, pContext);
			return pDreamModule;
		}

	private:
		int m_testingValue = -1;
		int m_count = 0;
	};

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamTestingModule> pDreamTestModules[5];

		CN(m_pDreamOS);

		CR(SetupPipeline());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		sphere *pSphere;
		pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);

		// Create the testing modules
		for (int i = 0; i < 5; i++) {
			pDreamTestModules[i] = m_pDreamOS->LaunchDreamModule<DreamTestingModule>(this);
			CNM(pDreamTestModules[i], "Failed to create dream test module");
			pDreamTestModules[i]->SetTestingValue(i);
		}

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

	auto pUITest = AddTest("modulemanager", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Testing module manager functionality");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamApps() {
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

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 10.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

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

	auto pUITest = AddTest("appmanager", fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestDescription("Testing the app management capabilities");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestUserApp() {
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

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

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

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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

	auto pUITest = AddTest("dreamuserapp", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Test the Dream user app");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// A test that includes all the basic UI apps in a functional state.
// User, ControlView, Keyboard, Browser, UIBar
RESULT DOSTestSuite::AddTestDreamOS() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext {
		std::shared_ptr<DreamUserApp> pUser = nullptr;
		std::shared_ptr<DreamUserControlAreaApp> pUserControlArea = nullptr;
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowserApp> pDreamBrowserApp = nullptr;
		std::shared_ptr<DreamUIBarApp> pDreamUIBar = nullptr;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		CR(SetupPipeline());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		// Cloud Controller
		{
			CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
			CommandLineManager *pCommandLineManager = CommandLineManager::instance();
			MenuControllerProxy *pMenuControllerProxy = nullptr;
			CN(pContext);
			CN(pCloudController);
			CN(pCommandLineManager);

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
			//CR(pCloudController->RegisterEnvironmentObserver(this));
		}
		//pDreamControlView = m_pDreamOS->LaunchDreamApp<DreamControlView>(this, false);
		//CN(pDreamControlView);

		// UIKeyboard App
		//m_pDreamOS->LaunchDreamApp<DreamFormApp>(this, false);

		pTestContext->pUser = m_pDreamOS->LaunchDreamApp<DreamUserApp>(this);
		CN(pTestContext->pUser);

		pTestContext->pUserControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlAreaApp>(this);
		CN(pTestContext->pUserControlArea);

		pTestContext->pUserControlArea->SetDreamUserApp(pTestContext->pUser);

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

		pTestContext->pUserControlArea->Update();

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

	auto pUITest = AddTest("dreamos", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("General DOS test");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestDreamShareView() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

	struct TestTimingContext {
		double m_msStart;
		std::shared_ptr<DreamShareViewApp> pDreamShareViewApp;
	};

	TestTimingContext *pTestContext = new TestTimingContext();
	pTestContext->m_msStart = msNow;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		//CR(SetupDreamAppPipeline());
		CR(SetupPipeline());

		{
			std::shared_ptr<DreamShareViewApp> pDreamShareViewApp = nullptr;

			auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);
			pDreamShareViewApp = m_pDreamOS->LaunchDreamApp<DreamShareViewApp>(this);
			pDreamShareViewApp->Show();

			auto pCastTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"website.png");
			//pDreamShareView->SetCastingTexture(std::shared_ptr<texture>(pCastTexture));
			pDreamShareViewApp->SetCastingTexture(pCastTexture);

			pTestContext->pDreamShareViewApp = pDreamShareViewApp;
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
			pTestContext->pDreamShareViewApp->ShowLoadingTexture();
		}
		else {
			pTestContext->pDreamShareViewApp->ShowCastingTexture();
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

	auto pUITest = AddTest("dreamshareview", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Test the Dream Share View app");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: What does this test do?
RESULT DOSTestSuite::AddTestBasicBrowserCast() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

	struct TestTimingContext {
		double m_msStart;
		std::shared_ptr<DreamShareViewApp> pDreamShareViewApp;
	};

	TestTimingContext *pTestContext = new TestTimingContext();
	pTestContext->m_msStart = msNow;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowserApp> pDreamBrowserApp = nullptr;
		std::shared_ptr<Dream2DMouseApp> pDream2DMouse = nullptr;

		std::string strURL = "http://www.youtube.com";

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		//std::string strURL = "http://ncu.rcnpv.com.tw/Uploads/20131231103232738561744.pdf";

		CN(m_pDreamOS);

		//CR(SetupDreamAppPipeline());
		CR(SetupPipeline());

		{
			std::shared_ptr<DreamShareViewApp> pDreamShareViewApp = nullptr;

			auto pTestContext = reinterpret_cast<TestTimingContext*>(pContext);
			pDreamShareViewApp = m_pDreamOS->LaunchDreamApp<DreamShareViewApp>(this);
			pDreamShareViewApp->Show();

			auto pCastTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"website.png");
			//pDreamShareView->SetCastingTexture(std::shared_ptr<texture>(pCastTexture));
			pDreamShareViewApp->SetCastingTexture(pCastTexture);

			pTestContext->pDreamShareViewApp = pDreamShareViewApp;

			// Create the Shared View App
			pDreamBrowserApp = m_pDreamOS->LaunchDreamApp<DreamBrowserApp>(this);
			CNM(pDreamBrowserApp, "Failed to create dream browser");

			// Set up the view
			//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
			pDreamBrowserApp->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
			pDreamBrowserApp->SetDiagonalSize(10.0f);
			pDreamBrowserApp->SetPosition(point(4.0f, 0.0f, 0.0f));
		}

		pDreamBrowserApp->SetURI(strURL);

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

	auto pUITest = AddTest("basicbrowsercast", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Browser casting test");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT DOSTestSuite::AddTestDreamDesktop() {
	RESULT r = R_PASS;

	double sTestTime = 10000.0;

	struct TestContext {
		std::shared_ptr<DreamDesktopApp> pDreamDesktop = nullptr;
		std::shared_ptr<DreamUserControlAreaApp> pDreamUserControlArea = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;
		texture* pTexture = nullptr;
		bool once = false;
	};
	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		
		//SetupDreamAppPipeline();
		CR(SetupPipeline());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));
		
		{
			//std::shared_ptr<EnvironmentAsset> pEnvAsset = nullptr;
			pTestContext->pDreamDesktop = m_pDreamOS->LaunchDreamApp<DreamDesktopApp>(this);
			CNM(pTestContext->pDreamDesktop, "Failed to create dream desktop");
			pTestContext->pDreamDesktop->StartDuplicationProcess();
			//pTestContext->pDreamUserControlArea = m_pDreamOS->LaunchDreamApp<DreamUserControlArea>(this);
			//pTestContext->pDreamUserControlArea->Show();
			//pTestContext->pDreamUserControlArea->AddEnvironmentAsset(pEnvAsset);
			//pTestContext->pDreamUserControlArea->SetActiveSource(pTestContext->pDreamDesktop);	
			//pTestContext->pDreamUserControlArea->GetComposite()->SetPosition(m_pDreamOS->GetCameraPosition() + point(0.0f, 1.5f, -.3f));
			//pTestContext->pDreamDesktop->InitializeWithParent(pTestContext->pDreamUserControlArea.get());
			
			/*
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

			pTestContext->pTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, pxWidth, pxHeight, PIXEL_FORMAT::BGRA, 4, m_pDataBuffer, (int)m_pDataBuffer_n);
			CR(dynamic_cast<OGLTexture*>(pTestContext->pTexture)->EnableOGLPBOUnpack());

			m_pDataBuffer_n = 0;
			
			pTestContext->pQuad->SetDiffuseTexture(pTestContext->pTexture);
			*/
		}

	Error:
		return r;
	};	// Test Code (this evaluates the test upon completion)

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		
		if (!pTestContext->once) {
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

	auto pUITest = AddTest("dreamdesktop", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Dream Desktop working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestGamepadCamera() {
	RESULT r = R_PASS;

	double sTestTime = 30000.0f;
	int nRepeats = 1;

	struct TestContext {
		sphere *pSphereGreen = nullptr;
		sphere *pSphereBlue = nullptr;
		sphere *pSphereRed = nullptr;
		sphere *pSphereWhite = nullptr;

		std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;

	};
	TestContext *pTestContext = new TestContext();

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// TODO:
		std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;

		CN(m_pDreamOS);

		CR(SetupPipeline("minimal"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		
		// Create the Shared View App
		{
			auto pDreamGamepadCamera = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CR(pDreamGamepadCamera->SetCamera(m_pDreamOS->GetCamera(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));
		}
		//CNM(pTestContext->pDreamUserApp, "Failed to create dream user app");

		// Sphere test
		pTestContext->pSphereGreen = m_pDreamOS->AddSphere(1.1f, 10, 10);
		pTestContext->pSphereBlue = m_pDreamOS->AddSphere(1.1f, 10, 10);
		pTestContext->pSphereRed = m_pDreamOS->AddSphere(1.1f, 10, 10);
		pTestContext->pSphereWhite = m_pDreamOS->AddSphere(1.1f, 10, 10);
		
		CN(pTestContext->pSphereGreen);
		CN(pTestContext->pSphereBlue);
		CN(pTestContext->pSphereRed);
		CN(pTestContext->pSphereWhite);
		
		pTestContext->pSphereGreen->SetMaterialColors(COLOR_GREEN);
		pTestContext->pSphereBlue->SetMaterialColors(COLOR_BLUE);
		pTestContext->pSphereRed->SetMaterialColors(COLOR_RED);
		pTestContext->pSphereWhite->SetMaterialColors(COLOR_WHITE);
		
		pTestContext->pSphereGreen->SetPosition(0, -10, 0);
		pTestContext->pSphereBlue->SetPosition(0, 20, 0);
		pTestContext->pSphereRed->SetPosition(20, 0, 0);
		pTestContext->pSphereWhite->SetPosition(-20, 0, 0);

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
		//CN(pTestContext);

	//Error:
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

	auto pUITest = AddTest("gamepadcamera", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test moving camera with gamepad");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestEnvironmentSwitching() {
	RESULT r = R_PASS;

	double sTestTime = 300.0f;
	int nRepeats = 1;

	struct TestContext {
		OGLProgramScreenFade *pScreenFadeProgram = nullptr;
		std::shared_ptr<DreamEnvironmentApp> pEnvironmentApp = nullptr;

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
		HALImp *pHAL = m_pDreamOS->GetHALImp();

		Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();
		SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CN(pTestContext);

		pTestContext->m_startTime = std::chrono::high_resolution_clock::now();

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

			/*
			vector vLightDirection = vector(1.0f, -1.0f, 0.0f);
			float lightIntensity = 1.0f;
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLightDirection);
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.70f * lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f * vLightDirection));
			//*/
			std::vector<SkyboxScatterProgram*> skyboxProgramNodes;
			skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pReflectionSkyboxProgram));
			skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pSkyboxProgram));

			pTestContext->pEnvironmentApp = m_pDreamOS->LaunchDreamApp<DreamEnvironmentApp>(this);
			CN(pTestContext->pEnvironmentApp);
			pTestContext->pEnvironmentApp->SetCurrentEnvironment(environment::CAVE);

			pTestContext->pEnvironmentApp->SetScreenFadeProgram(pTestContext->pScreenFadeProgram);
			pTestContext->pEnvironmentApp->SetSkyboxPrograms(skyboxProgramNodes);
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
					//pTestContext->pScreenFadeProgram->FadeOut();

					pTestContext->pEnvironmentApp->HideEnvironment(nullptr);
				}
				else {
					//pTestContext->pScreenFadeProgram->FadeIn();

					pTestContext->pEnvironmentApp->ShowEnvironment(nullptr);
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	// Add the test
	auto pNewTest = AddTest("environmentswitching", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Environment fade shader and switching test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DOSTestSuite::AddTestEnvironmentSeating() {
	RESULT r = R_PASS;

	double sTestTime = 300.0f;
	int nRepeats = 1;

	struct TestContext {
		OGLProgramScreenFade *pScreenFadeProgram = nullptr;
		std::shared_ptr<DreamEnvironmentApp> pEnvironmentApp = nullptr;

		std::vector<user*> m_users;
		std::shared_ptr<texture> m_pTestTexture;

		bool fFirst = true;
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
		HALImp *pHAL = m_pDreamOS->GetHALImp();

		Pipeline* pRenderPipeline = pHAL->GetRenderPipelineHandle();
		SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CN(pTestContext);

		{
			// Skybox

			ProgramNode* pScatteringSkyboxProgram;
			pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
			CN(pScatteringSkyboxProgram);
			CR(pScatteringSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// Reflection 
			
			auto m_pReflectionProgramNode = pHAL->MakeProgramNode("reflection");
			CN(m_pReflectionProgramNode);
			//CR(m_pReflectionProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
			CR(m_pReflectionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			ProgramNode* pReflectionSkyboxProgram;
			pReflectionSkyboxProgram = pHAL->MakeProgramNode("skybox");
			CN(pReflectionSkyboxProgram);
			CR(pReflectionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
			CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
			CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", m_pReflectionProgramNode->Output("output_framebuffer")));

			// Refraction

			
			auto m_pRefractionProgramNode = pHAL->MakeProgramNode("refraction");
			CN(m_pRefractionProgramNode);
			//CR(pRefractionProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
			CR(m_pRefractionProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			//ProgramNode* pRefractionSkyboxProgram;
			//pRefractionSkyboxProgram = pHAL->MakeProgramNode("skybox");
			//CN(pRefractionSkyboxProgram);
			//CR(pRefractionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
			//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
			//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer", m_pRefractionProgramNode->Output("output_framebuffer")));

			// "Water"

			ProgramNode* pWaterProgramNode = pHAL->MakeProgramNode("water");
			CN(pWaterProgramNode);
			// Still need scene graph for lights 
			// TODO: make lights a different node
			//CR(pWaterProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
			CR(pWaterProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// TODO: This is not particularly general yet
			// Uncomment below to turn on water effects
			CR(pWaterProgramNode->ConnectToInput("input_refraction_map", m_pRefractionProgramNode->Output("output_framebuffer")));
			CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));

			// Environment shader

			auto m_pRenderEnvironmentProgramNode = pHAL->MakeProgramNode("minimal_texture");
			CN(m_pRenderEnvironmentProgramNode);
			//CR(m_pRenderEnvironmentProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(m_pRenderEnvironmentProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			CR(m_pRenderEnvironmentProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));

			// Everything else
			ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("standard");
			CN(pRenderProgramNode);
			CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			// NOTE: Add this in if you want to have reflective objects
			//CR(pRenderProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

			CR(pRenderProgramNode->ConnectToInput("input_framebuffer", m_pRenderEnvironmentProgramNode->Output("output_framebuffer")));

			// Reference Geometry Shader Program
			ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
			CN(pReferenceGeometryProgram);
			CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
			CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

			CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

			// Skybox
			ProgramNode* pSkyboxProgram;
			pSkyboxProgram = pHAL->MakeProgramNode("skybox");
			CN(pSkyboxProgram);
			CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
			CR(pSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
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

			// save interface for UI apps
			m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

			// save interfaces to skybox nodes
			std::vector<SkyboxScatterProgram*> m_skyboxProgramNodes;
			m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pScatteringSkyboxProgram));
			//m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pReflectionSkyboxProgram));
			//m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pSkyboxProgram));

			auto pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

			/*
			ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("minimal_texture");
			CN(pUIProgramNode);
			CR(pUIProgramNode->ConnectToInput("scenegraph", GetUISceneGraphNode()->Output("objectstore")));
			CR(pUIProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));
			CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
			//*/

			// Screen Quad Shader (opt - we could replace this if we need to)
			ProgramNode *pRenderScreenFade = pHAL->MakeProgramNode("screenfade");
			CN(pRenderScreenFade);
			CR(pRenderScreenFade->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

			auto m_pScreenFadeProgramNode = dynamic_cast<OGLProgramScreenFade*>(pRenderScreenFade);
			// Connect Program to Display
			//CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
			CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenFade->Output("output_framebuffer")));

			//CR(pHAL->ReleaseCurrentContext());

			quad *pWaterQuad = m_pDreamOS->MakeQuad(1000.0f, 1000.0f);
			point ptQuadOffset = point(90.0f, -1.3f, -25.0f);
			pWaterQuad->SetPosition(ptQuadOffset);
			pWaterQuad->SetMaterialColors(color(57.0f / 255.0f, 112.0f / 255.0f, 151.0f / 255.0f, 1.0f));
			CN(pWaterQuad);

			if (pWaterProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(pWaterQuad));
			}

			if (m_pReflectionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramReflection*>(m_pReflectionProgramNode)->SetReflectionObject(pWaterQuad));
			}

			if (m_pRefractionProgramNode != nullptr) {
				CR(dynamic_cast<OGLProgramRefraction*>(m_pRefractionProgramNode)->SetRefractionObject(pWaterQuad));
			}

			if (pReflectionSkyboxProgram != nullptr) {
				CR(dynamic_cast<OGLProgramSkybox*>(pReflectionSkyboxProgram)->SetReflectionObject(pWaterQuad));
			}

			/*
			vector vLightDirection = vector(1.0f, -1.0f, 0.0f);
			float lightIntensity = 1.0f;
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLightDirection);
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.70f * lightIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-1.0f * vLightDirection));
			//*/
			std::vector<SkyboxScatterProgram*> skyboxProgramNodes;
			skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pReflectionSkyboxProgram));
			skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pSkyboxProgram));

			pTestContext->pEnvironmentApp = m_pDreamOS->LaunchDreamApp<DreamEnvironmentApp>(this, false);
			CN(pTestContext->pEnvironmentApp);
			pTestContext->pEnvironmentApp->SetCurrentEnvironment(environment::CAVE);

			pTestContext->pEnvironmentApp->SetScreenFadeProgram(pTestContext->pScreenFadeProgram);
			pTestContext->pEnvironmentApp->SetSkyboxPrograms(skyboxProgramNodes);

			auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
			CN(pDreamGamepadApp);

			 CR(m_pRenderEnvironmentProgramNode->ConnectToInput("scenegraph", pTestContext->pEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));
			 CR(m_pReflectionProgramNode->ConnectToInput("scenegraph", pTestContext->pEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));
			 CR(m_pRefractionProgramNode->ConnectToInput("scenegraph", pTestContext->pEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));
		}

		m_pDreamOS->GetCamera()->SetPosition(point(0.0f, 5.0f, 0.0f));
		pTestContext->m_pTestTexture = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"mouth.png"));
		for (int i = 0; i < 6; i++) {
			pTestContext->m_users.emplace_back(m_pDreamOS->AddUser());
			pTestContext->m_users[i]->SetVisible(true);
			pTestContext->m_users[i]->SetDreamOS(m_pDreamOS);
			pTestContext->m_users[i]->UpdateAvatarModelWithID((i % 4) + 1);
		}

		{
			for (int i = 0; i < 6; i++) {
			//	pTestContext->pEnvironmentApp->SeatUser(pTestContext->m_users[i], i);
				point ptPosition;
				quaternion qOrientation;
				pTestContext->pEnvironmentApp->GetEnvironmentSeatingPositionAndOrientation(ptPosition, qOrientation, i);
				pTestContext->m_users[i]->SetPosition(ptPosition + point(0.0f, 1.0f, 0.0f));
				pTestContext->m_users[i]->SetOrientation(qOrientation);
				pTestContext->m_users[i]->SetMouthPosition(ptPosition + point(0.0f, 1.0f, 0.0f));
				pTestContext->m_users[i]->SetMouthOrientation(qOrientation);

				/*
				auto pQuad = m_pDreamOS->MakeQuad(1.0f, 1.0f);
				pQuad->SetPosition(ptPosition);
				pQuad->SetOrientation(qOrientation);
				pQuad->SetDiffuseTexture(pTestContext->m_pTestTexture.get());
				m_pDreamOS->AddObjectToUIGraph(pQuad);
				//*/
			}
		}
		//pTestContext->m_users[0]->SetPosition()

	Error:
		return r;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		if (pTestContext->fFirst) {
			pTestContext->pEnvironmentApp->ShowEnvironment(nullptr);
			pTestContext->fFirst = false;
		}
		for (int i = 0; i < 6; i++) {
			pTestContext->m_users[i]->SetVisible(true);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	// Add the test
	auto pNewTest = AddTest("environmentseating", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Test for the environment seating functions");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: Should move this to the sandbox test suite
RESULT DOSTestSuite::AddTestCredentialStorage() {
	RESULT r = R_PASS;

	double sTestTime = 3000.0f;
	int nRepeats = 1;

	// Initialize Code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		//CN(m_pDreamOS);

		//CR(SetupDreamAppPipeline());

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		std::wstring wstrKey = L"key";
		//std::string strField = "field";
		std::string strField = "v7BgzYwu2WhE0EuD2YAaLVIzCLhaJgFVNLEaD2I6NPptJOOVAS4VOuz7VCYaMbQi";
		std::string strOut;
		{
			m_pDreamOS->GetCredential(wstrKey, strOut, CredentialManager::type::CREDENTIAL_GENERIC);
			DEBUG_LINEOUT("old cred or error not found: %s", strOut.c_str());

			m_pDreamOS->SaveCredential(wstrKey, strField, CredentialManager::type::CREDENTIAL_GENERIC, true);

			m_pDreamOS->GetCredential(wstrKey, strOut, CredentialManager::type::CREDENTIAL_GENERIC);
			DEBUG_LINEOUT("saved cred: %s", strOut.c_str());

			m_pDreamOS->SaveCredential(wstrKey, "THIS ISNT SUPPOSED TO OVERWRITE", CredentialManager::type::CREDENTIAL_GENERIC, false);

			m_pDreamOS->GetCredential(wstrKey, strOut, CredentialManager::type::CREDENTIAL_GENERIC);
			DEBUG_LINEOUT("checking for overwrite: %s", strOut.c_str());

			m_pDreamOS->RemoveCredential(wstrKey, CredentialManager::type::CREDENTIAL_GENERIC);
		}

	//Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		//Error:
		return r;
	};

	// Reset Code
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

		// TODO: Kill apps as needed

	Error:
		return r;
	};

	auto pUITest = AddTest("credentialstorage", fnInitialize, fnUpdate, fnTest, fnReset);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of using password vault");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

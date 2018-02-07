#include "MultiContentTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/opengl/OGLProgram.h"

#include "DreamShareView/DreamShareView.h"
#include "DreamGarage/Dream2DMouseApp.h"
#include "DreamGarage/DreamBrowser.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "WebBrowser/WebBrowserController.h"

#include "Cloud/CloudController.h"
#include "Cloud/CloudControllerFactory.h"
#include "Cloud/HTTP/HTTPController.h"

#include "Sandbox/CommandLineManager.h"

MultiContentTestSuite::MultiContentTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
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

	CR(AddTestTwoBrowsers());

	CR(AddTestMultiPeerBrowser());

	CR(AddTestMultiPeerBasic());

Error:
	return r;
}

RESULT MultiContentTestSuite::SetupPipeline() {
	RESULT r = R_PASS;

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
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT MultiContentTestSuite::AddTestTwoBrowsers() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;


	auto fnInitialize = [&](void *pContext) {

		RESULT r = R_PASS;

		DOSLOG(INFO, "[WebRTCTestingSuite] Multipeer Test Initializing ... ");

		vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();
		std::shared_ptr<CEFBrowserManager> pWebBrowserManager = nullptr;
		CR(SetupPipeline());

		m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		m_pBrowser2 = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);

		pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		CN(pWebBrowserManager);
		CR(pWebBrowserManager->Initialize());


		m_pTestQuad1 = std::shared_ptr<quad>(m_pDreamOS->AddQuad(1, 1, 1, 1, nullptr, vNormal));
		CN(m_pTestQuad1);

		m_pTestQuad1->SetMaterialAmbient(0.90f);
		m_pTestQuad1->FlipUVVertical();
		m_pTestQuad1->SetPosition(point(-1.0f, 0.0f, 0.0f));

		m_pTestQuad2 = std::shared_ptr<quad>(m_pDreamOS->AddQuad(1, 1, 1, 1, nullptr, vNormal));
		CN(m_pTestQuad2);

		m_pTestQuad2->SetMaterialAmbient(0.90f);
		m_pTestQuad2->FlipUVVertical();
		m_pTestQuad2->SetPosition(point(1.0f, 0.0f, 0.0f));

//		m_pDreamBrowser->SetBrowserManager(pWebBrowserManager);
		m_pDreamBrowser->InitializeWithBrowserManager(pWebBrowserManager);
		m_pBrowser2->InitializeWithBrowserManager(pWebBrowserManager);

		m_pDreamBrowser->SetURI("www.google.com");
		m_pBrowser2->SetURI("www.trello.com");

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {

		m_pTestQuad1->SetDiffuseTexture(m_pDreamBrowser->GetScreenTexture().get());
		m_pTestQuad2->SetDiffuseTexture(m_pBrowser2->GetScreenTexture().get());
		return R_PASS;
	};
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
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
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		m_pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this, true);
		m_pDreamShareView->Show();

		m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		//m_pDreamShareView->ShowCast

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
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
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		m_pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this, true);
		m_pDreamShareView->Show();

		m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);

		m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		//m_pDreamShareView->ShowCast

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
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
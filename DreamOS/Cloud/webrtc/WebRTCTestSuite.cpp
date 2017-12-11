#include "WebRTCTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Sandbox/CommandLineManager.h"
#include "Cloud/HTTP/HTTPController.h"

#include "Cloud/CloudControllerFactory.h"

#include "HAL/opengl/OGLProgram.h"

#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/Dream2DMouseApp.h"

#include "DreamLogger/DreamLogger.h"
#include "Sound/SoundClientFactory.h"

WebRTCTestSuite::WebRTCTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

WebRTCTestSuite::~WebRTCTestSuite() {
	// empty
}

RESULT WebRTCTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestChromeMultiBrowser());

	CR(AddTestWebRTCMultiPeer());

	CR(AddTestWebRTCAudio());

	CR(AddTestWebRTCVideoStream());

	// TODO: Need a data channel test

Error:
	return r;
}

CloudController *WebRTCTestSuite::GetCloudController() {
	return m_pDreamOS->GetCloudController();
}

OGLProgram *g_pRenderProg = nullptr;

RESULT WebRTCTestSuite::SetupSkyboxPipeline(std::string strRenderShaderName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderShaderName);
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

	g_pRenderProg = (OGLProgram*)(pRenderScreenQuad);

Error:
	return r;
}

RESULT WebRTCTestSuite::AddTestWebRTCMultiPeer() {
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

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		DOSLOG(INFO, "[WebRTCTestingSuite] Multipeer Test Initializing ... ");

		CR(SetupSkyboxPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);

		//*/

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		// Cloud Controller
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pTestContext->pCloudController->Initialize(), "Failed to initialize cloud controller");

		CRM(pTestContext->pCloudController->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			std::string strUsername = "test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			long environmentID = 170;
			std::string strPassword = "nightmare";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", strPassword));

			CRM(pTestContext->pCloudController->Start(strUsername, strPassword, environmentID), "Failed to log in");
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

	pNewTest->SetTestName("Testing multi-peer connection of WebRTC");
	pNewTest->SetTestDescription("Test multi-peer connections of WebRTc");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

#define UPDATE_SCREENCAST_COUNT 5	
#define UPDATE_SCREENCAST_MS ((1000.0f) / UPDATE_SCREENCAST_COUNT)
std::chrono::system_clock::time_point g_lastTestUpdate = std::chrono::system_clock::now();

RESULT WebRTCTestSuite::AddTestWebRTCVideoStream() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public CloudController::PeerConnectionObserver {
		quad *pQuad = nullptr;
		texture *pQuadTexture = nullptr;

		quad *pSourceQuad = nullptr;
		texture *pSourceTexture = nullptr;

		CloudController *pCloudController = nullptr;

		struct PendingVideoBuffer {
			uint8_t *pPendingBuffer = nullptr;
			int pxWidth = 0;
			int pxHeight = 0;
			bool fPendingBufferReady = false;
		} m_pendingVideoBuffer;

		uint8_t *pTestVideoFrameBuffer = nullptr;

		// PeerConnectionObserver
		virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDreamMessage) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			RESULT r = R_PASS;

			CBM((m_pendingVideoBuffer.fPendingBufferReady == false), "Buffer already pending");

			m_pendingVideoBuffer.pPendingBuffer = pVideoFrameDataBuffer;
			m_pendingVideoBuffer.pxWidth = pxWidth;
			m_pendingVideoBuffer.pxHeight = pxHeight;
			m_pendingVideoBuffer.fPendingBufferReady = true;

		Error:
			return r;
		}

	} *pTestContext = new TestContext();

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// temp
		int pxWidth = 500;
		int pxHeight = 500;
		int channels = 4;

		std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

		CR(SetupSkyboxPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		
		pTestContext->pQuad = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1);
		CN(pTestContext->pQuad);
		pTestContext->pQuad->RotateXByDeg(45.0f);
		pTestContext->pQuad->translateX(-1.0f);

		pTestContext->pSourceQuad = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1);
		CN(pTestContext->pSourceQuad);
		pTestContext->pSourceQuad->RotateXByDeg(45.0f);
		pTestContext->pSourceQuad->translateX(1.0f);

		// Temporary
		///*
		pTestContext->pQuadTexture = m_pDreamOS->MakeTexture(
			texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, 
			pxWidth, 
			pxHeight, 
			PIXEL_FORMAT::RGBA,
			4, 
			&vectorByteBuffer[0], 
			pxWidth * pxHeight * 4
		);

		CN(pTestContext->pQuadTexture);
		pTestContext->pQuad->SetDiffuseTexture(pTestContext->pQuadTexture);

		pTestContext->pSourceTexture = m_pDreamOS->MakeTexture(
			texture::TEXTURE_TYPE::TEXTURE_DIFFUSE,
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA,
			4,
			&vectorByteBuffer[0],
			pxWidth * pxHeight * 4
		);

		CN(pTestContext->pSourceTexture);
		pTestContext->pSourceQuad->SetDiffuseTexture(pTestContext->pSourceTexture);

		size_t bufSize = sizeof(uint8_t) * pxWidth * pxHeight * channels;

		pTestContext->pTestVideoFrameBuffer = (uint8_t*)malloc(bufSize);
		CN(pTestContext->pTestVideoFrameBuffer);
		int styleCounter = 0;

		for (int i = 0; i < pxHeight; i++) {
			for (int j = 0; j < pxWidth; j++) {
				uint8_t cPixel[4] = { 0x00, 0x00, 0x00, 0xFF };
				cPixel[styleCounter] = 0xFF;

				size_t offset = (i * ((pxWidth - 1)) + (j));
				offset *= 4;

				CB((offset < bufSize));

				uint8_t *pPixelMemLocation = pTestContext->pTestVideoFrameBuffer + offset;

				memcpy(pPixelMemLocation, cPixel, sizeof(cPixel));
			}

			if (i % 50 == 0) {
				if (++styleCounter > 3) {
					styleCounter = 0;
				}
			}
		}

		CR(pTestContext->pSourceTexture->Update(
			(unsigned char*)(pTestContext->pTestVideoFrameBuffer),
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA)
		);

		CR(pTestContext->pSourceTexture->LoadImageFromTexture(0, PIXEL_FORMAT::BGRA));

		//*/

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		// Cloud Controller
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pTestContext->pCloudController->Initialize(), "Failed to initialize cloud controller");

		CRM(pTestContext->pCloudController->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			std::string strUsername = "test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			std::string strPassword = "nightmare";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", strPassword));
			CR(pCommandLineManager->SetParameterValue("environment", std::to_string(6)));

			CRM(pTestContext->pCloudController->Start(true), "Failed to start cloud controller");
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

		if (pTestContext->m_pendingVideoBuffer.fPendingBufferReady && pTestContext->m_pendingVideoBuffer.pPendingBuffer != nullptr) {
			// Update the video buffer to texture
			CR(pTestContext->pQuadTexture->Update(
				(unsigned char*)(pTestContext->m_pendingVideoBuffer.pPendingBuffer),
				pTestContext->m_pendingVideoBuffer.pxWidth,
				pTestContext->m_pendingVideoBuffer.pxHeight,
				PIXEL_FORMAT::RGBA)
			);
		}

		// Replace with BroadcastTexture
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - g_lastTestUpdate).count() > UPDATE_SCREENCAST_MS) {
			if (pTestContext->pCloudController != nullptr) {
				pTestContext->pCloudController->BroadcastTextureFrame(pTestContext->pSourceTexture, 0, PIXEL_FORMAT::RGBA);

				/*
				HALImp *pHAL = m_pDreamOS->GetHALImp();
				Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

				if (g_pRenderProg != nullptr) {
					auto pScreenQuadTexture = g_pRenderProg->GetOGLFramebufferColorTexture();

					if (pScreenQuadTexture != nullptr) {
						pTestContext->pCloudController->BroadcastTextureFrame(pScreenQuadTexture, 0, texture::PixelFormat::RGBA);
					}
				}
				*/
			}
	
			g_lastTestUpdate = std::chrono::system_clock::now();
		}

	Error:
		pTestContext->m_pendingVideoBuffer.fPendingBufferReady = false;

		if (pTestContext->m_pendingVideoBuffer.pPendingBuffer != nullptr) {
			delete pTestContext->m_pendingVideoBuffer.pPendingBuffer;
			pTestContext->m_pendingVideoBuffer.pPendingBuffer = nullptr;
		}

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

	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service - this will hang for a while");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT WebRTCTestSuite::AddTestWebRTCAudio() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public SoundClient::observer {
		CloudController *pCloudController = nullptr;
		SoundClient *pSoundClient = nullptr;

		RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
			RESULT r = R_PASS;

			/*
			// Simply pushes the capture buffer to the render buffer
			if (pSoundClient != nullptr) {
				CR(pSoundClient->PushMonoAudioBufferToRenderBuffer(numFrames, pCaptureBuffer));
			}
			//*/

			CR(r);

			///*
			// TODO: Broadcast this audio
			if (pCloudController != nullptr) {
				// TODO: Retrieve audio packet from capture buffer (might need copy
				// or convert to correct packet format
				pCaptureBuffer->IncrementBuffer(numFrames);
				//AudioPacket pendingAudioPacket = pCaptureBuffer->GetAudioPacket(numFrames);
				
				// Send a dummy audio packet (generating audio right now)
				AudioPacket pendingAudioPacket = AudioPacket(numFrames, 1, 16, nullptr);
				//pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);
				//pCloudController->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket);
				//pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);
			}
			//*/

		Error:
			return r;
		}

	} *pTestContext = new TestContext();

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<Dream2DMouseApp> pDream2DMouse = nullptr;

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		std::string strURL = "http://urlme.me/troll/dream_test/1.jpg";

		CR(SetupSkyboxPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// TODO: Why does shit explode with no objects in scene
		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		// Sound Client
		pTestContext->pSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
		CN(pTestContext->pSoundClient);

		CR(pTestContext->pSoundClient->RegisterObserver(pTestContext));
		CR(pTestContext->pSoundClient->Start());

		// Cloud Controller
		pTestContext->pCloudController = m_pDreamOS->GetCloudController();
		CN(pTestContext->pCloudController);

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pTestContext->pCloudController->Initialize(), "Failed to initialize cloud controller");

		// Log in 
		{
			long environmentID = 170;

			std::string strUsername = "test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";
			if (pCommandLineManager->GetParameterValue("testval") != "1") {
				strURL = "https://www.youtube.com/watch?v=5vZ4lCKv1ik";
			}

			std::string strPassword = "nightmare";

			CRM(pTestContext->pCloudController->Start(strUsername, strPassword, environmentID), "Failed to log in");
		}

		/*
		// Create the 2D Mouse App
		pDream2DMouse = m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		CNM(pDream2DMouse, "Failed to create dream 2D mouse app");

		// Create the Browser App
		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(10.0f);
		pDreamBrowser->SetURI(strURL);
		*/

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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("WebRTC Audio");
	pNewTest->SetTestDescription("Tests the multi-peer audio capabilities of WebRTC using the Dream Sound Client");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT WebRTCTestSuite::AddTestChromeMultiBrowser() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext {
		CloudController *pCloudController = nullptr;
	} *pTestContext = new TestContext();

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<Dream2DMouseApp> pDream2DMouse = nullptr;

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		std::string strURL = "http://urlme.me/troll/dream_test/1.jpg";

		CR(SetupSkyboxPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		CN(m_pDreamOS);

		pTestContext->pCloudController = m_pDreamOS->GetCloudController();

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		// Cloud Controller
		CN(pTestContext->pCloudController);

		DEBUG_LINEOUT("Initializing Cloud Controller");
		CRM(pTestContext->pCloudController->Initialize(), "Failed to initialize cloud controller");

		// Log in 
		{
			std::string strUsername = "test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			if (pCommandLineManager->GetParameterValue("testval") != "1") {
				strURL = "https://www.youtube.com/watch?v=5vZ4lCKv1ik";
			}

			std::string strPassword = "nightmare";

			std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");

			long environmentID = 170;

			//CR(pCommandLineManager->SetParameterValue("environment", std::to_string(6)));
			CR(pCommandLineManager->SetParameterValue("environment", std::to_string(environmentID)));

			//CRM(pTestContext->pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");

			CRM(pTestContext->pCloudController->Start(strUsername, strPassword, environmentID), "Failed to log in");
		}

		// Create the 2D Mouse App
		pDream2DMouse = m_pDreamOS->LaunchDreamApp<Dream2DMouseApp>(this);
		CNM(pDream2DMouse, "Failed to create dream 2D mouse app");

		// Create the Browser App
		pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
		CNM(pDreamBrowser, "Failed to create dream browser");

		// Set up the view
		//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
		pDreamBrowser->SetDiagonalSize(10.0f);

		pDreamBrowser->SetURI(strURL);

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
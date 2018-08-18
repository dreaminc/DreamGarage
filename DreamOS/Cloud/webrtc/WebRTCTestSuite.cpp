#include "WebRTCTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Sandbox/CommandLineManager.h"
#include "Cloud/HTTP/HTTPController.h"

#include "Cloud/CloudControllerFactory.h"

#include "HAL/opengl/OGLProgram.h"
#include "HAL\opengl\OGLTexture.h"

#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/Dream2DMouseApp.h"
#include "DreamShareView/DreamShareView.h"

#include "DreamLogger/DreamLogger.h"
#include "Sound/SoundClientFactory.h"
#include "Sound/SpatialSoundObject.h"

#include "Cloud/CloudTestSuite.h"
#include "Sound/AudioPacket.h"

#include "DreamGarage\DreamBrowser.h"
#include "WebBrowser\WebBrowserController.h"
#include "WebBrowser\CEFBrowser/CEFBrowserManager.h"

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

	CR(AddTestWebRTCAudio());

	CR(AddTestWebRTCMultiPeer());

	CR(AddTestChromeMultiBrowser());

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

	{

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderShaderName);
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		
		//Reference Geometry Shader Program
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
		//CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));
	}
	

	CR(pHAL->ReleaseCurrentContext());

	//g_pRenderProg = (OGLProgram*)(pRenderScreenQuad);

Error:
	return r;
}

RESULT WebRTCTestSuite::AddTestWebRTCMultiPeer() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : public CloudController::PeerConnectionObserver {

		CloudController *pCloudController = nullptr;
		int testUserNum = 0;

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
			DEBUG_LINEOUT("OnAudioData: %s", strAudioTrackLabel.c_str());

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

		CR(SetupSkyboxPipeline("standard"));

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
		DEBUG_LINEOUT("Initializing Cloud Controller");
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		CRM(pTestContext->pCloudController->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			std::string strUsername = "test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			pTestContext->testUserNum = std::stoi(pCommandLineManager->GetParameterValue("testval"));

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

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Cloud Controller
		CloudController *pCloudController = pTestContext->pCloudController;
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

		CloudController *pCloudController = pTestContext->pCloudController;
		CN(pCloudController);

		// Every 20 ms

		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

		{
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 20) {

				lastUpdateTime = timeNow;

				if (pCloudController != nullptr && pTestContext->testUserNum == 2) {
					// TODO: Retrieve audio packet from capture buffer (might need copy
					// or convert to correct packet format
					//pCaptureBuffer->IncrementBuffer(numFrames);
					//AudioPacket pendingAudioPacket = pCaptureBuffer->GetAudioPacket(numFrames);

					// Send a dummy audio packet (generating audio right now)
					int nChannels = 1;
					int samplingFrequency = 44100;
					int numFrames = (nChannels * samplingFrequency) * 0.01f;
					AudioPacket pendingAudioPacket = AudioPacket(numFrames, 1, 16, nullptr);
					//pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);

					// DO BOTH
					pCloudController->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket);
					pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);
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

RESULT WebRTCTestSuite::AddTestWebRTCAudio() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;
	float radius = 1.0f;

	struct TestContext : 
		public SoundClient::observer, 
		public CloudController::PeerConnectionObserver, 
		public CloudController::UserObserver,
		public DreamBrowserObserver
	{
		CloudController *pCloudController = nullptr;
		UserController *pUserController = nullptr;

		SoundClient *pWASAPICaptureClient = nullptr;
		SoundClient *pXAudio2AudioClient = nullptr;

		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;

		int testUserNum = 0;

		sphere *pSphere = nullptr;
		std::shared_ptr<SpatialSoundObject> pXAudioSpatialSoundObject1 = nullptr;
		std::shared_ptr<SpatialSoundObject> pXAudioSpatialSoundObject2 = nullptr;

		quad *m_pBrowserQuad = nullptr;
		texture *pQuadTexture = nullptr;
		
		//sphere *pSphereLeftChrome = nullptr;
		//sphere *pSphereRightChrome = nullptr;
		//std::shared_ptr<SpatialSoundObject> pXAudioSpatialSoundObjectLeftChrome = nullptr;
		//std::shared_ptr<SpatialSoundObject> pXAudioSpatialSoundObjectRightChrome = nullptr;
		

		struct PendingVideoBuffer {
			uint8_t *pPendingBuffer = nullptr;
			int pxWidth = 0;
			int pxHeight = 0;
			bool fPendingBufferReady = false;
		} m_pendingVideoBuffer;

		uint8_t *pTestVideoFrameBuffer = nullptr;

		// SoundClient::observer
		RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
			RESULT r = R_PASS;
			
			int nChannels = pCaptureBuffer->NumChannels();
			int samplingFrequency = pCaptureBuffer->GetSamplingRate();
			//numFrames = samplingFrequency / 100;

			AudioPacket pendingAudioPacket;
			pCaptureBuffer->GetAudioPacket(numFrames, &pendingAudioPacket);

			// Measure time diff
			static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			auto diffVal = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();
			lastUpdateTime = timeNow;

			if (pCloudController != nullptr && testUserNum == 2) {
				pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);	
			}

			std::chrono::system_clock::time_point timeNow2 = std::chrono::system_clock::now();
			auto diffVal2 = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow2 - timeNow).count();

		//Error:
			return r;
		}

		// DreamBrowserObserver
		virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override {
			RESULT r = R_PASS;

			if (pCloudController != nullptr && testUserNum == 2) {
				CR(pCloudController->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket));
			}

		Error:
			return r;
		}

		virtual RESULT UpdateControlBarText(std::string& strTitle) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateContentSourceTexture(std::shared_ptr<texture> pTexture, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleIsInputFocused(bool fIsInputFocused, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSuccess() override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormCancel() override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) override { return R_NOT_HANDLED; }
		virtual RESULT HandleCanTabNext(bool fCanNext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleCanTabPrevious(bool fCanPrevious) override { return R_NOT_HANDLED; }

		// CloudController::PeerConnectionObserver
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
			RESULT r = R_PASS;
			
			DEBUG_LINEOUT("OnAudioData: %s", strAudioTrackLabel.c_str());

			if (strAudioTrackLabel == kUserAudioLabel) {

				if (pXAudioSpatialSoundObject1 != nullptr) {
					// Do I need to copy the buffer over (getting over written maybe)
					int16_t *pInt16Soundbuffer = new int16_t[frames];
					memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * frames);

					if (pInt16Soundbuffer != nullptr) {
						CR(pXAudioSpatialSoundObject1->PushMonoAudioBuffer((int)frames, pInt16Soundbuffer));
					}
				}
			}
			else if (strAudioTrackLabel == kChromeAudioLabel) {
				
				if (pXAudioSpatialSoundObject1 != nullptr) {
					// Do I need to copy the buffer over (getting over written maybe)
					int16_t *pInt16Soundbuffer = new int16_t[frames];
					memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * frames);
				
					if (pInt16Soundbuffer != nullptr) {
						CR(pXAudioSpatialSoundObject2->PushMonoAudioBuffer((int)frames, pInt16Soundbuffer));
					}
				}
			}

		Error:
			return r;
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
			RESULT r = R_PASS;

			CBM((m_pendingVideoBuffer.fPendingBufferReady == false), "Buffer already pending");

			//DEBUG_LINEOUT("on video frame");

			m_pendingVideoBuffer.pPendingBuffer = pVideoFrameDataBuffer;
			m_pendingVideoBuffer.pxWidth = pxWidth;
			m_pendingVideoBuffer.pxHeight = pxHeight;
			m_pendingVideoBuffer.fPendingBufferReady = true;

		Error:
			return r;
		}

		// CloudController::UserObserver
		virtual RESULT OnGetSettings(float height, float depth, float scale) override {
			DEBUG_LINEOUT("OnGetSettings");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnSetSettings() override {
			DEBUG_LINEOUT("OnSetSettings");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnLogin() override {
			DEBUG_LINEOUT("OnLogin");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnLogout() override {
			DEBUG_LINEOUT("OnLogout");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
			DEBUG_LINEOUT("OnFormURL");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnAccessToken");

			CBM(fSuccess, "Request of access token failed");

			CRM(pUserController->RequestUserProfile(strAccessToken), "Failed to request user profile");
			
			CRM(pUserController->RequestTwilioNTSInformation(strAccessToken), "Failed to request twilio info");

			CRM(pUserController->GetTeam(strAccessToken), "Failed to request team");

		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnGetToken");

			CB(fSuccess);

			//CRM(pUserController->SetUserDefaultEnvironmentID(environmentId), "Failed to set default environment id");

			// Using environment 170 for testing
			CRM(pUserController->SetUserDefaultEnvironmentID(168), "Failed to set default environment id");

			CRM(pUserController->UpdateLoginState(), "Failed to update login status");

		Error:
			return r;
		};

	} *pTestContext = new TestContext();

	// Initialize the test
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamBrowser> pDreamBrowser = nullptr;
		std::shared_ptr<Dream2DMouseApp> pDream2DMouse = nullptr;

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		//std::string strURL = "http://urlme.me/troll/dream_test/1.jpg";
		std::string strURL = "https://www.youtube.com/watch?v=JzqumbhfxRo&t=27s";
		std::string strTestValue;

		CR(SetupSkyboxPipeline("standard"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		// TODO: Why does shit explode with no objects in scene
		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		strTestValue = pCommandLineManager->GetParameterValue("testval");
		int testUserNumber = atoi(strTestValue.c_str());

		// quad
		// This presents a timing issue if it works 
		pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(3.0f, 3.0f);
		CN(pTestContext->m_pBrowserQuad);
		pTestContext->m_pBrowserQuad->RotateXByDeg(90.0f);
		pTestContext->m_pBrowserQuad->RotateZByDeg(180.0f);

		// Browser
		if (testUserNumber == 2) {
			pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
			CN(pTestContext->m_pWebBrowserManager);
			CR(pTestContext->m_pWebBrowserManager->Initialize());

			// Create the Shared View App
			pTestContext->m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
			pTestContext->m_pDreamBrowser->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
			CNM(pTestContext->m_pDreamBrowser, "Failed to create dream browser");
			CRM(pTestContext->m_pDreamBrowser->RegisterObserver(pTestContext), "Failed to register browser observer");

			// Set up the view
			//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
			//pTestContext->m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
			//pTestContext->m_pDreamBrowser->SetDiagonalSize(10.0f);

			pTestContext->m_pDreamBrowser->SetURI(strURL);
		}
		else {
			// temp
			int pxWidth = 500;
			int pxHeight = 500;
			int channels = 4;

			std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

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
			pTestContext->m_pBrowserQuad->SetDiffuseTexture(pTestContext->pQuadTexture);
		}

		// Cloud Controller

		DEBUG_LINEOUT("Initializing Cloud Controller");

		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		CRM(pTestContext->pCloudController->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");
		CRM(pTestContext->pCloudController->RegisterUserObserver(pTestContext), "Failed to register user observer");

		// TODO: All of the login stuff should be pushed into CloudController and consolidated
		CRM(pTestContext->pCloudController->Start(false), "Failed to start cloud controller");

		DEBUG_LINEOUT("Initializing Cloud Controller");

		// WASAPI Capture Sound Client
		pTestContext->pWASAPICaptureClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
		CN(pTestContext->pWASAPICaptureClient);

		CR(pTestContext->pWASAPICaptureClient->RegisterObserver(pTestContext));
		CR(pTestContext->pWASAPICaptureClient->StartCapture());

		// XAudio2 Render / Spatial Sound Client
		pTestContext->pXAudio2AudioClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_XAUDIO2);
		CN(pTestContext->pXAudio2AudioClient);
		{

			point ptPosition = point(-2.0f, 0.0f, -radius);
			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pXAudioSpatialSoundObject1 = pTestContext->pXAudio2AudioClient->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			CN(pTestContext->pXAudioSpatialSoundObject1);

			ptPosition = point(2.0f, 0.0f, -radius);
			vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pXAudioSpatialSoundObject2 = pTestContext->pXAudio2AudioClient->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			CN(pTestContext->pXAudioSpatialSoundObject2);
		}

		CR(pTestContext->pXAudio2AudioClient->StartSpatial());
		CR(pTestContext->pXAudio2AudioClient->StartRender());

		// Log in 
		{
			pTestContext->pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");

			pTestContext->testUserNum = testUserNumber;

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->GetAccessToken(strTestUserRefreshToken), "Failed to request access token");
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
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CloudController *pCloudController = pTestContext->pCloudController;
		CN(pCloudController);

		if (pTestContext->m_pBrowserQuad != nullptr) {

			if (pTestContext->m_pDreamBrowser != nullptr) {
				auto pSourceTexture = pTestContext->m_pDreamBrowser->GetSourceTexture().get();

				pTestContext->m_pBrowserQuad->SetDiffuseTexture(pSourceTexture);

				//GetDOS()->BroadcastSharedVideoFrame((unsigned char*)(pBuffer), width, height);

				// Testing: Memory Leak
				pCloudController->BroadcastTextureFrame(pSourceTexture, 0, PIXEL_FORMAT::RGBA);
			}
			else if (pTestContext->m_pendingVideoBuffer.fPendingBufferReady && pTestContext->m_pendingVideoBuffer.pPendingBuffer != nullptr) {
				
				reinterpret_cast<OGLTexture*>(pTestContext->pQuadTexture)->Resize(pTestContext->m_pendingVideoBuffer.pxWidth, pTestContext->m_pendingVideoBuffer.pxHeight);
			
				// Update the video buffer to texture
			
				// NOTE: Looks like this bad boy is leaking some mems
				CR(pTestContext->pQuadTexture->Update(
					(unsigned char*)(pTestContext->m_pendingVideoBuffer.pPendingBuffer),
					pTestContext->m_pendingVideoBuffer.pxWidth,
					pTestContext->m_pendingVideoBuffer.pxHeight,
					PIXEL_FORMAT::RGBA)
				);
			}
		}

		// Every 20 ms

		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

		/*
		{
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 0) {

				lastUpdateTime = timeNow;

				if (pCloudController != nullptr && pTestContext->testUserNum == 1) {
					// TODO: Retrieve audio packet from capture buffer (might need copy
					// or convert to correct packet format
					//pCaptureBuffer->IncrementBuffer(numFrames);
					//AudioPacket pendingAudioPacket = pCaptureBuffer->GetAudioPacket(numFrames);

					// Send a dummy audio packet (generating audio right now)
					int nChannels = 1;
					int samplingFrequency = 44100;
					int numFrames = (nChannels * samplingFrequency) * 0.01f;
					AudioPacket pendingAudioPacket = AudioPacket(numFrames, 1, 16, nullptr);
					//pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);

					// DO BOTH
					pCloudController->BroadcastAudioPacket(kUserAudioLabel, pendingAudioPacket);
					pCloudController->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket);
				}
			}
		}
		//*/

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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("WebRTC Audio");
	pNewTest->SetTestDescription("Tests the multi-peer audio capabilities of WebRTC using the Dream Sound Client");
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

		virtual RESULT OnNewSocketConnection(int seatPosition) {
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
		std::shared_ptr<DreamShareView>	pDreamShareView = nullptr;

		//std::string strURL = "https://www.w3schools.com/html/html_forms.asp";
		std::string strURL = "http://urlme.me/troll/dream_test/1.jpg";

		CR(SetupSkyboxPipeline("standard"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		CN(m_pDreamOS);

		// Initialize Cloud controller if not already initialized 
		if (m_pDreamOS->GetSandboxConfiguration().fInitCloud == false) {
			DEBUG_LINEOUT("Initializing Cloud Controller");
			CRM(m_pDreamOS->InitializeCloudController(), "Failed to initialize cloud controller");
			//CRM(m_pDreamOS->GetCloudController()->Initialize(), "Failed to initialize cloud controller");
		}

		pTestContext->pCloudController = m_pDreamOS->GetCloudController();
		

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		// Cloud Controller
		CN(pTestContext->pCloudController);

		// Log in 
		{
			std::string strUsername = "test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			if (pCommandLineManager->GetParameterValue("testval") != "1") {
				strURL = "https://www.youtube.com/watch?v=5vZ4lCKv1ik";
				//strURL = "https://www.google.com";
			}

			std::string strPassword = "nightmare";

			std::string strOTK = pCommandLineManager->GetParameterValue("otk.id");

			long environmentID = 168;

			//CR(pCommandLineManager->SetParameterValue("environment", std::to_string(6)));
			CR(pCommandLineManager->SetParameterValue("environment", std::to_string(environmentID)));

			//CRM(pTestContext->pCloudController->LoginUser(strUsername, strPassword, strOTK), "Failed to log in");

			CRM(pTestContext->pCloudController->Start(strUsername, strPassword, environmentID), "Failed to log in");
		}

		pDreamShareView = m_pDreamOS->LaunchDreamApp<DreamShareView>(this);
		CNM(pDreamShareView, "Failed to create dream share view");

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

		pDreamShareView->Show();

		auto pComposite = m_pDreamOS->AddComposite();
		pComposite->SetPosition(pDreamBrowser->GetOrigin());
		m_pTestQuad = pComposite->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f));
		CN(m_pTestQuad);
		m_pTestQuad->translateX(pDreamBrowser->GetWidthFromAspectDiagonal() + 0.5f + 0.1f);

		m_pPointerCursor = pComposite->AddModel(L"\\mouse-cursor\\mouse-cursor.obj");
		CN(m_pPointerCursor);

		m_pPointerCursor->SetPivotPoint(point(-0.2f, -0.43f, 0.0f));
		m_pPointerCursor->SetScale(0.01f);
		m_pPointerCursor->SetOrientationOffset(vector((float)M_PI_2, 0.0f, 0.0f));
		m_pPointerCursor->SetMaterialAmbient(1.0f);
		m_pPointerCursor->SetVisible(false);

		m_pDreamOS->AddObjectToInteractionGraph(pComposite);

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

RESULT WebRTCTestSuite::HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	//*
	switch (pEvent->m_eventType) {
		case ELEMENT_INTERSECT_BEGAN: {
	//		m_fTestQuadActive = true;
		} break;

		case ELEMENT_INTERSECT_MOVED: {
			// empty
		} break;

		case ELEMENT_INTERSECT_ENDED: {
	//		m_fTestQuadActive = false;
		} break;

		case INTERACTION_EVENT_SELECT_DOWN: {
/*
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

	//*/
		} break;
	}
	CR(r);

Error:
	return r;
}
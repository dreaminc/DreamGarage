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
	DreamTestSuite("webrtc", pDreamOS)
{
	// empty
}

WebRTCTestSuite::~WebRTCTestSuite() {
	// empty
}

RESULT WebRTCTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestWebRTCVCamAudioRelay());

	CR(AddTestWebRTCAudio());

	CR(AddTestWebRTCMultiPeer());

	CR(AddTestWebRTCVideoStream());

	CR(AddTestChromeMultiBrowser());

	// TODO: Need a data channel test

Error:
	return r;
}

CloudController *WebRTCTestSuite::GetCloudController() {
	return m_pDreamOS->GetCloudController();
}

OGLProgram *g_pRenderProg = nullptr;

RESULT WebRTCTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	// empty

Error:
	return r;
}

RESULT WebRTCTestSuite::SetupPipeline(std::string strRenderShaderName) {
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

	struct TestContext : 
		public CloudController::PeerConnectionObserver, 
		public CloudController::UserObserver
	{
		CloudController *pCloudController = nullptr;
		UserController *pUserController = nullptr;
		bool fExitTest = false;

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

		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			//DEVENV_LINEOUT(L"OnVideoFrame: %s", strVideoTrackLabel.c_str());

			return R_NOT_HANDLED;
		}

		// CloudController::UserObserver
		virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override {
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

		virtual RESULT OnPendLogout() override {
			DEBUG_LINEOUT("OnPendLogout");

			fExitTest = true;

			return R_NOT_HANDLED;
		}

		virtual RESULT OnSwitchTeams() override {
			DEBUG_LINEOUT("OnSwitchTeams");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
			DEBUG_LINEOUT("OnFormURL");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDreamVersion(version dreamVersion) override {
			DEBUG_LINEOUT("OnDreamVersion");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAPIConnectionCheck(bool fIsConnected) override {
			DEBUG_LINEOUT("OnAPIConnectionCheck");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnAccessToken");

			CBM(fSuccess, "Request of access token failed");

			CRM(pUserController->RequestUserProfile(strAccessToken), "Failed to request user profile");

			CRM(pUserController->RequestTwilioNTSInformation(strAccessToken), "Failed to request twilio info");

			CRM(pUserController->RequestTeam(strAccessToken), "Failed to request team");

		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override {
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
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::string strTestValue;

		DOSLOG(INFO, "[WebRTCTestingSuite] Multipeer Test Initializing ... ");

		CR(SetupPipeline("blinnphong"));

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

		strTestValue = pCommandLineManager->GetParameterValue("testval");
		int testUserNumber = atoi(strTestValue.c_str());

		// Cloud Controller
		DEBUG_LINEOUT("Initializing Cloud Controller");
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		CRM(pTestContext->pCloudController->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");
		CRM(pTestContext->pCloudController->RegisterUserObserver(pTestContext), "Failed to register user observer");

		CRM(pTestContext->pCloudController->Start(), "Failed to start cloud controller");

		// Log in 
		{
			pTestContext->pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");

			pTestContext->testUserNum = testUserNumber;

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->RequestAccessToken(strTestUserRefreshToken), "Failed to request access token");
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

		// TODO: Should have a way to kill the test that's not an error code
		if (pTestContext->fExitTest) {
			return R_FAIL;
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

RESULT WebRTCTestSuite::AddTestWebRTCVCamAudioRelay() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;
	float radius = 1.0f;

	struct TestContext :
		public DreamSoundSystem::observer,
		public CloudController::PeerConnectionObserver,
		public CloudController::UserObserver,
		public DreamBrowserObserver
	{
		DreamOS *pDreamOS = nullptr;

		CloudController *pCloudController = nullptr;
		UserController *pUserController = nullptr;

		SoundClient *pWASAPICaptureClient = nullptr;
		SoundClient *pXAudio2AudioClient = nullptr;

		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;

		int testUserNum = 0;

		sphere *pSphere = nullptr;


		quad *m_pBrowserQuad = nullptr;
		texture *pQuadTexture = nullptr;


		struct PendingVideoBuffer {
			uint8_t *pPendingBuffer = nullptr;
			int pxWidth = 0;
			int pxHeight = 0;
			bool fPendingBufferReady = false;
		} m_pendingVideoBuffer;

		uint8_t *pTestVideoFrameBuffer = nullptr;

		// DreamSoundSystem::observer
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

		Error:
			return r;
		}

		// DreamBrowserObserver
		virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override {
			RESULT r = R_PASS;

			if (pCloudController != nullptr && testUserNum == 2) {
				CR(pCloudController->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket));

				int numFrames = pendingAudioPacket.GetNumFrames();
				CRM(pDreamOS->PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET::LOCAL_BROWSER_0, numFrames, pendingAudioPacket), "Failed to push packet to sound system");
			}

		Error:
			return r;
		}

		virtual RESULT UpdateControlBarText(std::string& strTitle) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateAddressBarSecurity(bool fSecure) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateAddressBarText(std::string& strURL) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleIsInputFocused(bool fIsInputFocused, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSuccess() override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormCancel() override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) override { return R_NOT_HANDLED; }
		virtual RESULT HandleCanTabNext(bool fCanNext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleCanTabPrevious(bool fCanPrevious) override { return R_NOT_HANDLED; }
		virtual RESULT HandleLoadEnd() override { return R_NOT_HANDLED; }
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

			CN(pDreamOS);

			if (strAudioTrackLabel == kUserAudioLabel) {

				AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, sound::type::SIGNED_16_BIT, (uint8_t*)pAudioDataBuffer);
				CR(pDreamOS->GetDreamSoundSystem()->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, 1));

				CRM(pDreamOS->PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET::PEER_1, (int)frames, pendingPacket), "Failed to push packet to sound system");
			}
			else if (strAudioTrackLabel == kChromeAudioLabel) {

				AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, sound::type::SIGNED_16_BIT, (uint8_t*)pAudioDataBuffer);
				CR(pDreamOS->GetDreamSoundSystem()->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, 0));

				CRM(pDreamOS->PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET::REMOTE_BROWSER_MONO_0, (int)frames, pendingPacket), "Failed to push packet to sound system");
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

		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			RESULT r = R_PASS;

			CBM((m_pendingVideoBuffer.fPendingBufferReady == false), "Buffer already pending");

			//DEBUG_LINEOUT("on video frame %s", strVideoTrackLabel.c_str());

			m_pendingVideoBuffer.pPendingBuffer = pVideoFrameDataBuffer;
			m_pendingVideoBuffer.pxWidth = pxWidth;
			m_pendingVideoBuffer.pxHeight = pxHeight;
			m_pendingVideoBuffer.fPendingBufferReady = true;

		Error:
			return r;
		}

		// CloudController::UserObserver
		virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override {
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

		virtual RESULT OnPendLogout() override {
			DEBUG_LINEOUT("OnPendLogout");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnSwitchTeams() override {
			DEBUG_LINEOUT("OnSwitchTeams");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
			DEBUG_LINEOUT("OnFormURL");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDreamVersion(version dreamVersion) override {
			DEBUG_LINEOUT("OnDreamVersion");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAPIConnectionCheck(bool fIsConnected) override {
			DEBUG_LINEOUT("OnAPIConnectionCheck");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnAccessToken");

			CBM(fSuccess, "Request of access token failed");

			CRM(pUserController->RequestUserProfile(strAccessToken), "Failed to request user profile");

			CRM(pUserController->RequestTwilioNTSInformation(strAccessToken), "Failed to request twilio info");

			CRM(pUserController->RequestTeam(strAccessToken), "Failed to request team");

		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override {
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

		virtual std::string GetCertificateErrorURL() override {
			return "";
		}

		virtual std::string GetLoadErrorURL() override {
			return "";
		}

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

		CR(SetupPipeline("standard"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		pTestContext->pDreamOS = m_pDreamOS;

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		// TODO: Why does shit explode with no objects in scene
		//auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		strTestValue = pCommandLineManager->GetParameterValue("testval");
		int testUserNumber = atoi(strTestValue.c_str());

		// quad
		// This presents a timing issue if it works 
		pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(3.0f, 3.0f);
		CN(pTestContext->m_pBrowserQuad);
		pTestContext->m_pBrowserQuad->FlipUVHorizontal();
		pTestContext->m_pBrowserQuad->RotateXByDeg(90.0f);
		pTestContext->m_pBrowserQuad->RotateZByDeg(180.0f);

		// Browser
		if (testUserNumber == 2) {
			///* Dream Browser
			pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
			CN(pTestContext->m_pWebBrowserManager);
			CR(pTestContext->m_pWebBrowserManager->Initialize());

			// Create the Shared View App
			pTestContext->m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
			pTestContext->m_pDreamBrowser->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
			CNM(pTestContext->m_pDreamBrowser, "Failed to create dream browser");
			CRM(pTestContext->m_pDreamBrowser->RegisterObserver(pTestContext), "Failed to register browser observer");
			pTestContext->m_pDreamBrowser->SetForceObserverAudio(true);

			// Set up the view
			//pDreamBrowser->SetParams(point(0.0f), 5.0f, 1.0f, vector(0.0f, 0.0f, 1.0f));
			//pTestContext->m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
			//pTestContext->m_pDreamBrowser->SetDiagonalSize(10.0f);

			pTestContext->m_pDreamBrowser->SetURI(strURL);
			//*/
		}
		else {
			// temp
			int pxWidth = 500;
			int pxHeight = 500;
			int channels = 4;

			std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

			pTestContext->pQuadTexture = m_pDreamOS->MakeTexture(
				texture::type::TEXTURE_2D,
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

		CR(m_pDreamOS->UnregisterSoundSystemObserver());
		CR(m_pDreamOS->RegisterSoundSystemObserver(pTestContext));

		// Log in 
		{
			pTestContext->pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");

			pTestContext->testUserNum = testUserNumber;

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->RequestAccessToken(strTestUserRefreshToken), "Failed to request access token");
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
				auto pSourceTexture = pTestContext->m_pDreamBrowser->GetSourceTexture();

				pTestContext->m_pBrowserQuad->SetDiffuseTexture(pSourceTexture);

				//GetDOS()->BroadcastSharedVideoFrame((unsigned char*)(pBuffer), width, height);

				// Testing: Memory Leak
				pCloudController->BroadcastTextureFrame(kChromeVideoLabel, pSourceTexture, 0, PIXEL_FORMAT::RGBA);
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

RESULT WebRTCTestSuite::AddTestWebRTCAudio() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;
	float radius = 1.0f;

	struct TestContext : 
		public DreamSoundSystem::observer, 
		public CloudController::PeerConnectionObserver, 
		public CloudController::UserObserver,
		public DreamBrowserObserver
	{
		DreamOS *pDreamOS = nullptr;

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

		// DreamSoundSystem::observer
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

		Error:
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
		virtual RESULT UpdateAddressBarSecurity(bool fSecure) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateAddressBarText(std::string& strURL) override { return R_NOT_HANDLED; }
		virtual RESULT UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleIsInputFocused(bool fIsInputFocused, DreamContentSource *pContext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSuccess() override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormCancel() override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) override { return R_NOT_HANDLED; }
		virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) override { return R_NOT_HANDLED; }
		virtual RESULT HandleCanTabNext(bool fCanNext) override { return R_NOT_HANDLED; }
		virtual RESULT HandleCanTabPrevious(bool fCanPrevious) override { return R_NOT_HANDLED; }
		virtual RESULT HandleLoadEnd() override { return R_NOT_HANDLED; }
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

			CN(pDreamOS);

			if (strAudioTrackLabel == kUserAudioLabel) {

				AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, (uint8_t*)pAudioDataBuffer);
				CR(pDreamOS->GetDreamSoundSystem()->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, 1));

				//if (pXAudioSpatialSoundObject1 != nullptr) {
				//	// Do I need to copy the buffer over (getting over written maybe)
				//	int16_t *pInt16Soundbuffer = new int16_t[frames];
				//	memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * frames);
				//
				//	if (pInt16Soundbuffer != nullptr) {
				//		CR(pXAudioSpatialSoundObject1->PushMonoAudioBuffer((int)frames, pInt16Soundbuffer));
				//	}
				//}
			}
			else if (strAudioTrackLabel == kChromeAudioLabel) {
				
				AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, (uint8_t*)pAudioDataBuffer);
				CR(pDreamOS->GetDreamSoundSystem()->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, 0));

				//if (pXAudioSpatialSoundObject1 != nullptr) {
				//	// Do I need to copy the buffer over (getting over written maybe)
				//	int16_t *pInt16Soundbuffer = new int16_t[frames];
				//	memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * frames);
				//
				//	if (pInt16Soundbuffer != nullptr) {
				//		CR(pXAudioSpatialSoundObject2->PushMonoAudioBuffer((int)frames, pInt16Soundbuffer));
				//	}
				//}
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

		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			RESULT r = R_PASS;

			CBM((m_pendingVideoBuffer.fPendingBufferReady == false), "Buffer already pending");

			//DEBUG_LINEOUT("on video frame %s", strVideoTrackLabel.c_str());

			m_pendingVideoBuffer.pPendingBuffer = pVideoFrameDataBuffer;
			m_pendingVideoBuffer.pxWidth = pxWidth;
			m_pendingVideoBuffer.pxHeight = pxHeight;
			m_pendingVideoBuffer.fPendingBufferReady = true;

		Error:
			return r;
		}

		// CloudController::UserObserver
		virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override {
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

		virtual RESULT OnPendLogout() override {
			DEBUG_LINEOUT("OnPendLogout");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnSwitchTeams() override {
			DEBUG_LINEOUT("OnSwitchTeams");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
			DEBUG_LINEOUT("OnFormURL");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDreamVersion(version dreamVersion) override {
			DEBUG_LINEOUT("OnDreamVersion");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAPIConnectionCheck(bool fIsConnected) override {
			DEBUG_LINEOUT("OnAPIConnectionCheck");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnAccessToken");

			CBM(fSuccess, "Request of access token failed");

			CRM(pUserController->RequestUserProfile(strAccessToken), "Failed to request user profile");
			
			CRM(pUserController->RequestTwilioNTSInformation(strAccessToken), "Failed to request twilio info");

			CRM(pUserController->RequestTeam(strAccessToken), "Failed to request team");

		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override {
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

		virtual std::string GetCertificateErrorURL() override {
			return "";
		}

		virtual std::string GetLoadErrorURL() override {
			return "";
		}

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

		CR(SetupPipeline("standard"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(m_pDreamOS);

		pTestContext->pDreamOS = m_pDreamOS;

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, -0.5f));

		// TODO: Why does shit explode with no objects in scene
		//auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		strTestValue = pCommandLineManager->GetParameterValue("testval");
		int testUserNumber = atoi(strTestValue.c_str());

		// quad
		// This presents a timing issue if it works 
		pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(3.0f, 3.0f);
		CN(pTestContext->m_pBrowserQuad);
		pTestContext->m_pBrowserQuad->FlipUVHorizontal();
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
			pTestContext->m_pDreamBrowser->SetForceObserverAudio(true);

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
				texture::type::TEXTURE_2D,
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

		CR(m_pDreamOS->UnregisterSoundSystemObserver());
		CR(m_pDreamOS->RegisterSoundSystemObserver(pTestContext));

		{
			point ptPosition = point(-2.0f, 0.0f, -radius);
			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pXAudioSpatialSoundObject1 = m_pDreamOS->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			CN(pTestContext->pXAudioSpatialSoundObject1);

			ptPosition = point(2.0f, 0.0f, -radius);
			vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pXAudioSpatialSoundObject2 = m_pDreamOS->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			CN(pTestContext->pXAudioSpatialSoundObject2);
		}

		// Log in 
		{
			pTestContext->pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");

			pTestContext->testUserNum = testUserNumber;

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->RequestAccessToken(strTestUserRefreshToken), "Failed to request access token");
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
				auto pSourceTexture = pTestContext->m_pDreamBrowser->GetSourceTexture();

				pTestContext->m_pBrowserQuad->SetDiffuseTexture(pSourceTexture);

				//GetDOS()->BroadcastSharedVideoFrame((unsigned char*)(pBuffer), width, height);

				// Testing: Memory Leak
				pCloudController->BroadcastTextureFrame(kChromeVideoLabel, pSourceTexture, 0, PIXEL_FORMAT::RGBA);
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

#define UPDATE_SCREENCAST_COUNT 30	
#define UPDATE_SCREENCAST_MS ((1000.0f) / UPDATE_SCREENCAST_COUNT)
std::chrono::system_clock::time_point g_lastTestUpdate = std::chrono::system_clock::now();

RESULT WebRTCTestSuite::AddTestWebRTCVideoStream() {
	RESULT r = R_PASS;

	double sTestTime = 2000.0f;
	int nRepeats = 1;

	struct TestContext : 
		public CloudController::PeerConnectionObserver,
		public CloudController::UserObserver
	{
		UserController *pUserController = nullptr;
		
		// Chrome Source / Dest
		quad *pChromeDestQuad = nullptr;
		texture *pChromeDestQuadTexture = nullptr;
		quad *pChromeSourceQuad = nullptr;
		texture *pChromeSourceTexture = nullptr;

		// VCam Source / Dest
		quad *pVCamDestQuad = nullptr;
		texture *pVCamDestQuadTexture = nullptr;
		quad *pVCamSourceQuad = nullptr;
		texture *pVCamSourceTexture = nullptr;

		CloudController *pCloudController = nullptr;

		bool fExitTest = false;
		int testUserNum = -1;

		struct PendingVideoBuffer {
			uint8_t *pPendingBuffer = nullptr;
			int pxWidth = 0;
			int pxHeight = 0;
			bool fPendingBufferReady = false;
		};

		PendingVideoBuffer m_pendingChromeVideoBuffer;
		PendingVideoBuffer m_pendingVCamVideoBuffer;

		uint8_t *pTestChromeVideoFrameBuffer = nullptr;
		uint8_t *pTestVCamVideoFrameBuffer = nullptr;

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

		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override {
			RESULT r = R_PASS;

			if (strVideoTrackLabel == kChromeVideoLabel) {
				CBM((m_pendingChromeVideoBuffer.fPendingBufferReady == false), "Chrome Buffer already pending");

				m_pendingChromeVideoBuffer.pPendingBuffer = pVideoFrameDataBuffer;
				m_pendingChromeVideoBuffer.pxWidth = pxWidth;
				m_pendingChromeVideoBuffer.pxHeight = pxHeight;
				m_pendingChromeVideoBuffer.fPendingBufferReady = true;
			}
			else if (strVideoTrackLabel == kVCamVideoLabel) {
				CBM((m_pendingVCamVideoBuffer.fPendingBufferReady == false), "VCam Buffer already pending");

				m_pendingVCamVideoBuffer.pPendingBuffer = pVideoFrameDataBuffer;
				m_pendingVCamVideoBuffer.pxWidth = pxWidth;
				m_pendingVCamVideoBuffer.pxHeight = pxHeight;
				m_pendingVCamVideoBuffer.fPendingBufferReady = true;
			}

		Error:
			return r;
		}

		// CloudController::UserObserver
		virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override {
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

		virtual RESULT OnPendLogout() override {
			DEBUG_LINEOUT("OnPendLogout");

			fExitTest = true;

			return R_NOT_HANDLED;
		}

		virtual RESULT OnSwitchTeams() override {
			DEBUG_LINEOUT("OnSwitchTeams");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
			DEBUG_LINEOUT("OnFormURL");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnDreamVersion(version dreamVersion) override {
			DEBUG_LINEOUT("OnDreamVersion");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAPIConnectionCheck(bool fIsConnected) override {
			DEBUG_LINEOUT("OnAPIConnectionCheck");

			return R_NOT_HANDLED;
		}

		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnAccessToken");

			CBM(fSuccess, "Request of access token failed");

			CRM(pUserController->RequestUserProfile(strAccessToken), "Failed to request user profile");

			CRM(pUserController->RequestTwilioNTSInformation(strAccessToken), "Failed to request twilio info");

			CRM(pUserController->RequestTeam(strAccessToken), "Failed to request team");

		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override {
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
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// temp
		int pxWidth = 500;
		int pxHeight = 500;
		int channels = 4;

		std::string strTestValue;

		std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

		CR(SetupPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		
		pTestContext->pChromeDestQuad = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1);
		CN(pTestContext->pChromeDestQuad);
		pTestContext->pChromeDestQuad->RotateXByDeg(45.0f);
		pTestContext->pChromeDestQuad->translateX(-1.0f);
		pTestContext->pChromeDestQuad->translateY(1.0f);

		pTestContext->pChromeSourceQuad = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1);
		CN(pTestContext->pChromeSourceQuad);
		pTestContext->pChromeSourceQuad->RotateXByDeg(45.0f);
		pTestContext->pChromeSourceQuad->translateX(1.0f);
		pTestContext->pChromeSourceQuad->translateY(1.0f);

		pTestContext->pVCamDestQuad = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1);
		CN(pTestContext->pVCamDestQuad);
		pTestContext->pVCamDestQuad->RotateXByDeg(45.0f);
		pTestContext->pVCamDestQuad->translateX(-1.0f);
		pTestContext->pVCamDestQuad->translateY(-1.0f);

		pTestContext->pVCamSourceQuad = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1);
		CN(pTestContext->pVCamSourceQuad);
		pTestContext->pVCamSourceQuad->RotateXByDeg(45.0f);
		pTestContext->pVCamSourceQuad->translateX(1.0f);
		pTestContext->pVCamSourceQuad->translateY(-1.0f);

		// Temporary
		///*
		// Chrome
		pTestContext->pChromeDestQuadTexture = m_pDreamOS->MakeTexture(
			texture::type::TEXTURE_2D, 
			pxWidth, 
			pxHeight, 
			PIXEL_FORMAT::RGBA,
			4, 
			&vectorByteBuffer[0], 
			pxWidth * pxHeight * 4
		);

		CN(pTestContext->pChromeDestQuadTexture);
		pTestContext->pChromeDestQuad->SetDiffuseTexture(pTestContext->pChromeDestQuadTexture);

		pTestContext->pChromeSourceTexture = m_pDreamOS->MakeTexture(
			texture::type::TEXTURE_2D,
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA,
			4,
			&vectorByteBuffer[0],
			pxWidth * pxHeight * 4
		);

		CN(pTestContext->pChromeSourceTexture);
		pTestContext->pChromeSourceQuad->SetDiffuseTexture(pTestContext->pChromeSourceTexture);

		// VCam
		pTestContext->pVCamDestQuadTexture = m_pDreamOS->MakeTexture(
			texture::type::TEXTURE_2D,
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA,
			4,
			&vectorByteBuffer[0],
			pxWidth * pxHeight * 4
		);

		CN(pTestContext->pVCamDestQuadTexture);
		pTestContext->pVCamDestQuad->SetDiffuseTexture(pTestContext->pVCamDestQuadTexture);

		pTestContext->pVCamSourceTexture = m_pDreamOS->MakeTexture(
			texture::type::TEXTURE_2D,
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA,
			4,
			&vectorByteBuffer[0],
			pxWidth * pxHeight * 4
		);

		CN(pTestContext->pVCamSourceTexture);
		pTestContext->pVCamSourceQuad->SetDiffuseTexture(pTestContext->pVCamSourceTexture);

		size_t bufSize = sizeof(uint8_t) * pxWidth * pxHeight * channels;

		pTestContext->pTestChromeVideoFrameBuffer = (uint8_t*)malloc(bufSize);
		CN(pTestContext->pTestChromeVideoFrameBuffer);

		pTestContext->pTestVCamVideoFrameBuffer = (uint8_t*)malloc(bufSize);
		CN(pTestContext->pTestVCamVideoFrameBuffer);

		int chromeStyleCounter = 0;
		int vcamStyleCounter = 0;

		for (int i = 0; i < pxHeight; i++) {
			for (int j = 0; j < pxWidth; j++) {
				uint8_t cChromePixel[4] = { 0x00, 0x00, 0x00, 0xFF };
				cChromePixel[chromeStyleCounter] = 0xFF;

				uint8_t cVCamPixel[4] = { 0x00, 0x00, 0x00, 0xFF };
				cVCamPixel[vcamStyleCounter] = 0xFF;

				size_t offset = (i * ((pxWidth - 1)) + (j));
				offset *= 4;

				CB((offset < bufSize));

				uint8_t *pPixelMemLocation = pTestContext->pTestChromeVideoFrameBuffer + offset;
				memcpy(pPixelMemLocation, cChromePixel, sizeof(cChromePixel));

				pPixelMemLocation = pTestContext->pTestVCamVideoFrameBuffer + offset;
				memcpy(pPixelMemLocation, cVCamPixel, sizeof(cVCamPixel));
			}

			if (i % 50 == 0) {
				if (++chromeStyleCounter > 3) {
					chromeStyleCounter = 0;
				}

				if (--vcamStyleCounter < 0) {
					vcamStyleCounter = 3;
				}
			}
		}

		CR(pTestContext->pChromeSourceTexture->Update(
			(unsigned char*)(pTestContext->pTestChromeVideoFrameBuffer),
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA)
		);
		CR(pTestContext->pChromeSourceTexture->LoadImageFromTexture(0, PIXEL_FORMAT::BGRA));

		CR(pTestContext->pVCamSourceTexture->Update(
			(unsigned char*)(pTestContext->pTestVCamVideoFrameBuffer),
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA)
		);
		CR(pTestContext->pVCamSourceTexture->LoadImageFromTexture(0, PIXEL_FORMAT::BGRA));

		//*/

		// Command Line Manager
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		strTestValue = pCommandLineManager->GetParameterValue("testval");
		int testUserNumber = atoi(strTestValue.c_str());

		// Cloud Controller
		DEBUG_LINEOUT("Initializing Cloud Controller");
		pTestContext->pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
		CNM(pTestContext->pCloudController, "Cloud Controller failed to initialize");

		CRM(pTestContext->pCloudController->RegisterPeerConnectionObserver(pTestContext), "Failed to register Peer Connection Observer");
		CRM(pTestContext->pCloudController->RegisterUserObserver(pTestContext), "Failed to register user observer");

		CRM(pTestContext->pCloudController->Start(), "Failed to start cloud controller");

		// Log in 
		{
			pTestContext->pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");

			pTestContext->testUserNum = testUserNumber;

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->RequestAccessToken(strTestUserRefreshToken), "Failed to request access token");
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

		if (pTestContext->m_pendingChromeVideoBuffer.fPendingBufferReady && pTestContext->m_pendingChromeVideoBuffer.pPendingBuffer != nullptr) {
			// Update the video buffer to texture
			CR(pTestContext->pChromeDestQuadTexture->Update(
				(unsigned char*)(pTestContext->m_pendingChromeVideoBuffer.pPendingBuffer),
				pTestContext->m_pendingChromeVideoBuffer.pxWidth,
				pTestContext->m_pendingChromeVideoBuffer.pxHeight,
				PIXEL_FORMAT::RGBA)
			);
		}

		if (pTestContext->m_pendingVCamVideoBuffer.fPendingBufferReady && pTestContext->m_pendingVCamVideoBuffer.pPendingBuffer != nullptr) {
			// Update the video buffer to texture
			CR(pTestContext->pVCamDestQuadTexture->Update(
				(unsigned char*)(pTestContext->m_pendingVCamVideoBuffer.pPendingBuffer),
				pTestContext->m_pendingVCamVideoBuffer.pxWidth,
				pTestContext->m_pendingVCamVideoBuffer.pxHeight,
				PIXEL_FORMAT::RGBA)
			);
		}

		// Scroll/Update the buffers
		int pxWidth = pTestContext->pChromeSourceTexture->GetWidth();
		int pxHeight = pTestContext->pChromeSourceTexture->GetHeight();
		int channels = pTestContext->pChromeSourceTexture->GetChannels();

		size_t bufSize = sizeof(uint8_t) * pxWidth * pxHeight * channels;
		size_t bufRowSize = sizeof(uint8_t) * pxWidth * channels;

		uint8_t *tempRow = (uint8_t*)malloc(bufRowSize);
		CN(tempRow);

		// Chrome Buffer
		// Save the row
		memcpy(tempRow, pTestContext->pTestChromeVideoFrameBuffer, bufRowSize);
		memcpy(pTestContext->pTestChromeVideoFrameBuffer,
			pTestContext->pTestChromeVideoFrameBuffer + bufRowSize,
			(bufSize - bufRowSize));
		memcpy(pTestContext->pTestChromeVideoFrameBuffer + (bufSize - bufRowSize),
			tempRow,
			bufRowSize);

		memcpy(tempRow, pTestContext->pTestVCamVideoFrameBuffer + (bufSize - bufRowSize), bufRowSize);
		memcpy(pTestContext->pTestVCamVideoFrameBuffer + bufRowSize,
			pTestContext->pTestVCamVideoFrameBuffer,
			(bufSize - bufRowSize));
		memcpy(pTestContext->pTestVCamVideoFrameBuffer,
			tempRow,
			bufRowSize);

		CR(pTestContext->pChromeSourceTexture->Update(
			(unsigned char*)(pTestContext->pTestChromeVideoFrameBuffer),
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA)
		);
		CR(pTestContext->pChromeSourceTexture->LoadImageFromTexture(0, PIXEL_FORMAT::BGRA));

		CR(pTestContext->pVCamSourceTexture->Update(
			(unsigned char*)(pTestContext->pTestVCamVideoFrameBuffer),
			pxWidth,
			pxHeight,
			PIXEL_FORMAT::RGBA)
		);
		CR(pTestContext->pVCamSourceTexture->LoadImageFromTexture(0, PIXEL_FORMAT::BGRA));


		if (tempRow != nullptr) {
			free(tempRow);
			tempRow = nullptr;
		}

		// Replace with BroadcastTexture
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - g_lastTestUpdate).count() > UPDATE_SCREENCAST_MS) {
			if (pTestContext->pCloudController != nullptr) {
				pTestContext->pCloudController->BroadcastTextureFrame(kChromeVideoLabel, pTestContext->pChromeSourceTexture, 0, PIXEL_FORMAT::RGBA);

				pTestContext->pCloudController->BroadcastTextureFrame(kVCamVideoLabel, pTestContext->pVCamSourceTexture, 0, PIXEL_FORMAT::RGBA);

				/*
				HALImp *pHAL = m_pDreamOS->GetHALImp();
				Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

				if (g_pRenderProg != nullptr) {
					auto pScreenQuadTexture = g_pRenderProg->GetOGLFramebufferColorTexture();

					if (pScreenQuadTexture != nullptr) {
						pTestContext->pCloudController->BroadcastTextureFrame(pScreenQuadTexture, 0, texture::PixelFormat::RGBA);
					}
				}
				//*/
			}

			

			g_lastTestUpdate = std::chrono::system_clock::now();
		}

	Error:

		// Release Chrome pending buffer
		pTestContext->m_pendingChromeVideoBuffer.fPendingBufferReady = false;
		if (pTestContext->m_pendingChromeVideoBuffer.pPendingBuffer != nullptr) {
			delete pTestContext->m_pendingChromeVideoBuffer.pPendingBuffer;
			pTestContext->m_pendingChromeVideoBuffer.pPendingBuffer = nullptr;
		}

		// Release VCam Pending Buffer
		pTestContext->m_pendingVCamVideoBuffer.fPendingBufferReady = false;
		if (pTestContext->m_pendingVCamVideoBuffer.pPendingBuffer != nullptr) {
			delete pTestContext->m_pendingVCamVideoBuffer.pPendingBuffer;
			pTestContext->m_pendingVCamVideoBuffer.pPendingBuffer = nullptr;
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

		CR(SetupPipeline("standard"));

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
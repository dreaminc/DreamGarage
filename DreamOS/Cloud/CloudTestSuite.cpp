#include "CloudTestSuite.h"
#include "DreamOS.h"

#include "Sandbox/CommandLineManager.h"
#include "Cloud/Menu/MenuNode.h"
#include "Cloud/HTTP/HTTPController.h"

#include "Cloud/CloudControllerFactory.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/opengl/OGLProgram.h"

#include "Cloud/Environment/EnvironmentAsset.h"

#include "DreamGarage/DreamContentView.h"

std::map<int, std::string> k_refreshTokens = {
	{ 0, "NakvA43v1eVBqvvTJuqUdXHWL02CNuDqrgHMEBrIY6P5FoHZ2GtgbCVDYvHMaRTw" },
	{ 1, "daehZbIcTcXaPh29tWQy75ZYSLrRL4prhBoBYMRQtU48NMs6svnt5CkzCA5RLKJq" },
	{ 2, "GckLS9Q691PO6RmdmwRp368JjWaETNOMEoASqQF0TCnImHzpmOv2Rch1RDrgr2V7" },
	{ 3, "HYlowX58aRPRB85IT0M2wB20RC8rd0zpOxfIIvEgMF9XVzzFbL8UzY3yyCovdEIQ" },
	{ 4, "sROmFa73UM38v7snrTaDy3JF1vCJGdJhBBLvBcCLaWxjoEYVfAqcgMAZPVHzaZrR" },
	{ 5, "gc2EPtlKmKtkmiZC6cRfUtMIHwiWW9Tf55wbFBcq45Wg8DBRDWV3iZiLsqBedfqF" },
	{ 6, "F5EwwHxmgf4pqLXZjP6zWH4NBn42UtLQUmrlU4vl62BGeprnug0Hn1WeMm3snHQa" },
	{ 7, "cuX1beJjJE58DdU4cYOrsIoNFil534fOWscH9bzmhmcFkV1qn3M8zPkdW7J3UEH1" },
	{ 8, "B3Wwz6Lbwfj2emo7caKBQXtKoMYXR9P70eOvkFzFIfh9NRlal6PLFqIagTFXiDHy" },
	{ 9, "HPfaNfjFrAhlbqS9DuZD5dCrAzI215ETDTRzFMVXrtoYrI2A9XBS3VEKOjGlDSVE" }
};

RESULT CloudTestSuite::SetupSkyboxPipeline(std::string strRenderShaderName) {
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

CloudTestSuite::CloudTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

CloudTestSuite::~CloudTestSuite() {
	// empty
}

RESULT CloudTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestSwitchingEnvironmentSockets());

	// TODO: Closed box testing (multi user/environment instances or cloud controllers if need be)
	CR(AddTestMultiConnectTest());

	// Requires login

	CR(AddTestDownloadFile());	 

	CR(AddTestConnectLogin());
	
	CR(AddTestMenuAPI());

	// TODO: Add Websocket tests
	// TODO: Add HTTP / CURL tests


Error:
	return r;
}


RESULT CloudTestSuite::AddTestSwitchingEnvironmentSockets() {
	RESULT r = R_PASS;

	double sTestTime = 1000.0f;

	// non-asynchronous methods could be set up for tests that login immediately like this
	struct TestContext :
		public CloudController::UserObserver,
		public CloudController::PeerConnectionObserver
	{
		UserController *pUserController = nullptr;
		CloudController *pCloudController = nullptr;

		int testUserNum = 0;

		int environmentIDs[2] = { 168, 170 };
		int environmentIDs_n = 2;
		int curEnvID = 0;

		virtual RESULT OnGetSettings(float height, float depth, float scale) override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnSetSettings() override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnLogin() override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnLogout() override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnPendLogout() override {
			return R_NOT_HANDLED;
		}

		virtual RESULT OnSwitchTeams() override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
			return R_NOT_IMPLEMENTED;
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

			CRM(pUserController->GetTeam(strAccessToken), "Failed to request team");

		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override {
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnGetToken");

			curEnvID = 0;			
			int envID = environmentIDs[curEnvID];

			CB(fSuccess);
			
			CRM(pUserController->SetUserDefaultEnvironmentID(envID), "Failed to set environment id %d", envID);

			//CRM(pUserController->SetUserDefaultEnvironmentID(environmentId), "Failed to set environment id %d", environmentId);

			CRM(pUserController->UpdateLoginState(), "Failed to update login status");

		Error:
			return r;
		};

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
			DEVENV_LINEOUT("OnVideoFrame");

			return R_NOT_HANDLED;
		}

	} *pTestContext = new TestContext();

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		std::string strTestValue;

		DOSLOG(INFO, "[WebRTCTestingSuite] Multipeer Test Initializing ... ");

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CR(SetupSkyboxPipeline("blinnphong"));

		{

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

			// Log in 
			
			pTestContext->pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");

			pTestContext->testUserNum = testUserNumber;

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->GetAccessToken(strTestUserRefreshToken), "Failed to request access token");
			
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		{

			CloudController *pCloudController = pTestContext->pCloudController;
			CN(pCloudController);

			// Every 20 ms

			static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 1500) {

				lastUpdateTime = timeNow;

				if (pCloudController != nullptr) {
					EnvironmentController *pEnvironmentController = dynamic_cast<EnvironmentController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
					UserController *pUserController = dynamic_cast<UserController*>(pTestContext->pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
					auto currentUser = pUserController->GetUser();

					// Increment through teams
					pTestContext->curEnvID += 1;
					if (pTestContext->curEnvID >= pTestContext->environmentIDs_n)
						pTestContext->curEnvID = 0;

					int envID = pTestContext->environmentIDs[pTestContext->curEnvID];

					// blech
					pUserController->SetUserDefaultEnvironmentID(envID);

					// First off disconnect 
					DEBUG_LINEOUT("Disconnecting from environment socket");
					CR(pEnvironmentController->DisconnectFromEnvironmentSocket());

					//// Reconnect 
					CR(pEnvironmentController->ConnectToEnvironmentSocket(currentUser, envID));
				}
			}
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

	// Reset Code 
	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Test Switching Environment Sockets");
	pNewTest->SetTestDescription("Test switching between environment sockets");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CloudTestSuite::AddTestMultiConnectTest() {
	RESULT r = R_PASS;

	double sTestTime = 200.0f;

	// non-asynchronous methods could be set up for tests that login immediately like this
	struct TestContext : 
		public CloudController::UserObserver 
	{
		UserController *pUserController = nullptr;

		virtual RESULT OnGetSettings(float height, float depth, float scale) override { 
			return R_NOT_IMPLEMENTED; 
		}
		
		virtual RESULT OnSetSettings() override { 
			return R_NOT_IMPLEMENTED; 
		}
		
		virtual RESULT OnLogin() override { 
			return R_NOT_IMPLEMENTED; 
		}
		
		virtual RESULT OnLogout() override { 
			return R_NOT_IMPLEMENTED; 
		}

		virtual RESULT OnPendLogout() override {
			return R_NOT_HANDLED;
		}
		
		virtual RESULT OnSwitchTeams() override {
			return R_NOT_IMPLEMENTED;
		}

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override { 
			return R_NOT_IMPLEMENTED; 
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
			CRM(pUserController->GetTeam(strAccessToken), "Failed to request team");
			
		Error:
			return r;
		};

		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override { 
			RESULT r = R_PASS;

			DEBUG_LINEOUT("OnGetToken");

			CB(fSuccess); 
			
			CRM(pUserController->SetUserDefaultEnvironmentID(environmentId), "Failed to set default environment id");
			CRM(pUserController->UpdateLoginState(), "Failed to update login status");

		Error:
			return r;
		};

	} *pTestContext = new TestContext();

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		
		{

			DEBUG_LINEOUT("Initializing Cloud Controller");
			//CRM(pCloudController->Initialize(), "Failed to initialize cloud controller");

			CRM(m_pDreamOS->InitializeCloudController(), "Failed to initialize DreamOS cloud controller");
			CRM(m_pDreamOS->GetCloudController()->RegisterUserObserver(pTestContext), "Failed to register user observer");

			// Cloud Controller
			CloudController *pCloudController = m_pDreamOS->GetCloudController();
			CommandLineManager *pCommandLineManager = CommandLineManager::instance();

			CN(pCloudController);
			CN(pCommandLineManager);

			// For later
			m_pCloudController = pCloudController;

			CR(SetupPipeline());

			// user controller is set up during initialize
		
			// Objects 
			light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

			auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pSphere);


			pTestContext->pUserController = dynamic_cast<UserController*>(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
			CNM(pTestContext->pUserController, "Failed to acquire User Controller Proxy");
			
			std::string strTestValue = pCommandLineManager->GetParameterValue("testval");
			int testUserNumber = atoi(strTestValue.c_str());

			// m_tokens stores the refresh token of users test0-9,
			// so use -t 0 to login as test0@dreamos.com
			std::string strTestUserRefreshToken = CloudTestSuite::GetTestUserRefreshToken(testUserNumber);
			CRM(pTestContext->pUserController->GetAccessToken(strTestUserRefreshToken), "Failed to request access token");

			m_pUserController = pTestContext->pUserController;

		}

		// Log in 
		{
			// TODO: This way to start the cloud controller thread is not great
			/*
			std::string strUsername = "jason_test";
			strUsername += pCommandLineManager->GetParameterValue("testval");
			strUsername += "@dreamos.com";

			CR(pCommandLineManager->SetParameterValue("username", strUsername));
			CR(pCommandLineManager->SetParameterValue("password", "nightmare"));

			CRM(pCloudController->Start(), "Failed to start cloud controller");
			//*/
			
		}

	Error:
		return R_PASS;
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

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service - this will hang for a while");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}


RESULT CloudTestSuite::AddTestDownloadFile() {
	RESULT r = R_PASS;

	double sTestTime = 20.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		//std::string strImagePlaceholderURI = "http://placehold.it/300.png/09f/fff";
		std::string strImagePlaceholderURI = "https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png";

		// Destination Path
		std::wstring strImageDest;
		PathManager* pPathManager = PathManager::instance();
		strImageDest = pPathManager->GetFilePath(PATH_VALUE_TYPE::PATH_DATA, L"testimg.png");

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		HTTPControllerProxy *pHTTPControllerProxy = nullptr;
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCloudController);
		CN(pCommandLineManager);

		// For later
		m_pCloudController = pCloudController;

		// Set up file request
		DEBUG_LINEOUT("Requesting File %s", strImagePlaceholderURI.c_str());
		pHTTPControllerProxy = (HTTPControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP));
		CNM(pHTTPControllerProxy, "Failed to get http controller proxy");

		CR(pHTTPControllerProxy->RequestFile(strImagePlaceholderURI, strImageDest));

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");

		/*
		CBM(pCloudController->IsUserLoggedIn(), "User was not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket did not connect");
		*/

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Download File");
	pNewTest->SetTestDescription("Test downloading a file from arbitrary URL");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CloudTestSuite::AddTestConnectLogin() {
	RESULT r = R_PASS;

	double sTestTime = 30.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCloudController);
		CN(pCommandLineManager);

		CR(SetupPipeline());

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

	Error:
		return R_PASS;
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

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CloudTestSuite::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	//https://api.develop.dreamos.com/environment-asset/{id}/file
	if (m_pDreamContentView != nullptr) {
		//CR(m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset));
		m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset);
	}

//Error:
	return r;
}

RESULT CloudTestSuite::SetupPipeline() {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode;
	pRenderProgramNode = pHAL->MakeProgramNode("minimal");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Screen Quad Shader (opt - we could replace this if we need to)
	ProgramNode *pRenderScreenQuad;
	pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Connect Program to Display

	// Connected in parallel (order matters)
	// NOTE: Right now this won't work with mixing for example
	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT CloudTestSuite::LaunchDreamView() {
	RESULT r = R_PASS;

	CN(m_pDreamOS);

	// Create the Shared View App
	m_pDreamContentView = m_pDreamOS->LaunchDreamApp<DreamContentView>(this);
	CNM(m_pDreamContentView, "Failed to create dream content view");

	// Set up the view
	m_pDreamContentView->SetParams(point(0.0f), 5.0f, DreamContentView::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));
	//m_pDreamContentView->SetFitTextureAspectRatio(true);

	//pDreamContentView->SetScreenTexture(L"crate_color.png");
	//pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
	//m_pDreamContentView->SetScreenURI("https://static.dreamos.com/www/image/hero.387eddfc05dc.jpg");

Error:
	return r;
}

RESULT CloudTestSuite::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	CR(pMenuNode->PrintMenuNode());

	if (pMenuNode->NumSubMenuNodes() > 0) {
		auto pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		std::shared_ptr<MenuNode> pFirstSubMenuNode = pMenuNode->GetSubMenuNodes()[0];
		CN(pFirstSubMenuNode);

		std::string strScope = pFirstSubMenuNode->GetScope();
		std::string strPath = pFirstSubMenuNode->GetPath();
		std::string strTitle = pFirstSubMenuNode->GetTitle();

		if (pFirstSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
			CRM(pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle), "Failed to request sub menu");
		}
		else if (pFirstSubMenuNode->GetNodeType() == MenuNode::type::FILE) {
			auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
			CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");

			CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");
		}
	}

Error:
	return r;
}

RESULT CloudTestSuite::AddTestMenuAPI() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;

	// Initialize the test
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		MenuControllerProxy *pMenuControllerProxy = nullptr;
		CN(pCloudController);
		CBM(pCloudController->IsUserLoggedIn(), "User not logged in");
		CBM(pCloudController->IsEnvironmentConnected(), "Environment socket not connected");

		//CR(pCloudController->RegisterEnvironmentAssetCallback(std::bind(&CloudTestSuite::HandleOnEnvironmentAsset, this, std::placeholders::_1)));
		CR(pCloudController->RegisterEnvironmentObserver(this));

		CR(LaunchDreamView());
		
		// Set up menu stuff
		DEBUG_LINEOUT("Requesting Menu");
		pMenuControllerProxy = (MenuControllerProxy*)(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");

		CRM(pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");
		CRM(pMenuControllerProxy->RequestSubMenu("", "", "menu"), "Failed to request sub menu");

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// Cloud Controller
		CloudController *pCloudController = reinterpret_cast<CloudController*>(pContext);
		CN(pCloudController);

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnTest, GetCloudController());
	CN(pNewTest);

	pNewTest->SetTestName("Test Menu API");
	pNewTest->SetTestDescription("Test Menu API");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

CloudController *CloudTestSuite::GetCloudController() {
	return m_pDreamOS->GetCloudController();
}

#include "DreamTestClient.h"

#include "logger/DreamLogger.h"

#include <string>

#include "Test/TestSuiteFactory.h"

#include "Sandbox/CommandLineManager.h"

// TODO make it possible to have different Dream Applications, then split the TESTING code into a new app
//#define TESTING

cylinder *g_pCylinder = nullptr;
volume *g_pVolume = nullptr;
volume *g_pVolume2 = nullptr;
sphere *pSphere5 = nullptr;

DreamTestClient::DreamTestClient() {
	// empty
}

DreamTestClient::~DreamTestClient() {
	// empty
}

RESULT DreamTestClient::ConfigureSandbox() {
	RESULT r = R_PASS;

	Sandbox::configuration sandboxconfig;
	
	sandboxconfig.fUseHMD = false;
	sandboxconfig.fUseLeap = false;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = true;
	sandboxconfig.fInitCloud = false;		// TODO: This is currently breaking stuff
	sandboxconfig.fInitSound = false;
	sandboxconfig.fInitUserApp = false;		// Turn on for testing User app related functionality 
	sandboxconfig.fInitNamedPipe = true;
	sandboxconfig.fInitKeyboard = false;

	sandboxconfig.fHMDMirror = true;
	sandboxconfig.fHideWindow = false;
	sandboxconfig.f3rdPersonCamera = false;

	sandboxconfig.hmdType = HMD_ANY_AVAILABLE;

	SetSandboxConfiguration(sandboxconfig);

	// Set up API routes
	// Set up command line manager
	auto pCommandLineManager = CommandLineManager::instance();
	CN(pCommandLineManager);

	// Test CLI selector
	CR(pCommandLineManager->RegisterParameter("teststring", "ts", "dreamos."));
	CR(pCommandLineManager->RegisterParameter("testval", "t", "1"));

	// previous AWS server
	// CR(m_pCommandLineManager->RegisterParameter("api.ip", "api.ip", "http://ec2-54-175-210-194.compute-1.amazonaws.com:8000"));
	// CR(m_pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "ws://ec2-54-175-210-194.compute-1.amazonaws.com:8000"));

	// TODO: Since DreamOS project doesn't get PRODUCTION pre-processors and the OCULUS_PRODUCTION_BUILD one is supposed to be temporary
	//		 This will need to be reworked at that time as well.
#ifdef PRODUCTION_BUILD
	CR(pCommandLineManager->RegisterParameter("www.ip", "www.ip", "https://www.dreamos.com:443"));
	CR(pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.dreamos.com:443"));
	CR(pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "wss://ws.dreamos.com:443"));

	// Disable these in production
	CR(pCommandLineManager->DisableParameter("www.ip"));
	CR(pCommandLineManager->DisableParameter("api.ip"));
	CR(pCommandLineManager->DisableParameter("ws.ip"));
#else
	CR(pCommandLineManager->RegisterParameter("www.ip", "www.ip", "https://www.develop.dreamos.com:443"));
	CR(pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.develop.dreamos.com:443"));

	#ifdef USE_LOCALHOST
		CR(pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "ws://localhost:8000"));
	#else
		CR(pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "wss://ws.develop.dreamos.com:443"));
	#endif
#endif

Error:
	return r;
}

RESULT DreamTestClient::LoadScene() {
	RESULT r = R_PASS;

	std::vector<std::string> testStringValues;

	// IO
	RegisterSubscriber((SenseVirtualKey)('N'), this);

	// Set up the HAL Configuration 
	HALImp::HALConfiguration halconf;
	halconf.fRenderReferenceGeometry = false;
	halconf.fDrawWireframe = false;
	halconf.fRenderProfiler = false;
	SetHALConfiguration(halconf);

	// Set up command line manager
	auto pCommandLineManager = CommandLineManager::instance();
	CN(pCommandLineManager);

	testStringValues = pCommandLineManager->GetParameterValues("teststring");
	CBM((testStringValues.size() == 2), "Test string malformed");

	{
		std::string strTestSuiteName = testStringValues[0];
		std::string strTestName = testStringValues[1];

		// Register Test Suites
		CRM(RegisterTestSuites(), "Failed to register test suites");

		CRM(SelectTest(strTestSuiteName, strTestName), "Failed to select %s test from %s test suite", strTestName.c_str(), strTestSuiteName.c_str());
	}

	// TODO: Kill this yo
	AddSkybox();

Error:
	return r;
}

RESULT DreamTestClient::RegisterTestSuite(std::shared_ptr<TestSuite> pTestSuite) {
	RESULT r = R_PASS;

	CNM(pTestSuite, "Test suite is null");

	CBM((m_registeredTestSuites.find(pTestSuite->GetName()) == m_registeredTestSuites.end()), 
		"%s test suite already registered", pTestSuite->GetName().c_str());

	m_registeredTestSuites[pTestSuite->GetName()] = pTestSuite;

Error:
	return r;
}

RESULT DreamTestClient::RegisterTestSuites() {
	RESULT r = R_PASS;

	m_registeredTestSuites = std::map<std::string, std::shared_ptr<TestSuite>>();

	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::WEBRTC, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::HAL, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::UI, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::OS, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::SOUND, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::DIMENSION, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PHYSICS, this));
	RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PIPELINE, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::COLLISION, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::MULTICONTENT, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::UIVIEW, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::CLOUD, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PHYSICS, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::INTERACTION, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::ANIMATION, this));
	//RegisterTestSuite(TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::SANDBOX, this));

Error:
	return r;
}

RESULT DreamTestClient::SelectTest(std::string strTestSuiteName, std::string strTestName) {
	RESULT r = R_PASS;

	CBM((m_registeredTestSuites.find(strTestSuiteName) != m_registeredTestSuites.end()),
		"%s test suite not registered", strTestSuiteName.c_str());

	m_pCurrentTestSuite = m_registeredTestSuites[strTestSuiteName];

	CRM(m_pCurrentTestSuite->SelectTest(strTestName), "Failed to select %s test", strTestName.c_str());

Error:
	return r;
}

RESULT DreamTestClient::Update(void) {
	RESULT r = R_PASS;

	CNM(m_pCurrentTestSuite, "No test suite selected");

	CR(m_pCurrentTestSuite->UpdateAndRunTests((void*)(this)));

Error:
	return r;
}

version DreamTestClient::GetDreamVersion() {
	return version("0.0.0");		// This is... a placeholder?
}

RESULT DreamTestClient::MakePipeline(CameraNode* pCamera, OGLProgram* &pRenderNode, OGLProgram* &pEndNode, Sandbox::PipelineType pipelineType) {
	return R_NOT_IMPLEMENTED;
}

// Cloud
RESULT DreamTestClient::OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::OnNewDreamPeer(DreamPeerApp *pDreamPeer) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	CR(r);

	// Uncomment the below for use with Dream Peer testing (should iron this out in general)

	//// Create a new peer
	//auto pDreamPeer = CreateNewPeer(pPeerConnection);
	//CN(pDreamPeer);
	//
	//CR(pDreamPeer->RegisterDreamPeerObserver(this));

Error:
	return r;
}

RESULT DreamTestClient::OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::OnNewSocketConnection(int seatPosition) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::SaveCameraSettings(point ptPosition, quaternion qOrientation) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestClient::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	switch (kbEvent->KeyCode) {
		case (SenseVirtualKey)('N') : {
			if (kbEvent->KeyState != 0) {
				//HUD_OUT("Key 'N' is pressed - next test");
				m_pCurrentTestSuite->NextTest();
			}
		} break;
	}

	//Error:
	return r;
}

RESULT DreamTestClient::Notify(CollisionObjectEvent *oEvent) {
	RESULT r = R_PASS;

	dynamic_cast<DimObj*>(oEvent->m_pCollisionObject)->SetVertexColor(color(COLOR_RED));

	for (auto &pObj : oEvent->m_collisionGroup) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);
		pDimObj->SetVertexColor(color(COLOR_PINK));
	}

//Error:
	return r;
}

#include "DreamLogger/DreamLogger.h"
#include "DreamTestApp.h"
#include <string>

#include "Test/TestSuiteFactory.h"

// TODO make it possible to have different Dream Applications, then split the TESTING code into a new app
//#define TESTING

cylinder *g_pCylinder = nullptr;
volume *g_pVolume = nullptr;
volume *g_pVolume2 = nullptr;
sphere *pSphere5 = nullptr;

RESULT DreamTestApp::ConfigureSandbox() {
	RESULT r = R_PASS;

	SandboxApp::configuration sandboxconfig;
	sandboxconfig.fUseHMD = true;
	sandboxconfig.fUseLeap = false;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = true;
	sandboxconfig.fInitCloud = false;		// TODO: This is currently breaking stuff
	SetSandboxConfiguration(sandboxconfig);

//Error:
	return r;
}

RESULT DreamTestApp::LoadScene() {
	RESULT r = R_PASS;

	// IO
	RegisterSubscriber((SenseVirtualKey)('N'), this);

	// Set up the HAL Configuration as needed
	///*
	HALImp::HALConfiguration halconf;
	halconf.fRenderReferenceGeometry = false;
	halconf.fDrawWireframe = false;
	halconf.fRenderProfiler = false;
	SetHALConfiguration(halconf);
	//*/

	// Push to a test suite factory in testing

	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PHYSICS, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::COLLISION, this);
	m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::SOUND, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::WEBRTC, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::MULTICONTENT, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::HAL, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::UIVIEW, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::OS, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::UI, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::CLOUD, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PHYSICS, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::INTERACTION, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::ANIMATION, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::SANDBOX, this);

	CN(m_pTestSuite);

	AddSkybox();

	//light *pLight = AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));
	//pLight->EnableShadows();

	/*
	quad *pBQuad = AddQuad(10.0f, 20.0f, 200, 200);// , pHeightTextureCobble);
	pBQuad->MoveTo(point(0.0f, -1.0f, 0.0f));

	sphere *pSphere = AddSphere(0.5f, 10, 10, color(COLOR_RED));
	pSphere->MoveTo(1.5f, 0.5f, 0.0f);

	volume *pVolume = AddVolume(0.5f, false);
	pVolume->MoveTo(-1.5f, 0.5f, 0.0f);
	pVolume->SetWireframe(true);
	*/

	/*
	pComposite = AddModel(L"\\Models\\Boar\\boar-obj.obj", nullptr, point(0.0f, 0.0f, 0.0f), 0.15f, vector(0.0f, 0.0f, 0.0f));
	pComposite->SetMass(1.0f);
	AddPhysicsObject(pComposite);
	//*/

	//*/

Error:
	return r;
}

RESULT DreamTestApp::Update(void) {
	RESULT r = R_PASS;

	CR(m_pTestSuite->UpdateAndRunTests((void*)(this)));

Error:
	return r;
}

// Cloud
RESULT DreamTestApp::OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnNewDreamPeer(DreamPeerApp *pDreamPeer) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
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

RESULT DreamTestApp::OnNewSocketConnection(int seatPosition) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) {
	RESULT r = R_PASS;
	m_pTestSuite->OnDesktopFrame(messageSize, pMessageData, pxHeight, pxWidth);
	return r;
}

RESULT DreamTestApp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	switch (kbEvent->KeyCode) {
		case (SenseVirtualKey)('N') : {
			if (kbEvent->KeyState != 0) {
				//HUD_OUT("Key 'N' is pressed - next test");
				m_pTestSuite->NextTest();
			}
		} break;
	}

	//Error:
	return r;
}

RESULT DreamTestApp::Notify(CollisionObjectEvent *oEvent) {
	RESULT r = R_PASS;

	dynamic_cast<DimObj*>(oEvent->m_pCollisionObject)->SetVertexColor(color(COLOR_RED));

	for (auto &pObj : oEvent->m_collisionGroup) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);
		pDimObj->SetVertexColor(color(COLOR_PINK));
	}

//Error:
	return r;
}

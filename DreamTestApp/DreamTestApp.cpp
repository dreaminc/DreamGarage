#include "Logger/Logger.h"
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
	sandboxconfig.fUseHMD = false;
	sandboxconfig.fUseLeap = false;
	sandboxconfig.fMouseLook = true;
	SetSandboxConfiguration(sandboxconfig);

//Error:
	return r;
}

RESULT DreamTestApp::LoadScene() {
	RESULT r = R_PASS;

	// IO
	RegisterSubscriber((SenseVirtualKey)('N'), this);

	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamApp, this);

	// Set up the HAL Configuration as needed
	///*
	HALImp::HALConfiguration halconf;
	halconf.fRenderReferenceGeometry = true;
	halconf.fDrawWireframe = false;
	halconf.fRenderProfiler = false;
	SetHALConfiguration(halconf);
	//*/

	// Push to a test suite factory in testing

	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PHYSICS, this);
	m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::UIVIEW, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::UI, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::CLOUD, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::PHYSICS, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::INTERACTION, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::ANIMATION, this);
	//m_pTestSuite = TestSuiteFactory::Make(TestSuiteFactory::TEST_SUITE_TYPE::HAL, this);

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

RESULT DreamTestApp::OnNewDreamPeer(DreamPeer *pDreamPeer) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeer> pDreamPeer) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamTestApp::OnAudioData(PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	return R_NOT_IMPLEMENTED;
}


RESULT DreamTestApp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;
	
	switch (kbEvent->KeyCode) {
		case (SenseVirtualKey)('N') : {
			if (kbEvent->KeyState != 0) {
				HUD_OUT("Key 'N' is pressed - next test");
				m_pTestSuite->NextTest();
			}
		} break;
	}
	
	//Error:
	return r;
}

RESULT DreamTestApp::Notify(CollisionObjectEvent *oEvent) {
	RESULT r = R_PASS;

	dynamic_cast<DimObj*>(oEvent->m_pCollisionObject)->SetColor(color(COLOR_RED));

	for (auto &pObj : oEvent->m_collisionGroup) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);
		pDimObj->SetColor(color(COLOR_PINK));
	}

//Error:
	return r;
}

RESULT DreamTestApp::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	HUD_OUT("DreamAPP command");

//Error:
	return r;
}

#include "SoundTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "SoundClientFactory.h"
#include "SoundFile.h"
#include "SpatialSoundObject.h"

// Browser
#include "DreamGarage\DreamBrowser.h"
#include "WebBrowser\WebBrowserController.h"
#include "WebBrowser\CEFBrowser/CEFBrowserManager.h"

#include "DreamGarage\DreamGamepadCameraApp.h"

SoundTestSuite::SoundTestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("sound", pDreamOS)
{
	// empty
}

RESULT SoundTestSuite::AddTests() {
	RESULT r = R_PASS;

	// Add the tests

	CR(AddTestBrowserSoundRouting());

	CR(AddTestCaptureSound());

	CR(AddTestSpatialSound());

	CR(AddTestPlaySound());

	// Add: MP3 and shits

	CR(AddTestSoundClient());

	CR(AddTestEnumerateDevices());

Error:
	return r;
}

RESULT SoundTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	// empty

Error:
	return r;
}

// TODO: STUB
RESULT SoundTestSuite::AddTestSpatialSound() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	float radius = 2.0f;

	struct TestContext : public DreamSoundSystem::observer {
		
		sphere *pSphere = nullptr;

		std::shared_ptr<SpatialSoundObject> pSpatialSoundObject = nullptr;

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

	} *pTestContext = new TestContext();

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("standard"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

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
			//SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(L"95BPMPiano01.wav", SoundFile::type::WAVE);
			SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(L"TR808/CP.WAV", SoundFile::type::WAVE);
			CN(pNewSoundFile);

			CR(m_pDreamOS->RegisterSoundSystemObserver(pTestContext));

			//CR(pTestContext->pSoundClient->PlaySound(pNewSoundFile));

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 0.0f, 0.0f);
			
			pTestContext->pSpatialSoundObject = m_pDreamOS->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			CN(pTestContext->pSpatialSoundObject);

			CR(pTestContext->pSpatialSoundObject->LoopSoundFile(pNewSoundFile));
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
			static float theta = 0.0f;
			static float localRadius = radius;

			point ptPosition(0.0f, 0.0f, -localRadius);
			ptPosition = RotationMatrix(RotationMatrix::Y_AXIS, theta) * ptPosition;

			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			//vector vEmitterDireciton = point(0.0f, 0.0f, 1.0f);

			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);
			
			pTestContext->pSphere->SetPosition(ptPosition);

			pTestContext->pSpatialSoundObject->SetPosition(ptPosition);
			pTestContext->pSpatialSoundObject->SetEmitterListenerDirection(vEmitterDireciton, vListenerDireciton);

			//theta += 0.00025f;
			//localRadius += 0.0001f;
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

	auto pUITest = AddTest("spatialsound", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of playing a spatial sound");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestBrowserSoundRouting() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	float radius = 2.0f;

	struct TestContext : 
		public SoundClient::observer,
		public DreamBrowserObserver
	{
		SoundClient *pWASAPISoundClient = nullptr;
		SoundClient *pXAudioSoundClient = nullptr;
		
		sphere *pSphere = nullptr;
		quad *m_pBrowserQuad = nullptr;
		
		std::shared_ptr<SpatialSoundObject> pXAudioSpatialSoundObject = nullptr;

		std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
		std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;

		// SoundClient::observer
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
		virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override {
			RESULT r = R_PASS;

			if (pXAudioSoundClient != nullptr) {
				CRM(pXAudioSoundClient->PushAudioPacket(pendingAudioPacket), "Failed to push audio packet");
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
		virtual std::string GetCertificateErrorURL() { return std::string(""); };
		virtual std::string GetLoadErrorURL() { return std::string(""); };

	} *pTestContext = new TestContext();

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("standard"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		
		CN(pTestContext);
		
		{
			light *pLight;
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, -1.0f));

			std::string strURL = "https://www.youtube.com/watch?v=JzqumbhfxRo&t=27s";

			point ptPosition = point(0.0f, 0.0f, -radius);
			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pTestContext->pSphere);
			pTestContext->pSphere->SetPosition(ptPosition);

			// Browser

			pTestContext->m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
			CN(pTestContext->m_pWebBrowserManager);
			CR(pTestContext->m_pWebBrowserManager->Initialize());

			// This presents a timing issue if it works 
			pTestContext->m_pBrowserQuad = m_pDreamOS->AddQuad(3.0f, 3.0f);
			CN(pTestContext->m_pBrowserQuad);

			pTestContext->m_pBrowserQuad->FlipUVHorizontal();

			pTestContext->m_pBrowserQuad->RotateXByDeg(90.0f);
			pTestContext->m_pBrowserQuad->RotateZByDeg(180.0f);
			pTestContext->m_pBrowserQuad->SetPosition(ptPosition);

			// Create the Shared View App
			pTestContext->m_pDreamBrowser = m_pDreamOS->LaunchDreamApp<DreamBrowser>(this);
			pTestContext->m_pDreamBrowser->InitializeWithBrowserManager(pTestContext->m_pWebBrowserManager, strURL);
			CNM(pTestContext->m_pDreamBrowser, "Failed to create dream browser");
			CRM(pTestContext->m_pDreamBrowser->RegisterObserver(pTestContext), "Failed to register browser observer");

			pTestContext->m_pDreamBrowser->SetURI(strURL);

			// Create the capture client
			pTestContext->pWASAPISoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
			CN(pTestContext->pWASAPISoundClient);
			CR(pTestContext->pWASAPISoundClient->RegisterObserver(pTestContext));

			///*
			pTestContext->pXAudioSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_XAUDIO2);
			CN(pTestContext->pXAudioSoundClient);

			//pTestContext->pXAudioSpatialSoundObject = pTestContext->pXAudioSoundClient->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			//CN(pTestContext->pXAudioSpatialSoundObject);

			//CR(pTestContext->pXAudioSoundClient->StartSpatial());
			//CR(pTestContext->pWASAPISoundClient->StartCapture());

			CR(pTestContext->pXAudioSoundClient->StartRender());

			//*/

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 0.0f, 0.0f);
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

		if (pTestContext->m_pBrowserQuad != nullptr && pTestContext->m_pDreamBrowser != nullptr) {
			pTestContext->m_pBrowserQuad->SetDiffuseTexture(pTestContext->m_pDreamBrowser->GetSourceTexture());
		}

		{
			static float theta = 0.0f;
			static float localRadius = radius;

			point ptPosition(0.0f, 0.0f, -localRadius);
			ptPosition = RotationMatrix(RotationMatrix::Y_AXIS, theta) * ptPosition;

			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			//vector vEmitterDireciton = point(0.0f, 0.0f, 1.0f);

			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			if (pTestContext->pSphere != nullptr) {
				pTestContext->pSphere->SetPosition(ptPosition);
			}

			if (pTestContext->pXAudioSpatialSoundObject != nullptr) {
				pTestContext->pXAudioSpatialSoundObject->SetPosition(ptPosition);
				pTestContext->pXAudioSpatialSoundObject->SetEmitterListenerDirection(vEmitterDireciton, vListenerDireciton);
			}

			theta += 0.00025f;
			localRadius += 0.0001f;
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

	auto pUITest = AddTest("browsersoundrouting", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Testing the routing of the browser audio");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestCaptureSound() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	float radius = 2.0f;

	struct TestContext : public DreamSoundSystem::observer {
		SoundClient *pWASAPISoundClient = nullptr;
		SoundClient *pXAudioSoundClient = nullptr;
		sphere *pSphere = nullptr;
		std::shared_ptr<SpatialSoundObject> pXAudioSpatialSoundObject = nullptr;

		RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
			RESULT r = R_PASS;

			float *pFloatAudioBuffer = nullptr;

			//// Simply pushes the capture buffer to the render buffer
			if (pXAudioSpatialSoundObject != nullptr) {
				CR(pXAudioSpatialSoundObject->PushMonoAudioBuffer(numFrames, pCaptureBuffer));
			}

			CNM(pCaptureBuffer, "Soundbuffer invalid");

			//pFloatAudioBuffer = (float*)malloc(sizeof(float) * numFrames);
			pFloatAudioBuffer = new float[numFrames];
			CNM(pFloatAudioBuffer, "Failed to allocate float buffer");

			// This is safe since we control the type of buffer that goes into 
			// the spatial sound object - the soundbuffer we get from capture 
			// could be any type

			CRM(pCaptureBuffer->LoadDataToInterlacedTargetBufferTargetType(pFloatAudioBuffer, numFrames), "Failed to load data into buffer");

			//// Simply pushes the capture buffer to the render buffer
			//if (pWASAPISoundClient != nullptr) {
			//	CR(pWASAPISoundClient->PushMonoAudioBufferToRenderBuffer(numFrames, pCaptureBuffer));
			//}

		Error:
			delete pFloatAudioBuffer;

			return r;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("standard"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);
		{

			//light *pLight;
			//pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));
			vector vLight1 = vector(1.0f, -0.5f, 1.0f);
			vector vLight2 = vector(-1.0f, -0.5f, 1.0f);
			vector vLight3 = vector(0.0f, -0.5f, -1.0f);

			vector vLight4 = vector(0.0f, 1.0f, 0.0f);
			float m_directionalIntensity = 0.45f;

			m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight1);
			m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight2);
			m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight3);
			m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.2f * m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight4);

			point ptPosition = point(0.0f, 0.0f, -radius);
			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pTestContext->pSphere);
			pTestContext->pSphere->SetPosition(ptPosition);

			// Open a sound file
			std::shared_ptr<SoundFile> pNewSoundFile;
			pNewSoundFile = m_pDreamOS->LoadSoundFile(L"95BPMPiano01.wav", SoundFile::type::WAVE);
			CN(pNewSoundFile);

			m_pDreamOS->UnregisterSoundSystemObserver();
			m_pDreamOS->RegisterSoundSystemObserver(pTestContext);

			pTestContext->pXAudioSpatialSoundObject = m_pDreamOS->AddSpatialSoundObject(ptPosition, vEmitterDireciton, vListenerDireciton);
			CN(pTestContext->pXAudioSpatialSoundObject);

			m_pDreamOS->GetCamera()->SetPosition(0.0f, 0.0f, 0.0f);

			//auto pDreamGamepadApp = m_pDreamOS->LaunchDreamApp<DreamGamepadCameraApp>(this);
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
			static float theta = 0.0f;
			static float localRadius = radius;

			point ptPosition(0.0f, 0.0f, -localRadius);
			ptPosition = RotationMatrix(RotationMatrix::Y_AXIS, theta) * ptPosition;

			vector vEmitterDireciton = point(0.0f, 0.0f, 0.0f) - ptPosition;
			//vector vEmitterDireciton = point(0.0f, 0.0f, 1.0f);

			vector vListenerDireciton = vector(0.0f, 0.0f, -1.0f);

			if (pTestContext->pSphere != nullptr) {
				pTestContext->pSphere->SetPosition(ptPosition);
			}

			if (pTestContext->pXAudioSpatialSoundObject != nullptr) {
				pTestContext->pXAudioSpatialSoundObject->SetPosition(ptPosition);
				pTestContext->pXAudioSpatialSoundObject->SetEmitterListenerDirection(vEmitterDireciton, vListenerDireciton);
			}

			theta += 0.00025f;
			localRadius += 0.0001f;
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

	auto pUITest = AddTest("capture", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Test the mic capture capabilities of sound");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestSoundClient() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	struct TestContext {
		SoundClient *pSoundClient = nullptr;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("environment"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		sphere *pSphere;
		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);

		// Create the sound client
		pTestContext->pSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
		CN(pTestContext->pSoundClient);

		CR(pTestContext->pSoundClient->Start());

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
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Do stuff

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

	auto pUITest = AddTest("soundclient", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Testing the general sound client arch");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestPlaySound() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;
	float radius = 2.0f;

	struct TestContext : public SoundClient::observer {
		SoundClient *pSoundClient = nullptr;
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

	} *pTestContext = new TestContext();

	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("standard"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

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
			SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(L"95BPMPiano01.wav", SoundFile::type::WAVE);
			//SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(L"TR808/CP.WAV", SoundFile::type::WAVE);
			CN(pNewSoundFile);

			// Create the sound client
			pTestContext->pSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_XAUDIO2);
			CN(pTestContext->pSoundClient);

			CR(pTestContext->pSoundClient->RegisterObserver(pTestContext));

			CR(pTestContext->pSoundClient->StartRender());

			CR(pTestContext->pSoundClient->PlaySoundFile(pNewSoundFile));
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
			// empty
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

	auto pUITest = AddTest("playsound", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of playing a sound");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestPlaySoundHRTF() {
	return R_NOT_IMPLEMENTED;
}

RESULT SoundTestSuite::AddTestEnumerateDevices() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	struct TestContext {
		SoundClient *pSoundClient = nullptr;
	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("environment"));

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight;
		pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		sphere *pSphere;
		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		// Create the sound client
		pTestContext->pSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
		CN(pTestContext->pSoundClient);

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
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Do stuff

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

	auto pUITest = AddTest("enumerateaudiodevices", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestDescription("Basic test of enumerating audio devices");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}
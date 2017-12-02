#include "SoundTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "SoundClientFactory.h"
#include "SoundFile.h"

SoundTestSuite::SoundTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

SoundTestSuite::~SoundTestSuite() {
	// empty
}

RESULT SoundTestSuite::AddTests() {
	RESULT r = R_PASS;

	// Add the tests

	CR(AddTestSpatialSound());

	CR(AddTestCaptureSound());

	CR(AddTestSoundClient());
	CR(AddTestEnumerateDevices());

Error:
	return r;
}

RESULT SoundTestSuite::SetupPipeline(std::string strRenderProgramName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderProgramName);
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

	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	//TODO: Matrix node
	//	CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

	// Connect output as pass-thru to internal blend program
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

// TODO: STUB
RESULT SoundTestSuite::AddTestSpatialSound() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	struct TestContext : public SoundClient::observer {
		SoundClient *pSoundClient = nullptr;

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

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		//// Open a sound file
		//SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(L"95BPMPiano01.wav", SoundFile::type::WAVE);
		//CN(pNewSoundFile);

		// Create the sound client
		pTestContext->pSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
		CN(pTestContext->pSoundClient);

		//CR(pTestContext->pSoundClient->RegisterObserver(pTestContext));
		//CR(pTestContext->pSoundClient->Start());

		//CR(pTestContext->pSoundClient->PlaySound(pNewSoundFile));

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Audio Play Spatial Sound");
	pUITest->SetTestDescription("Basic test of playing a spatial sound");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestCaptureSound() {
	RESULT r = R_PASS;

	double sTestTime = 6000.0f;
	int nRepeats = 1;

	struct TestContext : public SoundClient::observer {
		SoundClient *pSoundClient = nullptr;

		RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
			RESULT r = R_PASS;

			
			// Simply pushes the capture buffer to the render buffer
			if (pSoundClient != nullptr) {
				CR(pSoundClient->PushMonoAudioBufferToRenderBuffer(numFrames, pCaptureBuffer));
			}

		Error:
			return r;
		}

	} *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		CR(SetupPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

		// Open a sound file
		SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(L"95BPMPiano01.wav", SoundFile::type::WAVE);
		CN(pNewSoundFile);

		// Create the sound client
		pTestContext->pSoundClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
		CN(pTestContext->pSoundClient);

		CR(pTestContext->pSoundClient->RegisterObserver(pTestContext));

		CR(pTestContext->pSoundClient->Start());

		CR(pTestContext->pSoundClient->PlaySound(pNewSoundFile));

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Audio Play Sound WASAPI");
	pUITest->SetTestDescription("Basic test of playing a sound utilizing WASAPI");
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

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Audio Play Sound WASAPI");
	pUITest->SetTestDescription("Basic test of playing a sound utilizing WASAPI");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT SoundTestSuite::AddTestPlaySound() {
	return R_NOT_IMPLEMENTED;
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

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);

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

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("Audio Device Enumeration Test");
	pUITest->SetTestDescription("Basic test of enumerating devices");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}
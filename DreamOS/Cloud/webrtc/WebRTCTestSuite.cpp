#include "WebRTCTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Sandbox/CommandLineManager.h"
#include "Cloud/HTTP/HTTPController.h"

#include "Cloud/CloudControllerFactory.h"

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

	CR(AddTestWebRTCVideoStream());

Error:
	return r;
}

CloudController *WebRTCTestSuite::GetCloudController() {
	return m_pDreamOS->GetCloudController();
}

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

Error:
	return r;
}

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

		virtual RESULT OnAudioData(PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
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
		int pxWidth = 640;
		int pxHeight = 480;
		int channels = 4;

		std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

		CR(SetupSkyboxPipeline("environment"));

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects 
		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		
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
			texture::PixelFormat::RGBA, 
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
			texture::PixelFormat::RGBA,
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
			texture::PixelFormat::BGRA)
		);

		//CR(pTestContext->pSourceTexture->LoadImageFromTexture(0, texture::PixelFormat::BGRA));

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
				texture::PixelFormat::BGRA)
			);
		}

		// Replace with BroadcastTexture
		if (pTestContext->pCloudController != nullptr) {
			pTestContext->pCloudController->BroadcastTextureFrame(pTestContext->pSourceTexture, 0, texture::PixelFormat::BGRA);
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
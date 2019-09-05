#include "DreamVCamApp.h"

#include "os/DreamOS.h"

#include "ui/UICommon.h"

#include "scene/ObjectStoreNode.h"
#include "scene/CameraNode.h"

// TODO: get rid of OGL at this level
// Think about a more portable way to do this
// but VCam will fail right now outside of OGL anyways
#include "hal/ogl/OGLTexture.h"
#include "hal/ogl/OGLProgram.h"
#include "hal/ogl/OGLProgramReflection.h"
#include "hal/ogl/OGLProgramRefraction.h"
#include "hal/ogl/OGLProgramWater.h"
#include "hal/ogl/OGLProgramSkybox.h"

#include "sandbox/NamedPipeServer.h"

#include "pipeline/ProgramNode.h"

#include "apps/DreamGamepadCameraApp/DreamGamepadCameraApp.h"

#include "DreamUpdateVCamMessage.h"

DreamVCamApp::DreamVCamApp(DreamOS *pDreamOS, void *pContext) :
	DreamModule<DreamVCamApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamVCamApp::~DreamVCamApp() {
	Shutdown();
}

RESULT DreamVCamApp::InitializeModule(void *pContext) {
	RESULT r = R_PASS;

	float cameraScale = 0.0004f;

	float cameraScreenWidth = 0.6f;
	float cameraScreenHeight = cameraScreenWidth * 9.0f / 16.0f;
	float cameraScreenDepth = -0.05f;

	point ptCameraModel = point(0.0f, -0.24f, -cameraScreenDepth);
	point ptDefaultCamera;
	quaternion qDefaultCamera;

	// offset so the background rendering doesn't conflict with the screen
	float cameraScreenBackgroundOffset = -0.001f;

	SetName("DreamVCam");
	SetModuleDescription("The Dream Virtual Camera Module");

	m_pCameraComposite = GetDOS()->MakeComposite();
	CN(m_pCameraComposite);
	GetDOS()->AddObject(m_pCameraComposite);
	m_pCameraComposite->SetOrientationOffsetDeg(0, 180, 0);
	m_pCameraComposite->SetVisible(true, false);

	m_pCameraModel = m_pCameraComposite->AddModel(L"\\camera\\camera.fbx");
	CN(m_pCameraModel);
	GetDOS()->AddObject(m_pCameraComposite);
	m_pCameraModel->SetScale(cameraScale);
	m_pCameraModel->SetPosition(ptCameraModel);
	m_pCameraModel->SetVisible(false);

	m_pCameraQuad = m_pCameraComposite->AddQuad(cameraScreenWidth, cameraScreenHeight);
	CN(m_pCameraQuad);
	m_pCameraQuad->RotateXByDeg(90.0f);
	m_pCameraQuad->SetVisible(false);

	m_pCameraQuad->FlipUVVertical(); // TODO: shouldn't this not have to happen?

	m_pCameraQuadBackground = m_pCameraComposite->AddQuad(cameraScreenWidth*BORDER_WIDTH, cameraScreenWidth*BORDER_HEIGHT);
	CN(m_pCameraQuadBackground);
	m_pCameraQuadBackground->RotateXByDeg(90.0f);
	m_pCameraQuadBackground->SetPosition(point(0.0f, 0.0f, cameraScreenBackgroundOffset));
	m_pCameraQuadBackground->SetVisible(false);

	m_pCameraQuadBackgroundTexture = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"control-view-main-background.png");
	CN(m_pCameraQuadBackgroundTexture);
	m_pCameraQuadBackground->SetDiffuseTexture(m_pCameraQuadBackgroundTexture);

	m_pMuteTexture = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &k_wstrMute[0]);
	m_pInUseTexture = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &k_wstrInUse[0]);
	m_pClosedTexture = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &k_wstrClosed[0]);

	{
		auto pOGLTexture = dynamic_cast<OGLTexture*>(m_pClosedTexture);
		pOGLTexture->EnableOGLPBOPack();

		pOGLTexture = dynamic_cast<OGLTexture*>(m_pInUseTexture);
		pOGLTexture->EnableOGLPBOPack();
	}

	GetDOS()->GetDefaultVCamPlacement(ptDefaultCamera, qDefaultCamera);

	m_pCamera = DNode::MakeNode<CameraNode>(ptDefaultCamera, viewport(1280, 720, 60));
	CN(m_pCamera);
	CB(m_pCamera->incRefCount());
	m_pCamera->SetOrientation(qDefaultCamera);

	m_pDreamGamepadCamera = GetDOS()->LaunchDreamApp<DreamGamepadCameraApp>(this, false);
	CN(m_pDreamGamepadCamera);
	CR(m_pDreamGamepadCamera->SetCamera(m_pCamera, DreamGamepadCameraApp::CameraControlType::SENSECONTROLLER));
	CR(m_pDreamGamepadCamera->RegisterGamepadCameraObserver(this));

Error:
	return r;
}

const wchar_t kDreamVCamNamedPipeServerName[] = L"dreamvcampipe";

RESULT DreamVCamApp::InitializePipeline() {
	RESULT r = R_PASS;	
	
	// Set up named pipe server
	m_pNamedPipeServer = GetDOS()->MakeNamedPipeServer(kDreamVCamNamedPipeServerName);
	CN(m_pNamedPipeServer);

	CRM(m_pNamedPipeServer->RegisterMessageHandler(std::bind(&DreamVCamApp::HandleServerPipeMessage, this, std::placeholders::_1, std::placeholders::_2)),
		"Failed to register message handler");

	CRM(m_pNamedPipeServer->Start(), "Failed to start server");

	CR(m_pNamedPipeServer->RegisterNamedPipeServerObserver(this));
	m_sourceType = SourceType::CAMERA;	// defaulting to camera on open

	// TODO: Parameterize this eventually
	int width = 1280;
	int height = 720;
	int channels = 4;
	
	m_pLoadBuffer_n = width * height * channels * sizeof(unsigned char);
	m_pLoadBuffer[0] = (unsigned char*) malloc(m_pLoadBuffer_n);
	m_pLoadBuffer[1] = (unsigned char*)malloc(m_pLoadBuffer_n);
	CNM(m_pLoadBuffer[0], "Failed to allocate DreamCam buffer");
	CNM(m_pLoadBuffer[1], "Failed to allocate DreamCam buffer");

	// Set up the aux camera and local pipeline


	r = GetDOS()->MakePipeline(m_pCamera, m_pOGLRenderNode, m_pOGLEndNode, Sandbox::PipelineType::AUX);
	if (r != R_NOT_IMPLEMENTED) {
		CR(r);
	}
	else {
		///*
		ProgramNode *pRenderProgramNode = GetDOS()->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", GetDOS()->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pCamera->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode *pReferenceGeometryProgram = GetDOS()->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", GetDOS()->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pCamera->Output("stereocamera")));
		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode *pSkyboxProgram = GetDOS()->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", GetDOS()->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pCamera->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		m_pOGLRenderNode = dynamic_cast<OGLProgram*>(pRenderProgramNode);
		CN(m_pOGLRenderNode);

		m_pOGLEndNode = dynamic_cast<OGLProgram*>(pSkyboxProgram);
		CN(m_pOGLEndNode);

		//CRM(SetSourceTexture(std::shared_ptr<texture>(m_pOGLRenderNode->GetOGLFramebufferColorTexture())), "Failed to set source texture");

		//CRM(SetSourceTexture(m_pOGLRenderNode->GetOGLFramebufferColorTexture()), "Failed to set source texture");
		//*/
	}

	CNM(m_pOGLRenderNode, "Failed to create mirror pipeline for virtual camera");
	CNM(m_pOGLEndNode, "Failed to create mirror pipeline for virtual camera");
	m_pSourceTexture = m_pOGLRenderNode->GetOGLFramebufferColorTexture();
	m_pSourceTexture->SetUVVerticalFlipped();

	// Pass in a context if needed in future
	CRM(StartModuleProcess(), "Failed to start module process");

Error:
	return r;
}

CameraNode *DreamVCamApp::GetCameraNode() {
	return m_pCamera;
}

RESULT DreamVCamApp::HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	char *pszMessage = (char *)(pBuffer);
	CN(pszMessage);

	DEBUG_LINEOUT("DreamVCam::HandleServerPipeMessage: %s", pszMessage);

Error:
	return r;
}

RESULT DreamVCamApp::Update(void *pContext) {
	RESULT r = R_PASS;

	static int count = 0;

	static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

	if (m_pendingFrame.fPending) {
		CR(UpdateFromPendingVideoFrame());
	}

	//*
	if (m_fReceivingCameraPlacement && m_pLoadBuffer[0] != nullptr) {
		m_pStreamingTexture = m_pInUseTexture;

		size_t bufferSize = m_pStreamingTexture->GetTextureSize();
		m_loadBufferIndex = (m_loadBufferIndex + 1) % 2;
		m_pStreamingTexture->LoadBufferFromTexture(m_pLoadBuffer[m_loadBufferIndex], bufferSize);
	}
	//*/
	/*
	else if (!m_fIsRunning) {
		m_pStreamingTexture = m_pClosedTexture;

		size_t bufferSize = m_pStreamingTexture->GetTextureSize();
		m_loadBufferIndex = (m_loadBufferIndex + 1) % 2;
		m_pStreamingTexture->LoadFlippedBufferFromTexture(m_pLoadBuffer[m_loadBufferIndex], bufferSize);
	}
	//*/

	CBR(m_fIsRunning, R_SKIPPED);

	CNR(m_pOGLEndNode, R_SKIPPED);
	CNR(m_pOGLRenderNode, R_SKIPPED);
	//*
	// TODO: Some more logic around texture / buffer sizes etc 
	//if (m_pNamedPipeServer != nullptr && m_pSourceTexture != nullptr) {
	{
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		// Approximately 24 FPS
		if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 41) {

			texture *pTexture = m_pOGLRenderNode->GetOGLFramebufferColorTexture();	

			UnsetSourceTexture();
			CRM(SetSourceTexture(pTexture), "Failed to set source texture from render node in Dream VCam");

			GetDOS()->GetUserApp()->UpdateLabelOrientation(m_pCamera);
			GetDOS()->UpdateAllPeerLabelOrientations(m_pCamera);
			// Update the local render
			CR(m_pOGLEndNode->RenderNode(count++));
			GetDOS()->UpdateAllPeerLabelOrientations(GetDOS()->GetCamera());

			if (m_fIsMuted) {
				m_pStreamingTexture = m_pMuteTexture;
			}
			else {
				switch (m_sourceType) {
				case(DreamVCamApp::SourceType::CAMERA): {	
					m_pStreamingTexture = m_pSourceTexture;
				} break;

				case(DreamVCamApp::SourceType::SHARE_SCREEN): {
					if (GetDOS()->GetSharedContentPointerTexture() != nullptr) {
						m_pStreamingTexture = GetDOS()->GetSharedContentPointerTexture();
					}
					else {	// could also just cascade close event from ControlArea?
						m_sourceType = SourceType::CAMERA;
					}
				} break;
				}
			}
			//*	
			size_t bufferSize = m_pStreamingTexture->GetTextureSize();
			m_loadBufferIndex = (m_loadBufferIndex + 1) % 2;

			std::unique_lock<std::mutex> lockBufferMutex(m_BufferMutex[m_loadBufferIndex]);

			if (bufferSize == m_pLoadBuffer_n) {
				// TODO: We currently don't support multi-sample, so need to make sure
				// to render one sample (expose with flag)
				if (m_sourceType == SourceType::CAMERA) {	
					OGLTexture* pOGLStreamingTexture = dynamic_cast<OGLTexture*>(m_pStreamingTexture);
					if (!pOGLStreamingTexture->IsOGLPBOPackEnabled()) {
						pOGLStreamingTexture->EnableOGLPBOPack();
					}
					m_pStreamingTexture->LoadBufferFromTexture(m_pLoadBuffer[m_loadBufferIndex], bufferSize);
				}
				else {
					OGLTexture* pOGLStreamingTexture = dynamic_cast<OGLTexture*>(m_pStreamingTexture);
					if (!pOGLStreamingTexture->IsOGLPBOPackEnabled()) {
						pOGLStreamingTexture->EnableOGLPBOPack();
					}

					m_pStreamingTexture->LoadFlippedBufferFromTexture(m_pLoadBuffer[m_loadBufferIndex], bufferSize);
				}


				if (m_fPendDisconnectPipes) {
					auto timeNow = std::chrono::system_clock::now();
					m_pStreamingTexture = m_pClosedTexture;
					size_t bufferSize = m_pStreamingTexture->GetTextureSize();
					m_loadBufferIndex = (m_loadBufferIndex + 1) % 2;
					m_pStreamingTexture->LoadBufferFromTexture(m_pLoadBuffer[m_loadBufferIndex], bufferSize);

					if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - m_msTimeClosed).count() > 1000) {
						m_fPendDisconnectPipes = false;
						CR(m_pNamedPipeServer->ClearConnections());
						m_fIsRunning = false;
					}
				}

				// This part can at least go in the thread
				//m_pNamedPipeServer->SendMessage((void*)(m_pLoadBuffer[m_loadBufferIndex]), m_pLoadBuffer_n);

				lastUpdateTime = timeNow;
			}
			else {
				DEBUG_LINEOUT("Mismatch in buffer size for source texture and virtual camera");
			}
			//*/
		}
	}
	//*/

	if (m_pCameraModel != nullptr && m_pCamera != nullptr && m_pParentApp != nullptr) {
		
		// Check if Active Source
		if (m_pParentApp->GetActiveSource() != nullptr) {
			if (m_pParentApp->GetActiveSource().get() == this && m_pParentApp->IsContentVisible()) {

				if (m_pParentApp->IsScrollingTabs(HAND_TYPE::HAND_LEFT)) {
					m_pDreamGamepadCamera->ClearUpdateFlags(HAND_TYPE::HAND_LEFT);
				}
				if (m_pParentApp->IsScrollingTabs(HAND_TYPE::HAND_RIGHT)) {
					m_pDreamGamepadCamera->ClearUpdateFlags(HAND_TYPE::HAND_RIGHT);
				}

				if (m_pDreamGamepadCamera->GetCameraControlType() != DreamGamepadCameraApp::CameraControlType::SENSECONTROLLER) {
					CR(m_pDreamGamepadCamera->SetCamera(m_pCamera, DreamGamepadCameraApp::CameraControlType::SENSECONTROLLER));
				}
			}
			else if (m_pDreamGamepadCamera->GetCameraControlType() != DreamGamepadCameraApp::CameraControlType::INVALID) {
				CR(m_pDreamGamepadCamera->UnregisterFromEvents());
			}
		}
		
		m_pCameraComposite->SetPosition(m_pCamera->GetPosition(true));
		m_pCameraComposite->SetOrientation(m_pCamera->GetWorldOrientation());
		/*
		m_pCameraModel->SetPosition(m_pCamera->GetPosition(true));
		m_pCameraModel->SetOrientation(m_pCamera->GetWorldOrientation());

		m_pCameraQuad->SetPosition(m_pCamera->GetPosition(true) + point(0.0f, 0.12f, 0.0f));
		m_pCameraQuad->SetOrientation(m_pCamera->GetWorldOrientation());
		//*/
	}

	// With better communication with Gamepad, potentially could send this less often
	//*
	if (m_fSendingCameraPlacement && m_pParentApp != nullptr) {
		CR(BroadcastVCamMessage());
	}
	//*/

	/*
	if (m_fPendCameraPlacement) {
		m_fPendCameraPlacement = false;
		CR(ShareCameraSource());
	}
	//*/

Error:
	return r;
}

RESULT DreamVCamApp::OnDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	// TODO: 

Error:
	return r;
}

RESULT DreamVCamApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	// TODO: 

Error:
	return r;
}

// The Self Construct
DreamVCamApp* DreamVCamApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamVCamApp *pDreamModule = new DreamVCamApp(pDreamOS, pContext);
	return pDreamModule;
}

// NOTE: This is not currently used, update is used to render VCam pipeline
RESULT DreamVCamApp::ModuleProcess(void *pContext) {
	RESULT r = R_PASS;

	int stayAliveCount = 0;
	static std::chrono::system_clock::time_point lastSentTime = std::chrono::system_clock::now();

	// TODO: Cross thread OGL calls don't seem to work - need to investigate

	while (true) {
		//DEBUG_LINEOUT("vcam: stayalive - %d", (1 * stayAliveCount++));

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		// Approximately 24 FPS
		if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastSentTime).count() > 41) {
			int loadBufferIndex = (m_loadBufferIndex + 1) % 2;
			//std::unique_lock<std::mutex> lockBufferMutex(m_BufferMutex[m_loadBufferIndex], std::try_to_lock);

			//if (lockBufferMutex.owns_lock()) {
				if (m_pNamedPipeServer != nullptr && m_pStreamingTexture != nullptr) {

					m_pNamedPipeServer->SendMessage((void*)(m_pLoadBuffer[loadBufferIndex]), m_pLoadBuffer_n);

					lastSentTime = timeNow;
				}
				else {
					//DEBUG_LINEOUT("NamedPipeServer or Streaming Texture were nullptr in VCam Module Process");
				}
			//}
		}
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		Sleep(1);
	}

Error:
	return r;
}

RESULT DreamVCamApp::SetSourceTexture(texture* pTexture) {
	RESULT r = R_PASS;

	CBM((m_pSourceTexture == nullptr), "Source texture already set");

	m_pSourceTexture = pTexture;
	m_pSourceTexture->SetFormat(PIXEL_FORMAT::BGRA);
	
	if (m_sourceType == DreamVCamApp::SourceType::CAMERA) {
		if (!m_pSourceTexture->IsUVVerticalFlipped()) {
			m_pSourceTexture->SetUVVerticalFlipped();
		}
	}

	if (m_pParentApp != nullptr) {
		std::shared_ptr<DreamContentSource> pContentSource = std::dynamic_pointer_cast<DreamContentSource>(GetDOS()->GetDreamModuleFromUID(GetUID()));
		CNM(pContentSource, "Failed getting VirtualCamera as a content source");
		m_pParentApp->UpdateContentSourceTexture(m_pSourceTexture, pContentSource);
	}

	// Enable PBO packing (DMA memory mapping) 
	//OGLTexture *pOGLTexture = dynamic_cast<OGLTexture*>(m_pSourceTexture.get());
	//CNM(pOGLTexture, "Source texture not compatible OpenGL Texture");

	//if (pOGLTexture->IsOGLPBOPackEnabled() == false) {
	//	CRM(pOGLTexture->EnableOGLPBOPack(), "Failed to enable pack PBO on source texture");
	//}

Error:
	return r;
}

RESULT DreamVCamApp::UnsetSourceTexture() {
	m_pSourceTexture = nullptr;
	return R_PASS;
}

RESULT DreamVCamApp::InitializeWithParent(DreamUserControlAreaApp *pParentApp) {
	RESULT r = R_PASS;

	CN(pParentApp);
	m_pParentApp = pParentApp;	
	
	if (m_fPendCameraPlacement) {
		m_fPendCameraPlacement = false;
		CR(ShareCameraSource());
		DOSLOG(INFO, "Requesting sharing camera source");
	}

Error:
	return r;
}

RESULT DreamVCamApp::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	m_assetID = pEnvironmentAsset->GetAssetID();
	m_strContentType = pEnvironmentAsset->GetContentType();
	return R_PASS;
}

RESULT DreamVCamApp::OnClick(point ptDiff, bool fMouseDown) { 
	return R_NOT_IMPLEMENTED; 
}

RESULT DreamVCamApp::OnMouseMove(point mousePoint) { 
	return R_NOT_IMPLEMENTED; 
}

RESULT DreamVCamApp::OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) { 
	return R_NOT_IMPLEMENTED; 
}

RESULT DreamVCamApp::OnKeyPress(char chkey, bool fkeyDown) { 
	return R_NOT_IMPLEMENTED; 
}

texture* DreamVCamApp::GetSourceTexture() {
	return m_pSourceTexture;
}

RESULT DreamVCamApp::SetScope(std::string strScope) {
	m_strScope = strScope;
	return R_PASS;
}

RESULT DreamVCamApp::SetPath(std::string strPath) {
	m_strPath = strPath;
	return R_PASS;
}

long DreamVCamApp::GetCurrentAssetID() {
	return m_assetID;
}

RESULT DreamVCamApp::SendFirstFrame() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamVCamApp::CloseSource() {
	RESULT r = R_PASS;

	DOSLOG(INFO, "Camera Coordinates: x: %0.3f, y: %0.3f, z: %0.3f", m_pCamera->GetPosition(true).x(), m_pCamera->GetPosition(true).y(), m_pCamera->GetPosition(true).z());

	CR(GetDOS()->SaveCameraSettings(m_pCamera->GetPosition(true), m_pCamera->GetOrientation()));

	m_pCameraModel->SetVisible(false);
	CR(HideCameraSource());

	m_fAutoOpened = false;

	m_fPendDisconnectPipes = true;
	m_msTimeClosed = std::chrono::system_clock::now();

	m_pParentApp->HandleCameraClosed();
	SetSourceType(SourceType::CAMERA);

Error:
	return r;
}

int DreamVCamApp::GetWidth() {
	return 0;
}

int DreamVCamApp::GetHeight() {
	return 0;
}

std::string DreamVCamApp::GetTitle() {
	return m_strTitle;
}

std::string DreamVCamApp::GetContentType() {
	return m_strContentType;
}

std::string DreamVCamApp::GetScheme() {
	return "";
}

std::string DreamVCamApp::GetURL() {
	return "";
}

RESULT DreamVCamApp::OnClientConnect() {
	RESULT r = R_PASS;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	if (m_fSendingCameraPlacement && m_pCurrentCameraShare == nullptr) {
		CR(ShareCameraSource());
	}
	// auto-open case
	else if (!m_fSendingCameraPlacement) {
		pEnvironmentControllerProxy->RequestOpenCamera();
		m_fPendCameraPlacement = true;
		m_fAutoOpened = true;
	}

Error:
	return r;
}

RESULT DreamVCamApp::OnClientDisconnect() {
	RESULT r = R_PASS;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	CR(pEnvironmentControllerProxy->RequestStopSharing(m_pCurrentCameraShare));

	CR(HideCameraSource());

//	m_fIsRunning = false;

Error:
	return r;
}

RESULT DreamVCamApp::OnCameraInMotion() {
	RESULT r = R_PASS;

	CNR(m_pParentApp, R_SKIPPED);
	CR(m_pParentApp->OnCameraInMotion());

Error:
	return r;
}

RESULT DreamVCamApp::OnCameraAtRest() {
	RESULT r = R_PASS;

	CBR(m_fHasReceivedSettings, R_SKIPPED);
	
	//DOSLOG(INFO, "Camera Coordinates: x: %0.3f, y: %0.3f, z: %0.3f", m_pCamera->GetPosition().x(), m_pCamera->GetPosition().y(), m_pCamera->GetPosition().z());
	GetDOS()->SaveCameraSettings(m_pCamera->GetPosition(true), m_pCamera->GetOrientation());

	CNR(m_pParentApp, R_SKIPPED);
	CR(m_pParentApp->OnCameraAtRest());

Error:
	return r;
}

RESULT DreamVCamApp::HandleSettings(point ptPosition, quaternion qOrientation) {
	RESULT r = R_PASS;

	m_fHasReceivedSettings = true;
	DOSLOG(INFO, "Got VCam settings");

	m_pCamera->SetPosition(ptPosition);
	m_pCamera->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamVCamApp::SetIsSendingCameraPlacement(bool fSendingCameraPlacement) {
	RESULT r = R_PASS;

	m_fSendingCameraPlacement = fSendingCameraPlacement;
	m_pCameraComposite->SetVisible(m_fSendingCameraPlacement, false);
	m_pCameraModel->SetVisible(m_fSendingCameraPlacement);

	m_fIsRunning = true;
	DOSLOG(INFO, "VCam visible and running");

Error:
	return r;
}

RESULT DreamVCamApp::SetIsReceivingCameraPlacement(bool fReceivingCameraPlacement) {
	m_fReceivingCameraPlacement = fReceivingCameraPlacement;
	m_pCameraComposite->SetVisible(m_fReceivingCameraPlacement, false);
	m_pCameraModel->SetVisible(m_fReceivingCameraPlacement);
	return R_PASS;
}

bool DreamVCamApp::IsSendingCameraPlacement() {
	return m_fSendingCameraPlacement;
}

bool DreamVCamApp::IsReceivingCameraPlacement() {
	return m_fReceivingCameraPlacement;
}

RESULT DreamVCamApp::HideCameraSource() {
	RESULT r = R_PASS;
	
	m_pCameraQuad->SetVisible(false);
	m_pCameraQuadBackground->SetVisible(false);

Error:
	return r;
}

RESULT DreamVCamApp::ShareCameraSource() {
	RESULT r = R_PASS;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	CR(pEnvironmentControllerProxy->RequestShareAsset(m_pParentApp->GetActiveSource()->GetCurrentAssetID(), SHARE_TYPE_CAMERA));

Error:
	return r;
}

RESULT DreamVCamApp::BroadcastVCamMessage() {
	RESULT r = R_PASS;

	DreamUpdateVCamMessage *pMessage = nullptr;

	CN(m_pCamera);
	CN(m_pCameraModel);

	CBR(m_fSendingCameraPlacement, R_SKIPPED);

	pMessage = new DreamUpdateVCamMessage(0, 0, m_pCamera->GetPosition(true), m_pCamera->GetWorldOrientation(), GetUID());
	CN(pMessage);

	CN(m_pParentApp);
	m_pParentApp->BroadcastDreamAppMessage(pMessage);

Error:
	if (pMessage != nullptr) {
		delete pMessage;
	}
	return r;
}

RESULT DreamVCamApp::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	DreamUpdateVCamMessage* pMessage = (DreamUpdateVCamMessage*)(pDreamAppMessage);

	if (m_fReceivingCameraPlacement) {
		m_pCameraComposite->SetPosition(pMessage->m_body.ptPosition);
		m_pCameraComposite->SetOrientation(pMessage->m_body.qOrientation);
		// TODO: temp
		m_pCameraModel->SetVisible(true);
	}

Error:
	return r;
}

RESULT DreamVCamApp::StartSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	CN(pEnvironmentShare);
	m_pCurrentCameraShare = pEnvironmentShare;

	m_pCameraQuad->SetVisible(true);
	m_pCameraQuadBackground->SetVisible(true);

	//GetActiveSource may be a problem
	if (m_fAutoOpened) {
		m_pCameraQuadTexture = m_pParentApp->GetActiveCameraSource()->GetSourceTexture();
	}
	else {
		m_pCameraQuadTexture = m_pParentApp->GetActiveSource()->GetSourceTexture();
	}

	m_pCameraQuad->SetDiffuseTexture(m_pCameraQuadTexture);	

Error:
	return r;
}

RESULT DreamVCamApp::StopSharing() {
	RESULT r = R_PASS;

	m_pCurrentCameraShare = nullptr;
	m_pCameraQuadTexture = nullptr;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	if (m_fAutoOpened) {
		CR(pEnvironmentControllerProxy->RequestCloseCamera(m_assetID));
	}

	m_fAutoOpened = false;

Error:
	return r;
}

RESULT DreamVCamApp::StartReceiving(PeerConnection *pPeerConnection, std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (GetDOS()->IsRegisteredCameraVideoStreamSubscriber(this)) {
		CR(GetDOS()->UnregisterCameraVideoStreamSubscriber(this));
	}

	CR(GetDOS()->RegisterCameraVideoStreamSubscriber(pPeerConnection, this));

	m_pCameraQuad->SetVisible(true);
	m_pCameraQuadBackground->SetVisible(true);

//	m_pCurrentCameraShare = pEnvironmentShare;

Error:
	return r;
}

RESULT DreamVCamApp::StopReceiving() {
	RESULT r = R_PASS;

	m_pCameraQuad->SetVisible(false);
	m_pCameraQuadBackground->SetVisible(false);

	CR(GetDOS()->UnregisterCameraVideoStreamSubscriber(this));

//	m_pCurrentCameraShare = nullptr;

Error:
	return r;
}

RESULT DreamVCamApp::OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CBR(strVideoTrackLabel == kVCamVideoLabel, R_SKIPPED);
	CNM(pVideoFrameDataBuffer, "no data buffer");

	r = SetupPendingVideoFrame((unsigned char*)(pVideoFrameDataBuffer), pxWidth, pxHeight);

	if (r == R_OVERFLOW) {
		DEBUG_LINEOUT("Overflow frame!");
		return R_PASS;
	}

	CRM(r, "Failed for other reason");

	/*
	if (!GetComposite()->IsVisible()) {
		GetComposite()->SetVisible(true);
	}
	//*/

	if (!m_pCameraQuad->IsVisible()) {
		m_pCameraQuad->SetVisible(true);
	}

Error:
	return r;
}

RESULT DreamVCamApp::SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {

	RESULT r = R_PASS;

	// TODO: programmatic 
	int channels = 4;

	CBRM((m_pendingFrame.fPending == false), R_OVERFLOW, "Buffer already pending");

	m_pendingFrame.fPending = true;
	m_pendingFrame.pxWidth = pxWidth;
	m_pendingFrame.pxHeight = pxHeight;

	// Allocate
	// TODO: Might be able to avoid this if the video buffer is not changing size
	// and just keep the memory allocated instead
	m_pendingFrame.pDataBuffer_n = sizeof(uint8_t) * pxWidth * pxHeight * channels;
	//m_pendingFrame.pDataBuffer = (uint8_t*)malloc(m_pendingFrame.pDataBuffer_n);

	m_pendingFrame.pDataBuffer = pVideoFrameDataBuffer;

	CNM(m_pendingFrame.pDataBuffer, "Failed to allocate video buffer mem");

	// Copy
	//memcpy(m_pendingFrame.pDataBuffer, pVideoFrameDataBuffer, m_pendingFrame.pDataBuffer_n);

Error:
	return r;
}

RESULT DreamVCamApp::UpdateFromPendingVideoFrame() {
	RESULT r = R_PASS;

	if (m_pCameraQuadTexture == nullptr ||
		m_pendingFrame.pDataBuffer_n != m_pCameraQuadTexture->GetHeight() * m_pCameraQuadTexture->GetWidth() * m_pCameraQuadTexture->GetChannels()) {

		m_pCameraQuadTexture = GetDOS()->MakeTexture(
			texture::type::TEXTURE_2D,
			m_pendingFrame.pxWidth,
			m_pendingFrame.pxHeight,
			PIXEL_FORMAT::BGRA,
			4,
			&m_pendingFrame.pDataBuffer[0],
			(int)m_pendingFrame.pDataBuffer_n);

		CR(m_pCameraQuadTexture->UpdateDimensions(m_pendingFrame.pxWidth, m_pendingFrame.pxHeight));

		OGLTexture* pOGLTexture = dynamic_cast<OGLTexture*>(m_pCameraQuadTexture);
		if (!pOGLTexture->IsOGLPBOUnpackEnabled()) {
			pOGLTexture->EnableOGLPBOUnpack();
		}
	}
	else {
		if (m_pCameraQuad->GetTextureDiffuse() != m_pCameraQuadTexture) {
			m_pCameraQuad->SetDiffuseTexture(m_pCameraQuadTexture);
		}

		CRM(m_pCameraQuadTexture->UpdateTextureFromBuffer((unsigned char*)(m_pendingFrame.pDataBuffer), m_pendingFrame.pDataBuffer_n), "Failed to update texture from pending frame");
	}

Error:
	if (m_pendingFrame.pDataBuffer != nullptr) {
		delete[] m_pendingFrame.pDataBuffer;
		m_pendingFrame.pDataBuffer = nullptr;

		memset(&m_pendingFrame, 0, sizeof(PendingFrame));
	}
	return r;
}

texture* DreamVCamApp::GetCameraQuadTexture() {
	return m_pCameraQuadTexture;
}

RESULT DreamVCamApp::SetSourceType(DreamVCamApp::SourceType sourceType) {
	RESULT r = R_PASS;
	
	DOSLOG(INFO, "Switching camera source to %d", (int)sourceType);
	m_sourceType = sourceType;

Error:
	return r;
}

RESULT DreamVCamApp::Mute(bool fMute) {
	RESULT r = R_PASS;

	m_fIsMuted = fMute;
	GetDOS()->MuteDreamVCamAudio(fMute);

Error:
	return r;
}

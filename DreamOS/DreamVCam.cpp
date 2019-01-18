#include "DreamVCam.h"

#include "DreamOS.h"

#include "Scene/ObjectStoreNode.h"

// Think about a more portable way to do this
// but VCam will fail right now outside of OGL anyways
#include "HAL/opengl/OGLTexture.h"

#include "Sandbox/NamedPipeServer.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/opengl/OGLProgram.h"
#include "DreamGarage/DreamGamepadCameraApp.h"

#include "HAL/opengl/OGLProgramReflection.h"
#include "HAL/opengl/OGLProgramRefraction.h"
#include "HAL/opengl/OGLProgramWater.h"
#include "HAL/opengl/OGLProgramSkybox.h"

#include "DreamUpdateVCamMessage.h"

#include "DreamGarage/UICommon.h"

DreamVCam::DreamVCam(DreamOS *pDreamOS, void *pContext) :
	DreamModule<DreamVCam>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamVCam::~DreamVCam() {
	Shutdown();
}

RESULT DreamVCam::InitializeModule(void *pContext) {
	RESULT r = R_PASS;

	float cameraScale = 0.0004f;

	float cameraScreenWidth = 0.6f;
	float cameraScreenDepth = -0.05f;

	point ptCameraModel = point(0.0f, -0.24f, -cameraScreenDepth);

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

	m_pCameraQuad = m_pCameraComposite->AddQuad(cameraScreenWidth, cameraScreenWidth*9.0f / 16.0f);
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

	m_pMuteTexture = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"camera-mute.png");

	/*
	m_pCameraQuad = GetDOS()->MakeQuad(0.12f*test, 0.12f*9.0f / 16.0f*test);
	//*/
	//GetDOS()->AddObject(m_pCameraQuad.get(), SandboxApp::PipelineType::MAIN);

	//m_pCameraQuad->SetPosition(0.0f, 0.12f, 0.0f);

	// TODO: 
	m_pCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(1280, 720, 60));
	CN(m_pCamera);
	CB(m_pCamera->incRefCount());

	m_pDreamGamepadCamera = GetDOS()->LaunchDreamApp<DreamGamepadCameraApp>(this, false);
	CN(m_pDreamGamepadCamera);
	CR(m_pDreamGamepadCamera->SetCamera(m_pCamera, DreamGamepadCameraApp::CameraControlType::SENSECONTROLLER));
	CR(m_pDreamGamepadCamera->RegisterGamepadCameraObserver(this));

Error:
	return r;
}

const wchar_t kDreamVCamNamedPipeServerName[] = L"dreamvcampipe";

RESULT DreamVCam::InitializePipeline() {
	RESULT r = R_PASS;

	// Pass in a context if needed in future
	//CRM(StartModuleProcess(), "Failed to start module process");
	
	// Set up named pipe server
	m_pNamedPipeServer = GetDOS()->MakeNamedPipeServer(kDreamVCamNamedPipeServerName);
	CN(m_pNamedPipeServer);

	CRM(m_pNamedPipeServer->RegisterMessageHandler(std::bind(&DreamVCam::HandleServerPipeMessage, this, std::placeholders::_1, std::placeholders::_2)),
		"Failed to register message handler");

	CRM(m_pNamedPipeServer->Start(), "Failed to start server");

	CR(m_pNamedPipeServer->RegisterNamedPipeServerObserver(this));
	m_sourceType = SourceType::CAMERA;	// defaulting to camera on open

	// TODO: Parameterize this eventually
	int width = 1280;
	int height = 720;
	int channels = 4;
	
	m_pLoadBuffer_n = width * height * channels * sizeof(unsigned char);
	m_pLoadBuffer = (unsigned char*) malloc(m_pLoadBuffer_n);
	CNM(m_pLoadBuffer, "Failed to allocate DreamCam buffer");

	// Set up the aux camera and local pipeline


	r = GetDOS()->MakePipeline(m_pCamera, m_pOGLRenderNode, m_pOGLEndNode, SandboxApp::PipelineType::AUX);
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

Error:
	return r;
}

CameraNode *DreamVCam::GetCameraNode() {
	return m_pCamera;
}

RESULT DreamVCam::HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	char *pszMessage = (char *)(pBuffer);
	CN(pszMessage);

	DEBUG_LINEOUT("DreamVCam::HandleServerPipeMessage: %s", pszMessage);

Error:
	return r;
}

RESULT DreamVCam::Update(void *pContext) {
	RESULT r = R_PASS;

	static int count = 0;

	static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

	if (m_pendingFrame.fPending) {
		CR(UpdateFromPendingVideoFrame());
	}

	CBR(m_fIsRunning, R_SKIPPED);

	CNR(m_pOGLEndNode, R_SKIPPED);
	CNR(m_pOGLRenderNode, R_SKIPPED);
	//*
	// TODO: Some more logic around texture / buffer sizes etc 
	//if (m_pNamedPipeServer != nullptr && m_pSourceTexture != nullptr) {
	{
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		// Approximately 30 FPS
		if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 41) {
			
			texture *pTexture = m_pOGLRenderNode->GetOGLFramebufferColorTexture();	

			UnsetSourceTexture();
			CRM(SetSourceTexture(pTexture), "Failed to set source texture from render node in Dream VCam");

			// Update the local render
			CR(m_pOGLEndNode->RenderNode(count++));

			if (m_fIsMuted) {
				m_pStreamingTexture = m_pMuteTexture;
			}
			else {
				switch (m_sourceType) {
				case(DreamVCam::SourceType::CAMERA): {	
					m_pStreamingTexture = m_pSourceTexture;
				} break;

				case(DreamVCam::SourceType::SHARE_SCREEN): {
					if (GetDOS()->GetSharedContentTexture() != nullptr) {
						m_pStreamingTexture = GetDOS()->GetSharedContentTexture();
					}
					else {	// could also just cascade close event from ControlArea?
						m_sourceType = SourceType::CAMERA;
					}
				} break;
				}
			}
			//*
			size_t bufferSize = m_pStreamingTexture->GetTextureSize();

			if (bufferSize == m_pLoadBuffer_n) {
				// TODO: We currently don't support multi-sample, so need to make sure
				// to render one sample (expose with flag)
				if (m_sourceType == SourceType::CAMERA) {	
					OGLTexture* pOGLStreamingTexture = dynamic_cast<OGLTexture*>(m_pStreamingTexture);
					if (!pOGLStreamingTexture->IsOGLPBOPackEnabled()) {
						pOGLStreamingTexture->EnableOGLPBOPack();
					}
					m_pStreamingTexture->LoadBufferFromTexture(m_pLoadBuffer, bufferSize);
				}
				else {
					OGLTexture* pOGLStreamingTexture = dynamic_cast<OGLTexture*>(m_pStreamingTexture);
					if (!pOGLStreamingTexture->IsOGLPBOPackEnabled()) {
						pOGLStreamingTexture->EnableOGLPBOPack();
					}

					m_pStreamingTexture->LoadFlippedBufferFromTexture(m_pLoadBuffer, bufferSize);
				}

				m_pNamedPipeServer->SendMessage((void*)(m_pLoadBuffer), m_pLoadBuffer_n);

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
	if (m_fSendingCameraPlacement) {
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

RESULT DreamVCam::OnDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	// TODO: 

Error:
	return r;
}

RESULT DreamVCam::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	// TODO: 

Error:
	return r;
}

// The Self Construct
DreamVCam* DreamVCam::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamVCam *pDreamModule = new DreamVCam(pDreamOS, pContext);
	return pDreamModule;
}

// NOTE: This is not currently used, update is used to render VCam pipeline
RESULT DreamVCam::ModuleProcess(void *pContext) {
	RESULT r = R_PASS;

	int stayAliveCount = 0;
	
	// TODO: Cross thread OGL calls don't seem to work - need to investigate

	while (true) {
		DEBUG_LINEOUT("vcam: stayalive - %d", (1 * stayAliveCount++));

		// TODO: Some more logic around texture / buffer sizes etc 
		if (m_pNamedPipeServer != nullptr && m_pSourceTexture != nullptr) {
			size_t bufferSize = m_pSourceTexture->GetTextureSize();

			CBM((bufferSize == m_pLoadBuffer_n), "Mismatch in buffer size for source texture and virtual camera");

			CRM(m_pSourceTexture->LoadBufferFromTexture(m_pLoadBuffer, bufferSize), "Failed to load buffer from texture");

			CRM(m_pNamedPipeServer->SendMessage((void*)(m_pLoadBuffer), m_pLoadBuffer_n), "Failed to send vcam buffer");
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

Error:
	return r;
}

RESULT DreamVCam::SetSourceTexture(texture* pTexture) {
	RESULT r = R_PASS;

	CBM((m_pSourceTexture == nullptr), "Source texture already set");

	m_pSourceTexture = pTexture;
	m_pSourceTexture->SetFormat(PIXEL_FORMAT::BGRA);
	
	if (m_sourceType == DreamVCam::SourceType::CAMERA) {
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

RESULT DreamVCam::UnsetSourceTexture() {
	m_pSourceTexture = nullptr;
	return R_PASS;
}

RESULT DreamVCam::InitializeWithParent(DreamUserControlArea *pParentApp) {
	RESULT r = R_PASS;

	CN(pParentApp);
	m_pParentApp = pParentApp;	
	m_fIsRunning = true;

	if (m_fPendCameraPlacement) {
		m_fPendCameraPlacement = false;
		CR(ShareCameraSource());
	}

Error:
	return r;
}

RESULT DreamVCam::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	m_assetID = pEnvironmentAsset->GetAssetID();
	m_strContentType = pEnvironmentAsset->GetContentType();
	return R_PASS;
}

RESULT DreamVCam::OnClick(point ptDiff, bool fMouseDown) { 
	return R_NOT_IMPLEMENTED; 
}

RESULT DreamVCam::OnMouseMove(point mousePoint) { 
	return R_NOT_IMPLEMENTED; 
}

RESULT DreamVCam::OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) { 
	return R_NOT_IMPLEMENTED; 
}

RESULT DreamVCam::OnKeyPress(char chkey, bool fkeyDown) { 
	return R_NOT_IMPLEMENTED; 
}

texture* DreamVCam::GetSourceTexture() {
	return m_pSourceTexture;
}

RESULT DreamVCam::SetScope(std::string strScope) {
	m_strScope = strScope;
	return R_PASS;
}

RESULT DreamVCam::SetPath(std::string strPath) {
	m_strPath = strPath;
	return R_PASS;
}

long DreamVCam::GetCurrentAssetID() {
	return m_assetID;
}

RESULT DreamVCam::SendFirstFrame() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamVCam::CloseSource() {
	RESULT r = R_PASS;

	m_fIsRunning = false;

	DOSLOG(INFO, "Camera Coordinates: x: %0.3f, y: %0.3f, z: %0.3f", m_pCamera->GetPosition().x(), m_pCamera->GetPosition().y(), m_pCamera->GetPosition().z());

	CR(GetDOS()->SaveCameraSettings(m_pCamera->GetPosition(true), m_pCamera->GetOrientation()));

	m_pCameraModel->SetVisible(false);
	CR(HideCameraSource());

	m_fAutoOpened = false;

	//CR(m_pNamedPipeServer->ClearConnections());

Error:
	return r;
}

int DreamVCam::GetWidth() {
	return 0;
}

int DreamVCam::GetHeight() {
	return 0;
}

std::string DreamVCam::GetTitle() {
	return m_strTitle;
}

std::string DreamVCam::GetContentType() {
	return m_strContentType;
}

RESULT DreamVCam::OnClientConnect() {
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

RESULT DreamVCam::OnClientDisconnect() {
	RESULT r = R_PASS;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	CR(pEnvironmentControllerProxy->RequestStopSharing(m_pCurrentCameraShare));

	CR(HideCameraSource());

Error:
	return r;
}

RESULT DreamVCam::OnCameraInMotion() {
	RESULT r = R_PASS;

	CNR(m_pParentApp, R_SKIPPED);
	CR(m_pParentApp->OnCameraInMotion());

Error:
	return r;
}

RESULT DreamVCam::OnCameraAtRest() {
	RESULT r = R_PASS;

	DOSLOG(INFO, "Camera Coordinates: x: %0.3f, y: %0.3f, z: %0.3f", m_pCamera->GetPosition().x(), m_pCamera->GetPosition().y(), m_pCamera->GetPosition().z());
	GetDOS()->SaveCameraSettings(m_pCamera->GetPosition(true), m_pCamera->GetOrientation());

	CNR(m_pParentApp, R_SKIPPED);
	CR(m_pParentApp->OnCameraAtRest());

Error:
	return r;
}

RESULT DreamVCam::HandleSettings(point ptPosition, quaternion qOrientation) {
	RESULT r = R_PASS;

	m_pCamera->SetPosition(ptPosition);
	m_pCamera->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamVCam::SetIsSendingCameraPlacement(bool fSendingCameraPlacement) {
	RESULT r = R_PASS;

	m_fSendingCameraPlacement = fSendingCameraPlacement;
	m_pCameraComposite->SetVisible(m_fSendingCameraPlacement, false);
	m_pCameraModel->SetVisible(m_fSendingCameraPlacement);

Error:
	return r;
}

RESULT DreamVCam::SetIsReceivingCameraPlacement(bool fReceivingCameraPlacement) {
	m_fReceivingCameraPlacement = fReceivingCameraPlacement;
	m_pCameraComposite->SetVisible(m_fReceivingCameraPlacement, false);
	m_pCameraModel->SetVisible(m_fReceivingCameraPlacement);
	return R_PASS;
}

bool DreamVCam::IsSendingCameraPlacement() {
	return m_fSendingCameraPlacement;
}

bool DreamVCam::IsReceivingCameraPlacement() {
	return m_fReceivingCameraPlacement;
}

RESULT DreamVCam::HideCameraSource() {
	RESULT r = R_PASS;
	
	m_pCameraQuad->SetVisible(false);
	m_pCameraQuadBackground->SetVisible(false);

Error:
	return r;
}

RESULT DreamVCam::ShareCameraSource() {
	RESULT r = R_PASS;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	CR(m_pParentApp->OnVirtualCameraCaptured());
	m_pCameraQuad->SetVisible(true);
	m_pCameraQuadBackground->SetVisible(true);
	m_pCameraQuadTexture = m_pParentApp->GetActiveSource()->GetSourceTexture();
	m_pCameraQuad->SetDiffuseTexture(m_pCameraQuadTexture);

	CR(pEnvironmentControllerProxy->RequestShareAsset(m_pParentApp->GetActiveSource()->GetCurrentAssetID(), SHARE_TYPE_CAMERA));

Error:
	return r;
}

RESULT DreamVCam::BroadcastVCamMessage() {
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
	return r;
}

RESULT DreamVCam::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
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

RESULT DreamVCam::StartSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	CN(pEnvironmentShare);
	m_pCurrentCameraShare = pEnvironmentShare;

Error:
	return r;
}

RESULT DreamVCam::StopSharing() {
	RESULT r = R_PASS;

	m_pCurrentCameraShare = nullptr;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	if (m_fAutoOpened) {
		CR(pEnvironmentControllerProxy->RequestCloseCamera(m_assetID));
	}

	m_fAutoOpened = false;

Error:
	return r;
}

RESULT DreamVCam::StartReceiving(PeerConnection *pPeerConnection, std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
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

RESULT DreamVCam::StopReceiving() {
	RESULT r = R_PASS;

	m_pCameraQuad->SetVisible(false);
	m_pCameraQuadBackground->SetVisible(false);

	CR(GetDOS()->UnregisterCameraVideoStreamSubscriber(this));

//	m_pCurrentCameraShare = nullptr;

Error:
	return r;
}

RESULT DreamVCam::OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
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

RESULT DreamVCam::SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {

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

RESULT DreamVCam::UpdateFromPendingVideoFrame() {
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

texture* DreamVCam::GetCameraQuadTexture() {
	return m_pCameraQuadTexture;
}

RESULT DreamVCam::SetSourceType(DreamVCam::SourceType sourceType) {
	RESULT r = R_PASS;
	
	DOSLOG(INFO, "Switching camera source to %d", (int)sourceType);
	m_sourceType = sourceType;

Error:
	return r;
}

RESULT DreamVCam::Mute(bool fMute) {
	RESULT r = R_PASS;

	m_fIsMuted = fMute;
	GetDOS()->MuteDreamVCamAudio(fMute);

Error:
	return r;
}

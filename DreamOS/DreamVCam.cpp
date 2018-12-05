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

	SetName("DreamVCam");
	SetModuleDescription("The Dream Virtual Camera Module");

	// Pass in a context if needed in future
	//CRM(StartModuleProcess(), "Failed to start module process");

	// Set up named pipe server
	m_pNamedPipeServer = GetDOS()->MakeNamedPipeServer(L"dreamvcampipe");
	CN(m_pNamedPipeServer);

	CRM(m_pNamedPipeServer->RegisterMessageHandler(std::bind(&DreamVCam::HandleServerPipeMessage, this, std::placeholders::_1, std::placeholders::_2)), 
		"Failed to register message handler");

	CRM(m_pNamedPipeServer->Start(), "Failed to start server");

	// TODO: Parameterize this eventually
	int width = 1280;
	int height = 720;
	int channels = 4;
	
	m_pLoadBuffer_n = width * height * channels * sizeof(unsigned char);
	m_pLoadBuffer = (unsigned char*) malloc(m_pLoadBuffer_n);
	CNM(m_pLoadBuffer, "Failed to allocate DreamCam buffer");

	// Set up the aux camera and local pipeline

	// TODO: 
	m_pCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(1280, 720, 60));
	CN(m_pCamera);
	CB(m_pCamera->incRefCount());

	CR(GetDOS()->MakePipeline(m_pCamera, m_pOGLRenderNode, m_pOGLEndNode));
	CNM(m_pOGLRenderNode, "Failed to create mirror pipeline for virtual camera");
	CNM(m_pOGLEndNode, "Failed to create mirror pipeline for virtual camera");

	/*
	ProgramNode *pRenderProgramNode = GetDOS()->MakeProgramNode("standard");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", GetDOS()->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pCamera->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode *pReferenceGeometryProgram = GetDOS()->MakeProgramNode("reference");	CN(pReferenceGeometryProgram);	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", GetDOS()->GetSceneGraphNode()->Output("objectstore")));	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pCamera->Output("stereocamera")));
	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	ProgramNode *pSkyboxProgram = GetDOS()->MakeProgramNode("skybox_scatter");	CN(pSkyboxProgram);	CR(pSkyboxProgram->ConnectToInput("scenegraph", GetDOS()->GetSceneGraphNode()->Output("objectstore")));	CR(pSkyboxProgram->ConnectToInput("camera", m_pCamera->Output("stereocamera")));	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	m_pOGLRenderNode = dynamic_cast<OGLProgram*>(pRenderProgramNode);
	CN(m_pOGLRenderNode);
	
	m_pOGLEndNode = dynamic_cast<OGLProgram*>(pSkyboxProgram);
	CN(m_pOGLEndNode);

	//CRM(SetSourceTexture(std::shared_ptr<texture>(m_pOGLRenderNode->GetOGLFramebufferColorTexture())), "Failed to set source texture");

	//CRM(SetSourceTexture(m_pOGLRenderNode->GetOGLFramebufferColorTexture()), "Failed to set source texture");
	//*/	

	{
		//auto pDreamGamepadCamera = GetDOS()->LaunchDreamApp<DreamGamepadCameraApp>(this, false);
		//CN(pDreamGamepadCamera);
		//CR(pDreamGamepadCamera->SetCamera(m_pCamera));
	}

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

	DEBUG_LINEOUT("HandleServerPipeMessage: %s", pszMessage);

Error:
	return r;
}

RESULT DreamVCam::Update(void *pContext) {
	RESULT r = R_PASS;

	static int count = 0;

	static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
	//*
	// TODO: Some more logic around texture / buffer sizes etc 
	//if (m_pNamedPipeServer != nullptr && m_pSourceTexture != nullptr) {
	{
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		// Approximately 30 FPS
		if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 41) {
			
			texture *pTexture = m_pOGLRenderNode->GetOGLFramebufferColorTexture();
			OGLTexture *pOGLTexture = dynamic_cast<OGLTexture*>(pTexture);
			CN(pOGLTexture);

			if (pOGLTexture->IsOGLPBOPackEnabled()) {
				CR(pOGLTexture->EnableOGLPBOPack());
			}
			
			UnsetSourceTexture();
			CRM(SetSourceTexture(pTexture), "Failed to set source texture for Dream VCam");

			// Update the local render
			CR(m_pOGLEndNode->RenderNode(count++));
			//*
			size_t bufferSize = m_pSourceTexture->GetTextureSize();

			if (bufferSize == m_pLoadBuffer_n) {
				// TODO: We currently don't support multi-sample, so need to make sure
				// to render one sample (expose with flag)
				m_pSourceTexture->LoadBufferFromTexture(m_pLoadBuffer, bufferSize);

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

	if (m_pParentApp != nullptr) {
		m_pParentApp->UpdateContentSourceTexture(m_pSourceTexture, this);
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
	m_pParentApp = pParentApp;
	//m_pParentApp->UpdateContentSourceTexture(std::shared_ptr<texture>(m_pSourceTexture), this);
	return R_PASS;
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
	Shutdown();
	return R_PASS;
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
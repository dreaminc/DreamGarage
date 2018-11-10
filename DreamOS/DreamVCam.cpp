#include "DreamVCam.h"

#include "DreamOS.h"

#include "Scene/ObjectStoreNode.h"

// Think about a more portable way to do this
// but VCam will fail right now outside of OGL anyways
#include "HAL/opengl/OGLTexture.h"

#include "Sandbox/NamedPipeServer.h"

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

Error:
	return r;
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

	// TODO: Some more logic around texture / buffer sizes etc 
	if (m_pNamedPipeServer != nullptr && m_pSourceTexture != nullptr) {

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		// Approximately 30 FPS
		if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count() > 30) {
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
		}
	}

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

RESULT DreamVCam::SetSourceTexture(texture *pTexture) {
	RESULT r = R_PASS;

	CBM((m_pSourceTexture == nullptr), "Source texture already set");

	m_pSourceTexture = pTexture;

	// Enable PBO packing (DMA memory mapping) 
	OGLTexture *pOGLTexture = dynamic_cast<OGLTexture*>(m_pSourceTexture);
	CNM(pOGLTexture, "Source texture not compatible OpenGL Texture");

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
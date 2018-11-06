#include "DreamVCam.h"

#include "DreamOS.h"

#include "Scene/ObjectStoreNode.h"

// Think about a more portable way to do this
// but VCam will fail right now outside of OGL anyways
#include "HAL/opengl/OGLTexture.h"

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
	CRM(StartModuleProcess(), "Failed to start module process");

	// TODO: 

Error:
	return r;
}

RESULT DreamVCam::Update(void *pContext) {
	RESULT r = R_PASS;

	// TODO: 

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

	while (true) {
		DEBUG_LINEOUT("vcam: stayalive - %d", (5 * stayAliveCount++));

		// TODO: 

		std::this_thread::sleep_for(std::chrono::seconds(5));
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

	if (pOGLTexture->IsOGLPBOPackEnabled() == false) {
		CRM(pOGLTexture->EnableOGLPBOPack(), "Failed to enable pack PBO on source texture");
	}

Error:
	return r;
}

RESULT DreamVCam::UnsetSourceTexture() {
	m_pSourceTexture = nullptr;
	return R_PASS;
}
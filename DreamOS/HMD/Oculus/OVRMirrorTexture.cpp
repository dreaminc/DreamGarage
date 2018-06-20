#include "OVRMirrorTexture.h"

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLAttachment.h"

#include "HAL/opengl/OGLTexture.h"
#include "HAL/opengl/OGLFramebuffer.h"

OVRMirrorTexture::OVRMirrorTexture(OpenGLImp *pParentImp, ovrSession session, int width, int height) :
	m_pParentImp(pParentImp),
	m_ovrSession(session),
	m_width(width),
	m_height(height),
	m_channels(DEFAULT_MIRROR_TEXTURE_CHANNELS),
	m_pOVRMirrorTexture(nullptr),
	m_pOGLFramebuffer(nullptr),
	m_pOGLTexture(nullptr)
{
	// empty
}

OVRMirrorTexture::~OVRMirrorTexture() {

	if (m_pOGLFramebuffer != nullptr) {
		delete m_pOGLFramebuffer;
		m_pOGLFramebuffer = nullptr;
	}

	if (m_pOVRMirrorTexture != nullptr) {
		DestroyMirrorTexture();
	}
}

RESULT OVRMirrorTexture::OVRInitialize() {
	RESULT r = R_PASS;

	memset(&m_ovrMirrorTextureDescription, 0, sizeof(ovrMirrorTextureDesc));

	m_ovrMirrorTextureDescription.Width = m_width;
	m_ovrMirrorTextureDescription.Height = m_height;
	m_ovrMirrorTextureDescription.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	CR((RESULT)ovr_CreateMirrorTextureGL(m_ovrSession, &m_ovrMirrorTextureDescription, &m_pOVRMirrorTexture));

	GLuint textureIndex = 0;
	CR((RESULT)ovr_GetMirrorTextureBufferGL(m_ovrSession, m_pOVRMirrorTexture, &textureIndex));
	CB((textureIndex != NULL));

	// This will create a new OGLTexture object, but will use the already allocated texture from OVR
	m_pOGLTexture = OGLTexture::MakeTextureFromAllocatedTexture(m_pParentImp, 
																texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, 
																GL_TEXTURE_2D,
																textureIndex,
																m_width,
																m_height,
																3);
	CN(m_pOGLTexture);

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, m_width, m_height, m_channels);
	CN(m_pOGLFramebuffer);
	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	CR(m_pOGLFramebuffer->MakeColorAttachment());
	CR(m_pOGLFramebuffer->InitializeColorAttachment(m_pOGLTexture));

	// TODO: Fit this in more generic?
	CR(m_pParentImp->glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
	CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

	// Turn off vsync to let the compositor do its magic
	CR(m_pParentImp->wglSwapIntervalEXT(0));

Error:
	return r;
}

RESULT OVRMirrorTexture::RenderMirrorToBackBuffer() {
	RESULT r = R_PASS;

	// TODO: Move this to framebuffer
	CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pOGLFramebuffer->GetFramebufferIndex()));
	CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

	CR(m_pParentImp->glBlitFramebuffer(0, m_height, m_width, 0, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
	CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

Error:
	return r;
}

RESULT OVRMirrorTexture::DestroyMirrorTexture() {
	RESULT r = R_PASS;

	ovr_DestroyMirrorTexture(m_ovrSession, m_pOVRMirrorTexture);
	CBM(m_pOVRMirrorTexture == nullptr, "Error Destroying ovrMirrorTexture");

Error:
	return r;
}

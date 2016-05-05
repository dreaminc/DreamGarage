#include "OGLFramebuffer.h"

#include "OpenGLImp.h"
#include "OGLTexture.h"

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp) :
	m_pParentImp(pParentImp),
	m_pOGLTexture(nullptr)
{
	// empty
}

OGLFramebuffer::~OGLFramebuffer() {
	if (m_pOGLTexture != nullptr) {
		delete m_pOGLTexture;
		m_pOGLTexture = nullptr;
	}
}

RESULT OGLFramebuffer::OGLInitialize() {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	// Create Buffer Objects
	CR(m_pParentImp->glGenFramebuffers(1, &m_framebufferIndex));
	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));

	m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR);
	CN(m_pOGLTexture);

Error:
	return r;
}
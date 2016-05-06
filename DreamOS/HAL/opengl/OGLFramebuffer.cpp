#include "OGLFramebuffer.h"

#include "OpenGLImp.h"
#include "OGLTexture.h"

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp, int width, int height, int channels) :
	framebuffer(width, height, channels),
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

	m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, m_width, m_height, m_channels);
	CN(m_pOGLTexture);

	// The depth buffer
	CR(m_pParentImp->glGenRenderbuffers(1, &m_renderbufferIndex));
	CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_renderbufferIndex));
	CR(m_pParentImp->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));
	CR(m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbufferIndex));

	CR(m_pParentImp->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_pOGLTexture->GetOGLTextureIndex(), 0));

	// Set the list of draw buffers.
	m_drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
	CR(m_pParentImp->glDrawBuffers(NUM_OGL_DRAW_BUFFERS, m_drawBuffers));

	// Always check that our framebuffer is ok
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

Error:
	return r;
}

RESULT OGLFramebuffer::BindOGLFramebuffer() {
	RESULT r = R_PASS;

	// Render to our framebuffer
	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));
	glViewport(0, 0, m_width, m_height); 

Error:
	return r;
}
#include "OGLRenderbuffer.h"


OGLRenderbuffer::OGLRenderbuffer(OpenGLImp *pParentImp, int width, int height, int sampleCount) :
	m_pParentImp(pParentImp),
	m_width(width),
	m_height(height),
	m_sampleCount(sampleCount)
{
	// empty
}

OGLRenderbuffer::~OGLRenderbuffer() {
	if (m_OGLRenderbufferIndex != 0) {
		m_pParentImp->glDeleteRenderbuffers(1, &m_OGLRenderbufferIndex);
		m_OGLRenderbufferIndex = 0;
	}
}

RESULT OGLRenderbuffer::OGLInitialize() {
	RESULT r = R_PASS;
	
	CR(m_pParentImp->glGenRenderbuffers(1, &m_OGLRenderbufferIndex));
	CB((m_OGLRenderbufferIndex != 0));

	CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_OGLRenderbufferIndex));
	CR(m_pParentImp->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));
	
	//CR(m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_OGLRenderbufferIndex));

Error:
	return r;
}

RESULT OGLRenderbuffer::OGLInitializeRenderBufferMultisample(GLenum internalDepthFormat, GLenum typeDepth, int multisample) {
	RESULT r = R_PASS;

	m_sampleCount = multisample;

	CR(m_pParentImp->glGenRenderbuffers(1, &m_OGLRenderbufferIndex));
	CB((m_OGLRenderbufferIndex != 0));

	CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_OGLRenderbufferIndex));
	CR(m_pParentImp->glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sampleCount, GL_DEPTH_COMPONENT, m_width, m_height));

Error:
	return r;
}


GLuint OGLRenderbuffer::GetOGLRenderbufferIndex() {
	return m_OGLRenderbufferIndex;
}
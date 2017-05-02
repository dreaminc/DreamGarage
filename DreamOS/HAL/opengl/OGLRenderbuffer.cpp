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
	OGLDelete();
}

RESULT OGLRenderbuffer::OGLDelete() {
	RESULT r = R_PASS;

	if (m_OGLRenderbufferIndex != 0) {
		CR(m_pParentImp->glDeleteRenderbuffers(1, &m_OGLRenderbufferIndex));
	}

	m_OGLRenderbufferIndex = 0;

Error:
	return r;
}

RESULT OGLRenderbuffer::OGLInitialize() {
	RESULT r = R_PASS;
	
	CR(m_pParentImp->glGenRenderbuffers(1, &m_OGLRenderbufferIndex));
	CB((m_OGLRenderbufferIndex != 0));

	CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_OGLRenderbufferIndex));

	if (m_sampleCount > 1) {
		CR(m_pParentImp->glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sampleCount, GL_DEPTH_COMPONENT, m_width, m_height));
	}
	else {
		CR(m_pParentImp->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));
	}

Error:
	return r;
}


GLuint OGLRenderbuffer::GetOGLRenderbufferIndex() {
	return m_OGLRenderbufferIndex;
}

RESULT OGLRenderbuffer::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CB((m_OGLRenderbufferIndex != 0));
	
	CR(OGLDelete());
	CR(OGLInitialize());

Error:
	return r;
}


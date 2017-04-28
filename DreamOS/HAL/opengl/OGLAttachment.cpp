#include "OGLAttachment.h"

#include "OGLTexture.h"
#include "OGLRenderbuffer.h"

OGLAttachment ::OGLAttachment (OpenGLImp *pParentImp, int width, int height, int sampleCount) :
	m_width(width),
	m_height(height),
	m_sampleCount(sampleCount),
	m_pParentImp(pParentImp)
{
	// Empty
}

OGLAttachment ::~OGLAttachment () {
	if (m_pOGLRenderbuffer != nullptr) {
		delete m_pOGLRenderbuffer;
		m_pOGLRenderbuffer = nullptr;
	}

	if (m_pOGLTexture != nullptr) {
		delete m_pOGLTexture;
		m_pOGLTexture = nullptr;
	}
}

GLuint OGLAttachment ::GetOGLRenderbufferIndex() {
	if (m_pOGLRenderbuffer != nullptr) {
		return m_pOGLRenderbuffer->GetOGLRenderbufferIndex();
	}

	return 0;
}

GLuint OGLAttachment ::GetOGLTextureIndex() {
	if (m_pOGLTexture != nullptr) {
		return m_pOGLTexture->GetOGLTextureIndex();
	}

	return 0;
}

// TODO: This is a temporary approach
RESULT OGLAttachment ::OGLInitializeRenderBuffer() {
	RESULT r = R_PASS;

	m_pOGLRenderbuffer = new OGLRenderbuffer(m_pParentImp, m_width, m_height, m_sampleCount);
	CN(m_pOGLRenderbuffer);

	CR(m_pOGLRenderbuffer->OGLInitialize());

	// Move this to the framebuffer
	//CR(m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbufferIndex));

Error:
	return r;
}

// TODO: Potentially combine with the upper function - use mutlisample or not based on multisample value
RESULT OGLAttachment ::OGLInitializeRenderBufferMultisample(GLenum internalDepthFormat, GLenum typeDepth, int multisample = 4) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glGenRenderbuffers(1, &m_depthbufferIndex));
	CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_depthbufferIndex));

	//CR(m_pParentImp->glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH_COMPONENT, m_width, m_height));
	
	CR(m_pParentImp->glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH_COMPONENT, m_width, m_height));
	CR(m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbufferIndex));

Error:
	return r;
}

RESULT OGLAttachment ::OGLInitialize(GLenum internalFormat = GL_DEPTH_COMPONENT24, GLenum type = GL_UNSIGNED_INT) {
	RESULT r = R_PASS;

	// TODO: Replace with texture object instead?
	CR(m_pParentImp->GenerateTextures(1, &m_depthbufferIndex));
	CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_depthbufferIndex));

	CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	/*
	if (GLE_ARB_depth_buffer_float) {
	internalFormat = GL_DEPTH_COMPONENT32F;
	type = GL_FLOAT;
	}
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, type, NULL);

	// The depth buffer
	// Implement OGLRenderbuffer
	// TODO: Create a depth buffer object (like OGLTexture / Framebuffer
	//m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_depthbufferIndex);
	//m_pParentImp->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	//m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbufferIndex);


Error:
	return r;
}
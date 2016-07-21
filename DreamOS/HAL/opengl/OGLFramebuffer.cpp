#include "OGLFramebuffer.h"

#include "OpenGLImp.h"
#include "OGLTexture.h"

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp) :
	framebuffer(),
	m_pParentImp(pParentImp),
	m_pOGLTexture(nullptr),
	m_pDrawBuffers(nullptr),
	m_pOGLDepthbuffer(nullptr)
{
	// empty
}

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp, int width, int height, int channels) :
	framebuffer(width, height, channels),
	m_pParentImp(pParentImp),
	m_pOGLTexture(nullptr),
	m_pDrawBuffers(nullptr),
	m_pOGLDepthbuffer(nullptr)
{
	// empty
}

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp, GLuint textureID, int width, int height, int channels) :
	framebuffer(width, height, channels),
	m_pParentImp(pParentImp),
	m_pOGLTexture(nullptr),
	m_pDrawBuffers(nullptr),
	m_pOGLDepthbuffer(nullptr)
{
	// empty
}

OGLFramebuffer::~OGLFramebuffer() {
	if (m_pOGLTexture != nullptr) {
		delete m_pOGLTexture;
		m_pOGLTexture = nullptr;
	}

	if (m_pOGLDepthbuffer != nullptr) {
		delete m_pOGLDepthbuffer;
		m_pOGLDepthbuffer = nullptr;
	}

	if (m_pDrawBuffers != nullptr) {
		delete[] m_pDrawBuffers;
		m_pDrawBuffers = nullptr;
	}
}

RESULT OGLFramebuffer::MakeOGLTexture() {
	RESULT r = R_PASS;

	m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, m_width, m_height, m_channels);
	CN(m_pOGLTexture);

Error:
	return r;
}

GLuint OGLFramebuffer::GetOGLDepthbufferIndex() {
	return m_pOGLDepthbuffer->GetOGLDepthbufferIndex();
}

RESULT OGLFramebuffer::SetOGLDepthbufferTextureToFramebuffer(GLenum target, GLenum attachment) {
	RESULT r = R_PASS;

	CN(m_pOGLDepthbuffer);
	CR(m_pParentImp->glFramebufferTexture(target, attachment, m_pOGLDepthbuffer->GetOGLDepthbufferIndex(), 0));

Error:
	return r;
}

RESULT OGLFramebuffer::SetDepthTexture(int textureNumber) {
	RESULT r = R_PASS;

	CN(m_pOGLDepthbuffer);

	GLenum glTextureUnit = GL_TEXTURE0 + textureNumber;

	m_pParentImp->glActiveTexture(glTextureUnit);
	m_pParentImp->BindTexture(GL_TEXTURE_2D, m_pOGLDepthbuffer->GetOGLDepthbufferIndex());

Error:
	return r;
}

RESULT OGLFramebuffer::SetOGLTextureToFramebuffer(GLenum target, GLenum attachment) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glFramebufferTexture(target, attachment, m_pOGLTexture->GetOGLTextureIndex(), 0));

Error:
	return r;
}

RESULT OGLFramebuffer::SetOGLTexture(GLuint textureIndex) {
	RESULT r = R_PASS;

	if (textureIndex == NULL)
		m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, m_width, m_height, m_channels);
	else
		m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, textureIndex, m_width, m_height, m_channels);

	CN(m_pOGLTexture);

	// Set the Framebuffer Texture
	CR(SetOGLTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

Error:
	return r;
}

// TODO: Not sure if this is right / should be used
RESULT OGLFramebuffer::SetOGLDrawBuffers(int numDrawBuffers) {
	RESULT r = R_PASS;
	
	m_pDrawBuffers_n = numDrawBuffers;

	if (m_pDrawBuffers != nullptr) {
		delete[] m_pDrawBuffers;
		m_pDrawBuffers = nullptr;
	}

	if (m_pDrawBuffers_n == 0) {
		glDrawBuffer(GL_NONE);
	}
	else {
		m_pDrawBuffers = new GLenum[m_pDrawBuffers_n];
		CN(m_pDrawBuffers);

		for (int i = 0; i < m_pDrawBuffers_n; i++) {
			m_pDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}

		CR(m_pParentImp->glDrawBuffers(m_pDrawBuffers_n, m_pDrawBuffers));
	}

Error:
	return r;
}

RESULT OGLFramebuffer::InitializeRenderBuffer(GLenum internalDepthFormat, GLenum typeDepth) {
	RESULT r = R_PASS;

	CN(m_pOGLDepthbuffer);
	CR(m_pOGLDepthbuffer->OGLInitializeRenderBuffer());

Error:
	return r;
}

RESULT OGLFramebuffer::MakeOGLDepthbuffer() {
	RESULT r = R_PASS;

	m_pOGLDepthbuffer = new OGLDepthbuffer(m_pParentImp, m_width, m_height);
	CN(m_pOGLDepthbuffer);

Error:
	return r;
}

RESULT OGLFramebuffer::SetOGLDepthbuffer(OGLDepthbuffer *pOGLDepthbuffer) {
	RESULT r = R_PASS;

	CB((pOGLDepthbuffer == nullptr));
	m_pOGLDepthbuffer = pOGLDepthbuffer;

Error:
	return r;
}

RESULT OGLFramebuffer::InitializeDepthBuffer(GLenum internalDepthFormat, GLenum typeDepth) {
	RESULT r = R_PASS;

	CN(m_pOGLDepthbuffer);
	CR(m_pOGLDepthbuffer->OGLInitialize(internalDepthFormat, typeDepth));

Error:
	return r;
}

RESULT OGLFramebuffer::OGLInitialize(GLenum internalDepthFormat, GLenum typeDepth) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	// Create Buffer Objects
	CR(m_pParentImp->glGenFramebuffers(1, &m_framebufferIndex));
	
	// Initialize the depth buffer if it exists
	/*
	if (m_pOGLDepthbuffer != nullptr) {
		m_pOGLDepthbuffer->OGLInitialize(internalDepthFormat, typeDepth);
	}
	*/

Error:
	return r;
}

GLuint OGLFramebuffer::GetOGLTextureIndex() {
	if (m_pOGLTexture != nullptr) {
		return m_pOGLTexture->GetOGLTextureIndex();
	}
	else {
		return 0;
	}
}

GLuint OGLFramebuffer::GetFramebufferIndex() {
	return m_framebufferIndex;
}

RESULT OGLFramebuffer::SetAndClearViewport() {
	RESULT r = R_PASS;

	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_FRAMEBUFFER_SRGB);

Error:
	return r;
}

RESULT OGLFramebuffer::SetAndClearViewportDepthBuffer() {
	RESULT r = R_PASS;

	glViewport(0, 0, m_width, m_height);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_FRAMEBUFFER_SRGB);

Error:
	return r;
}

RESULT OGLFramebuffer::BindToScreen(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glViewport(0, 0, pxWidth, pxHeight);

Error:
	return r;
}

// This binds only the depth buffer (color not used) 
RESULT OGLFramebuffer::BindOGLDepthBuffer() {
	RESULT r = R_PASS;

	// Render to our framebuffer
	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));

	/*
	if (m_pOGLDepthbuffer != nullptr) {
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_pOGLDepthbuffer->GetOGLDepthbufferIndex(), 0));
	}
	
	glDrawBuffer(GL_NONE); 

	// Check framebuffer
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));
	*/

Error:
	return r;
}

RESULT OGLFramebuffer::AttachOGLTexture(GLuint textureIndex) {
	RESULT r = R_PASS;

	if (textureIndex == NULL) {
		if (m_pOGLTexture != nullptr) {
			CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GetOGLTextureIndex(), 0));
		}
	}
	else {
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIndex, 0));
	}

Error:
	return r;
}

RESULT OGLFramebuffer::AttachOGLDepthbuffer() {
	RESULT r = R_PASS;
		
	CN(m_pOGLDepthbuffer);
	CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_pOGLDepthbuffer->GetOGLDepthbufferIndex(), 0));

Error:
	return r;
}

RESULT OGLFramebuffer::BindOGLFramebuffer() {
	RESULT r = R_PASS;

	// Render to our framebuffer
	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));

	// TODO: This will clearly blow up texture swap chain
	/*
	if (m_pOGLDepthbuffer != nullptr) {
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_pOGLDepthbuffer->GetOGLDepthbufferIndex(), 0));
	}

	if (textureIndex == NULL) {
		if (m_pOGLTexture != nullptr) {
			CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GetOGLTextureIndex(), 0));
		}
	}
	else {
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIndex, 0));
	}
	//*/

	// Check framebuffer
	//CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

	//CR(SetOGLDrawBuffers(1));

Error:
	return r;
}

RESULT OGLFramebuffer::UnbindOGLFramebuffer() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));
	CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
	CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0));

Error:
	return r;
}
#include "OGLFramebuffer.h"

#include "OpenGLImp.h"

#include "OGLTexture.h"
#include "OGLAttachment.h"
#include "OGLRenderbuffer.h"

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp) :
	framebuffer(),
	m_pParentImp(pParentImp),
	m_pDrawBuffers(nullptr),
	m_pOGLDepthAttachment(nullptr)
{
	// empty
}

OGLFramebuffer::OGLFramebuffer(OpenGLImp *pParentImp, int width, int height, int channels) :
	framebuffer(width, height, channels),
	m_pParentImp(pParentImp),
	m_pDrawBuffers(nullptr),
	m_pOGLDepthAttachment(nullptr)
{
	// empty
}

OGLFramebuffer::~OGLFramebuffer() {
	if (m_pOGLDepthAttachment != nullptr) {
		delete m_pOGLDepthAttachment;
		m_pOGLDepthAttachment = nullptr;
	}

	if (m_pOGLColorAttachment != nullptr) {
		delete m_pOGLColorAttachment;
		m_pOGLColorAttachment = nullptr;
	}

	if (m_pDrawBuffers != nullptr) {
		delete[] m_pDrawBuffers;
		m_pDrawBuffers = nullptr;
	}
}

RESULT OGLFramebuffer::ResizeFramebuffer(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	m_width = pxWidth;
	m_height = pxHeight;

	CR(Bind());

	if (m_pOGLColorAttachment != nullptr) {		
		CR(m_pOGLColorAttachment->Resize(pxWidth, pxHeight));
	}

	if (m_pOGLDepthAttachment != nullptr) {
		CR(m_pOGLDepthAttachment->Resize(pxWidth, pxHeight));
	}

	//CR(InitializeOGLDrawBuffers(1));

	// Always check that our framebuffer is ok
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

Error:
	return r;
}

RESULT OGLFramebuffer::SetDepthTexture(int textureNumber) {
	RESULT r = R_PASS;

	CN(m_pOGLDepthAttachment);

	GLenum glTextureUnit = GL_TEXTURE0 + textureNumber;

	m_pParentImp->glActiveTexture(glTextureUnit);
	m_pParentImp->BindTexture(GL_TEXTURE_2D, m_pOGLDepthAttachment->GetOGLTextureIndex());

Error:
	return r;
}

//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);
RESULT OGLFramebuffer::SetOGLTextureToFramebuffer2D(GLenum target, GLenum attachment, GLenum textarget) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glFramebufferTexture2D(target, attachment, textarget, m_pOGLColorAttachment->GetOGLTextureIndex(), 0));

Error:
	return r;
}

/*
RESULT OGLFramebuffer::SetOGLTexture(GLuint textureIndex) {
	RESULT r = R_PASS;

	if (textureIndex == NULL)
		m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, m_width, m_height, m_channels);
	else
		m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, textureIndex, m_width, m_height, m_channels);

	CN(m_pOGLTexture);

	// Set the Frame buffer Texture
	CR(SetOGLTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

Error:
	return r;
}
*/

// TODO: Not sure if this is right / should be used
RESULT OGLFramebuffer::InitializeOGLDrawBuffers(int numDrawBuffers) {
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


RESULT OGLFramebuffer::MakeColorAttachment() {
	RESULT r = R_PASS;

	CB((m_pOGLColorAttachment == nullptr));

	m_pOGLColorAttachment = new OGLAttachment(m_pParentImp, m_width, m_height, m_channels);
	CN(m_pOGLColorAttachment);

Error:
	return r;
}

RESULT OGLFramebuffer::InitializeColorAttachment(OGLTexture *pOGLTexture) {
	RESULT r = R_PASS;

	CN(m_pOGLColorAttachment);
	CR(m_pOGLColorAttachment->OGLInitialize(pOGLTexture));
	CR(m_pOGLColorAttachment->AttachToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));
	CR(InitializeOGLDrawBuffers(1));

Error:
	return r;
}

RESULT OGLFramebuffer::MakeDepthAttachment() {
	RESULT r = R_PASS;

	CB((m_pOGLDepthAttachment == nullptr));

	m_pOGLDepthAttachment = new OGLAttachment(m_pParentImp, m_width, m_height, m_channels);
	CN(m_pOGLDepthAttachment);

Error:
	return r;
}

RESULT OGLFramebuffer::InitializeRenderBuffer(GLenum internalDepthFormat, GLenum typeDepth) {
	RESULT r = R_PASS;

	CN(m_pOGLDepthAttachment);
	CR(m_pOGLDepthAttachment->OGLInitializeRenderBuffer());

Error:
	return r;
}

RESULT OGLFramebuffer::OGLInitialize(GLenum internalDepthFormat, GLenum typeDepth) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	// Create Buffer Objects
	CR(m_pParentImp->glGenFramebuffers(1, &m_framebufferIndex));
	CB((m_framebufferIndex != 0));

Error:
	return r;
}

GLuint OGLFramebuffer::GetFramebufferIndex() {
	return m_framebufferIndex;
}

RESULT OGLFramebuffer::SetAndClearViewport(bool fColor, bool fDepth) {
	glViewport(0, 0, m_width, m_height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLbitfield glClearBitfield = ((fColor) ? GL_COLOR_BUFFER_BIT : 0) + ((fDepth) ? GL_DEPTH_BUFFER_BIT : 0);
	glClear(glClearBitfield);

	return R_PASS;
}

RESULT OGLFramebuffer::BindToScreen(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glViewport(0, 0, pxWidth, pxHeight);

Error:
	return r;
}

RESULT OGLFramebuffer::Bind() {
	RESULT r = R_PASS;

	// Render to our framebuffer
	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));

Error:
	return r;
}

RESULT OGLFramebuffer::UnbindAttachments() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));

	CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
	CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0));

Error:
	return r;
}

RESULT OGLFramebuffer::Unbind() {
	return m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
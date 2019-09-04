#include "OGLAttachment.h"

#include "OGLTexture.h"
#include "OGLCubemap.h"
#include "OGLRenderbuffer.h"

OGLAttachment::OGLAttachment (OpenGLImp *pParentImp, int width, int height, int channels, int sampleCount) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_sampleCount(sampleCount),
	m_pParentImp(pParentImp)
{
	// Empty
}

OGLAttachment::~OGLAttachment () {
	if (m_pOGLRenderbuffer != nullptr) {
		delete m_pOGLRenderbuffer;
		m_pOGLRenderbuffer = nullptr;
	}

	if (m_pOGLTexture != nullptr) {
		delete m_pOGLTexture;
		m_pOGLTexture = nullptr;
	}
}

GLuint OGLAttachment::GetOGLRenderbufferIndex() {
	if (m_pOGLRenderbuffer != nullptr) {
		return m_pOGLRenderbuffer->GetOGLRenderbufferIndex();
	}

	return 0;
}

GLenum OGLAttachment::GetOGLTextureTarget() {
	if (m_pOGLTexture != nullptr) {
		return m_pOGLTexture->GetOGLTextureTarget();
	}

	return 0;
}

GLuint OGLAttachment::GetOGLTextureIndex() {
	if (m_pOGLTexture != nullptr) {
		return m_pOGLTexture->GetOGLTextureIndex();
	}

	return 0;
}

RESULT OGLAttachment::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CBR(((pxWidth != m_width) || (pxHeight != m_height)), R_SKIPPED);

	m_width = pxWidth;
	m_height = pxHeight;

	if (m_pOGLTexture != nullptr) {
		CR(m_pOGLTexture->Resize(pxWidth, pxHeight));
	}

	if (m_pOGLRenderbuffer != nullptr) {
		CR(m_pOGLRenderbuffer->Resize(pxWidth, pxHeight));
	}

Error:
	return r;
}

// TODO: This is a temporary approach
RESULT OGLAttachment::OGLInitializeRenderBuffer() {
	RESULT r = R_PASS;

	m_pOGLRenderbuffer = new OGLRenderbuffer(m_pParentImp, m_width, m_height, m_sampleCount);
	CN(m_pOGLRenderbuffer);

	CR(m_pOGLRenderbuffer->OGLInitialize());

	// Move this to the framebuffer
	

Error:
	return r;
}

/*
// TODO: Potentially combine with the upper function - use mutli sample or not based on multi sample value
RESULT OGLAttachment ::OGLInitializeRenderBufferMultisample(GLenum internalDepthFormat, GLenum typeDepth, int multisample) {
	RESULT r = R_PASS;

	// TODO: Create a render buffer 
	CR(m_pParentImp->glGenRenderbuffers(1, &m_depthbufferIndex));
	CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_depthbufferIndex));

	//CR(m_pParentImp->glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH_COMPONENT, m_width, m_height));
	
	CR(m_pParentImp->glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH_COMPONENT, m_width, m_height));
	CR(m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbufferIndex));

Error:
	return r;
}
*/

RESULT OGLAttachment::OGLInitialize(OGLTexture *pOGLTexture) {
	RESULT r = R_PASS;

	CN(pOGLTexture);
	CB((m_pOGLTexture == nullptr));

	m_pOGLTexture = pOGLTexture;

Error:
	return r;
}

RESULT OGLAttachment::AttachToFramebuffer(GLenum target, GLenum attachment) {
	RESULT r = R_PASS;

	CN(m_pOGLTexture);
	CR(m_pParentImp->glFramebufferTexture(target, attachment, m_pOGLTexture->GetOGLTextureIndex(), 0));

Error:
	return r;
}

RESULT OGLAttachment::AttachTextureToFramebuffer(GLenum target, GLenum attachment) {
	RESULT r = R_PASS;

	CN(m_pOGLTexture);
	CR(m_pParentImp->glFramebufferTexture2D(target, attachment, m_pOGLTexture->GetOGLTextureTarget(), m_pOGLTexture->GetOGLTextureIndex(), 0));

Error:
	return r;
}

RESULT OGLAttachment::AttachRenderBufferToFramebuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget) {
	RESULT r = R_PASS;

	CN(m_pOGLRenderbuffer);
	CR(m_pParentImp->glFramebufferRenderbuffer(target, attachment, renderbuffertarget, m_pOGLRenderbuffer->GetOGLRenderbufferIndex()));

Error:
	return r;
}

RESULT OGLAttachment::MakeOGLDepthTexture(texture::type type, GLenum internalGLFormat, GLenum pixelDataType) {
	RESULT r = R_PASS;

	m_pOGLTexture = OGLTexture::MakeTextureWithFormat(m_pParentImp, 
		type, 
		m_width, 
		m_height, 
		m_channels,
		internalGLFormat, GL_DEPTH_COMPONENT, pixelDataType
	);
	CN(m_pOGLTexture);

Error:
	return r;
}

RESULT OGLAttachment::MakeOGLTexture(texture::type type) {
	RESULT r = R_PASS;

	if (m_sampleCount == 1) {
		m_pOGLTexture = OGLTexture::MakeTexture(m_pParentImp, type, m_width, m_height, m_channels);
		CN(m_pOGLTexture);
	}
	else {
		CR(MakeOGLTextureMultisample());
	}

Error:
	return r;
}

RESULT OGLAttachment::MakeOGLCubemap() {
	RESULT r = R_PASS;

	if (m_sampleCount == 1) {
		m_pOGLCubemap = OGLCubemap::MakeCubemap(m_pParentImp, m_width, m_height, m_channels);
		CN(m_pOGLCubemap);
	}
	else {
		CBM((false), "Not currently supporting multi-sample cube maps");
	}

Error:
	return r;
}

RESULT OGLAttachment::AttachCubemapToFramebuffer(GLenum target, GLenum attachment) {
	RESULT r = R_PASS;

	CN(m_pOGLCubemap);

	CR(m_pParentImp->glFramebufferTexture2D(target,
											attachment,
											//m_pOGLCubemap->GetOGLTextureTarget(), 
											GL_TEXTURE_CUBE_MAP_POSITIVE_X,				// why this?
											m_pOGLCubemap->GetOGLTextureIndex(),
											0));

Error:
	return r;
}

GLenum OGLAttachment::GetOGLCubemapTarget() {
	if (m_pOGLCubemap != nullptr) {
		return m_pOGLCubemap->GetOGLTextureTarget();
	}

	return 0;
}

GLuint OGLAttachment::GetOGLCubemapIndex() {
	if (m_pOGLCubemap != nullptr) {
		return m_pOGLCubemap->GetOGLTextureIndex();
	}

	return 0;
}

RESULT OGLAttachment::MakeOGLTextureMultisample() {
	RESULT r = R_PASS;

	m_pOGLTexture = new OGLTexture(m_pParentImp, texture::type::TEXTURE_2D, GL_TEXTURE_2D_MULTISAMPLE);
	CN(m_pOGLTexture);

	m_pOGLTexture->SetWidth(m_width);
	m_pOGLTexture->SetHeight(m_height);
	m_pOGLTexture->SetChannels(m_channels);
	m_pOGLTexture->SetSamples(m_sampleCount);

	m_pOGLTexture->OGLInitializeMultisample();

Error:
	return r;
}
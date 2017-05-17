#include "OVRTextureSwapChain.h"

OVRTextureSwapChain::OVRTextureSwapChain(OpenGLImp *pParentImp, ovrSession session, int width, int height, int mipLevels, unsigned char *data, int sampleCount) :
	m_ovrSession(session),
	m_ovrTextureSwapChain(nullptr),
	m_width(width),
	m_height(height),
	m_channels(DEFAULT_TEXTURE_SWAP_CHAIN_CHANNELS),
	m_pOGLResolveFramebuffer(nullptr),
	m_mipLevels(mipLevels),
	m_pParentImp(pParentImp),
	m_sampleCount(sampleCount),
	m_textureSwapChainLength(0)
{
	m_swapChainOGLTextures.clear();
}

OVRTextureSwapChain::~OVRTextureSwapChain() {
	while (m_swapChainOGLTextures.size() > 0) {
		OGLTexture *pTempOGLTexture = m_swapChainOGLTextures.back();
		m_swapChainOGLTextures.pop_back();

		delete pTempOGLTexture;
		pTempOGLTexture = nullptr;
	}

	if (m_pOGLResolveFramebuffer != nullptr) {
		delete m_pOGLResolveFramebuffer;
		m_pOGLResolveFramebuffer = nullptr;
	}
}

RESULT OVRTextureSwapChain::OVRInitialize() {
	RESULT r = R_PASS;

	int m_textureSwapChainLength = 0;

	CB(m_sampleCount <= 1); // The code doesn't currently handle MSAA textures.

	m_ovrTextureSwapChainDescription.Type = ovrTexture_2D;
	m_ovrTextureSwapChainDescription.ArraySize = 1;
	m_ovrTextureSwapChainDescription.Width = m_width;
	m_ovrTextureSwapChainDescription.Height = m_height;
	m_ovrTextureSwapChainDescription.MipLevels = 1;
	m_ovrTextureSwapChainDescription.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_ovrTextureSwapChainDescription.SampleCount = 1;
	m_ovrTextureSwapChainDescription.StaticImage = ovrFalse;

	CR((RESULT)ovr_CreateTextureSwapChainGL(m_ovrSession, &m_ovrTextureSwapChainDescription, &m_ovrTextureSwapChain));
	CR((RESULT)ovr_GetTextureSwapChainLength(m_ovrSession, m_ovrTextureSwapChain, &m_textureSwapChainLength));

	for (int i = 0; i < m_textureSwapChainLength; ++i) {
		GLuint chainTextureIndex;
		ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, i, &chainTextureIndex);

		//OGLTexture *pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, chainTextureIndex, m_width, m_height, m_channels);
		OGLTexture *pOGLTexture = OGLTexture::MakeTextureFromAllocatedTexture(m_pParentImp, 
																			  texture::TEXTURE_TYPE::TEXTURE_COLOR, 
																			  GL_TEXTURE_2D, 
																			  chainTextureIndex, 
																			  m_width, 
																			  m_height, 
																			  m_channels);
		CR(pOGLTexture->Bind());

		CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		// TODO: Move this into the texture class no?
		if (m_mipLevels > 1) {
			CR(m_pParentImp->glGenerateMipmap(GL_TEXTURE_2D));
		}

		m_swapChainOGLTextures.push_back(pOGLTexture);
	}

	m_pOGLResolveFramebuffer = new OGLFramebuffer(m_pParentImp, m_width, m_height, m_channels);
	CR(m_pOGLResolveFramebuffer->OGLInitialize());
	CR(m_pOGLResolveFramebuffer->Bind());

	CR(m_pOGLResolveFramebuffer->MakeDepthAttachment());
	CR(m_pOGLResolveFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture());

	// Set up render FBO
	m_pOGLRenderFramebuffer = new OGLFramebuffer(m_pParentImp, m_width, m_height, m_channels);
	CR(m_pOGLRenderFramebuffer->OGLInitialize());
	CR(m_pOGLRenderFramebuffer->Bind());

	CR(m_pOGLRenderFramebuffer->MakeDepthAttachment());
	//CR(m_pOGLRenderFramebuffer->GetDepthAttachment()->OGLInitializeRenderBufferMultisample(GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT, DEFAULT_OVR_MULTI_SAMPLE));
	CR(m_pOGLRenderFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer(DEFAULT_OVR_MULTI_SAMPLE));

	CR(m_pOGLRenderFramebuffer->MakeColorAttachment());
	CR(m_pOGLRenderFramebuffer->GetColorAttachment()->MakeOGLTextureMultisample());
	CR(m_pOGLRenderFramebuffer->SetOGLTextureToFramebuffer2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE));

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, 0));

Error:
	return r;
}

ovrTextureSwapChain OVRTextureSwapChain::GetOVRTextureSwapChain() {
	return m_ovrTextureSwapChain;
}

ovrSizei OVRTextureSwapChain::GetOVRSizei() {
	ovrSizei retSizei;

	retSizei.w = m_width;
	retSizei.h = m_height;

	return retSizei;
}

ovrRecti OVRTextureSwapChain::GetOVRViewportRecti() {
	ovrRecti retRecti;

	retRecti.Pos.x = 0;
	retRecti.Pos.y = 0;
	retRecti.Size = GetOVRSizei();

	return retRecti;
}

RESULT OVRTextureSwapChain::Commit() {
	if (m_ovrTextureSwapChain)
		return (RESULT)ovr_CommitTextureSwapChain(m_ovrSession, m_ovrTextureSwapChain);
	else
		return R_FAIL;
}

RESULT OVRTextureSwapChain::SetAndClearRenderSurface() {
	RESULT r = R_PASS;

	glEnable(GL_MULTISAMPLE);

	m_pOGLRenderFramebuffer->Bind();
	m_pOGLRenderFramebuffer->SetAndClearViewport(true, true);
	
//Error:
	return r;
}

RESULT OVRTextureSwapChain::UnsetRenderSurface() {
	RESULT r = R_PASS;

	//CR(m_pOGLResolveFramebuffer->UnbindOGLFramebuffer());
	GLuint currentTextureIndex;

	m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_MULTISAMPLE);

	if (m_ovrTextureSwapChain) {
		int currentIndex;
		CR((RESULT)ovr_GetTextureSwapChainCurrentIndex(m_ovrSession, m_ovrTextureSwapChain, &currentIndex));
		CR((RESULT)ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, currentIndex, &currentTextureIndex));
	}
	else {
		// TODO:
		// curTexId = m_textureIndex;
	}

	CR(m_pOGLResolveFramebuffer->Bind());

	// TODO: better way?
	m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currentTextureIndex, 0);

	CR(m_pOGLResolveFramebuffer->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));
	CR(m_pOGLResolveFramebuffer->SetAndClearViewport());
	
	m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pOGLRenderFramebuffer->GetFramebufferIndex());
	m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pOGLResolveFramebuffer->GetFramebufferIndex());

	m_pParentImp->glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

Error:
	return R_PASS;
}
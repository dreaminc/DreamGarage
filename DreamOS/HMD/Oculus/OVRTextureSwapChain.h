#ifndef OVR_TEXTURE_SWAP_CHAIN_H_
#define OVR_TEXTURE_SWAP_CHAIN_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVRTextureSwapChain.h
// The Oculus uses a texture swap chain to pass images to the HMD and doesn't fit 
// into a clear cut representation of a single texture / buffer so this class 
// should wrap up the respective frame and depth buffers as well as the respective textures in the 
// chain

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLDepthbuffer.h"
#include "OVR_CAPI_GL.h"

#include "HAL/opengl/OGLTexture.h"
#include "HAL/opengl/OGLFramebuffer.h"

#include <vector>

#define DEFAULT_TEXTURE_SWAP_CHAIN_CHANNELS 3

class OVRTextureSwapChain {

public:
	OVRTextureSwapChain(OpenGLImp *pParentImp, ovrSession session, int width, int height, int mipLevels, unsigned char *data, int sampleCount) :
		m_ovrSession(session),
		m_ovrTextureSwapChain(nullptr),
		m_width(width),
		m_height(height),
		m_channels(DEFAULT_TEXTURE_SWAP_CHAIN_CHANNELS),
		m_pOGLFramebuffer(nullptr),
		m_mipLevels(mipLevels),
		m_pParentImp(pParentImp),
		m_sampleCount(sampleCount),
		m_textureSwapChainLength(0)
	{
		m_swapChainOGLTextures.clear();
	}

	~OVRTextureSwapChain() {
		while (m_swapChainOGLTextures.size() > 0) {
			OGLTexture *pTempOGLTexture = m_swapChainOGLTextures.back();
			m_swapChainOGLTextures.pop_back();

			delete pTempOGLTexture;
			pTempOGLTexture = nullptr;
		}

		if (m_pOGLFramebuffer != nullptr) {
			delete m_pOGLFramebuffer;
			m_pOGLFramebuffer = nullptr;
		}
	}

	RESULT OVRInitialize() {
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

			OGLTexture *pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, chainTextureIndex, m_width, m_height, m_channels);
			CR(pOGLTexture->BindTexture(GL_TEXTURE_2D));

			CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			CR(pOGLTexture->SetTextureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			if (m_mipLevels > 1) {
				CR(m_pParentImp->glGenerateMipmap(GL_TEXTURE_2D));
			}

			m_swapChainOGLTextures.push_back(pOGLTexture);
		}

		m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, m_width, m_height, m_channels);
		CR(m_pOGLFramebuffer->SetOGLDepthbuffer(nullptr));
		CR(m_pOGLFramebuffer->OGLInitialize());

	Error:
		return r;
	}

	ovrTextureSwapChain GetOVRTextureSwapChain() {
		return m_ovrTextureSwapChain;
	}

	ovrSizei GetOVRSizei() {
		ovrSizei retSizei;

		retSizei.w = m_width;
		retSizei.h = m_height;

		return retSizei;
	}

	ovrRecti GetOVRViewportRecti() {
		ovrRecti retRecti;
		
		retRecti.Pos.x = 0;
		retRecti.Pos.y = 0;
		retRecti.Size = GetOVRSizei();

		return retRecti;
	}

	RESULT Commit() {
		if (m_ovrTextureSwapChain)
			return (RESULT)ovr_CommitTextureSwapChain(m_ovrSession, m_ovrTextureSwapChain);
		else
			return R_FAIL;
	}

	RESULT SetAndClearRenderSurface() {
		RESULT r = R_PASS;

		GLuint currentTextureIndex;

		if (m_ovrTextureSwapChain) {
			int currentIndex;
			CR((RESULT)ovr_GetTextureSwapChainCurrentIndex(m_ovrSession, m_ovrTextureSwapChain, &currentIndex));
			CR((RESULT)ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, currentIndex, &currentTextureIndex));
		}
		else {
			// TODO:
			// curTexId = m_textureIndex;
		}

		CR(m_pOGLFramebuffer->BindOGLFramebuffer(currentTextureIndex));
		CR(m_pOGLFramebuffer->SetAndClearViewport());

	Error:
		return r;
	}

	RESULT UnsetRenderSurface() {
		RESULT r = R_PASS;

		CR(m_pOGLFramebuffer->UnbindOGLFramebuffer());

	Error:
		return R_PASS;
	}

private:
	ovrSession m_ovrSession;

	ovrTextureSwapChainDesc m_ovrTextureSwapChainDescription;
	ovrTextureSwapChain  m_ovrTextureSwapChain;

	std::vector<OGLTexture*> m_swapChainOGLTextures;
	OGLFramebuffer *m_pOGLFramebuffer;

	GLuint	m_framebufferIndex;
	
	int m_width;
	int m_height;
	int m_channels;
	int m_sampleCount;
	int m_mipLevels;

	int m_textureSwapChainLength;
	
	OpenGLImp *m_pParentImp;
};

#endif // !OVR_TEXTURE_SWAP_CHAIN_H_
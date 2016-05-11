#ifndef OVR_TEXTURE_SWAP_CHAIN_H_
#define OVR_TEXTURE_SWAP_CHAIN_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVRTextureSwapChain.h
// The Oculus uses a texture swap chain to pass images to the HMD and doesn't fit 
// into a clear cut representation of a single texture / buffer so this class 
// should wrap up the respective frame and depth buffers as well as the respective textures in the 
// chain

// TODO: Once this is working, replace all parts using OGL objects

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLDepthbuffer.h"
#include "OVR_CAPI_GL.h"

#include "HAL/opengl/OGLTexture.h"
#include "HAL/opengl/OGLFramebuffer.h"

#include <vector>

class OVRTextureSwapChain {

public:
	struct OVRTextureBuffer {
		ovrSession ovrSession;
		ovrTextureSwapChain ovrTextureChain;
		GLuint textureIndex;
		GLuint framebufferIndex;
		ovrSizei textureSize;
	};

public:
	OVRTextureSwapChain(OpenGLImp *pParentImp, ovrSession session, int width, int height, int mipLevels, unsigned char *data, int sampleCount) :
		m_ovrSession(session),
		m_ovrTextureSwapChain(nullptr),
		m_width(width),
		m_height(height),
		m_pOGLFramebuffer(nullptr),
		m_framebufferIndex(0),
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

		// TODO: Replace with OGLTexture
		for (int i = 0; i < m_textureSwapChainLength; ++i) {
			GLuint chainTextureIndex;
			ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, i, &chainTextureIndex);

			OGLTexture *pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, chainTextureIndex, m_width, m_height, 3);
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

		// TODO: Replace with Framebuffer
		m_pParentImp->glGenFramebuffers(1, &m_framebufferIndex);

		m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp);

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

	RESULT SetAndClearRenderSurface(OGLDepthbuffer *oglDepthbuffer) {
		RESULT r = R_PASS;

		GLuint curTexId;

		if (m_ovrTextureSwapChain) {
			int curIndex;
			CR((RESULT)ovr_GetTextureSwapChainCurrentIndex(m_ovrSession, m_ovrTextureSwapChain, &curIndex));
			CR((RESULT)ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, curIndex, &curTexId));
		}
		else {
			// TODO:
			// curTexId = m_textureIndex;
		}

		CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, oglDepthbuffer->GetOGLTextureIndex(), 0));
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0));

		glViewport(0, 0, m_width, m_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_FRAMEBUFFER_SRGB);

	Error:
		return r;
	}

	RESULT UnsetRenderSurface() {
		RESULT r = R_PASS;

		CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex));
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
		CR(m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0));

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
	int m_sampleCount;
	int m_mipLevels;

	int m_textureSwapChainLength;
	
	OpenGLImp *m_pParentImp;
};

#endif // !OVR_TEXTURE_SWAP_CHAIN_H_
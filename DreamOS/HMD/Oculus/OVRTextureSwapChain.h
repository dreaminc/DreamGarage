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
#include "OVR_CAPI_GL.h"

class OVRTextureSwapChain {
public:
	OVRTextureSwapChain(OpenGLImp *pParentImp, ovrSession session, int width, int height, int mipLevels, unsigned char *data, int sampleCount) :
		m_ovrSession(session),
		m_ovrTextureSwapChain(nullptr),
		m_width(width),
		m_height(height),
		m_textureIndex(0),
		m_framebufferIndex(0),
		m_pParentImp(pParentImp)
	{
		assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		ovrTextureSwapChainDesc textureSwapChainDescription = {};
		textureSwapChainDescription.Type = ovrTexture_2D;
		textureSwapChainDescription.ArraySize = 1;
		textureSwapChainDescription.Width = m_width;
		textureSwapChainDescription.Height = m_height;
		textureSwapChainDescription.MipLevels = 1;
		textureSwapChainDescription.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		textureSwapChainDescription.SampleCount = 1;
		textureSwapChainDescription.StaticImage = ovrFalse;

		ovrResult result = ovr_CreateTextureSwapChainGL(m_ovrSession, &textureSwapChainDescription, &m_ovrTextureSwapChain);

		int length = 0;
		ovr_GetTextureSwapChainLength(session, m_ovrTextureSwapChain, &length);

		if (OVR_SUCCESS(result)) {
			for (int i = 0; i < length; ++i) {
				GLuint chainTextureIndex;
				ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, i, &chainTextureIndex);
				glBindTexture(GL_TEXTURE_2D, chainTextureIndex);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
		}

		/* TODO (implementation fill out)
		if (mipLevels > 1) {
			pParentImp->glGenerateMipmap(GL_TEXTURE_2D);
		}
		*/

		pParentImp->glGenFramebuffers(1, &m_framebufferIndex);
	}

	RESULT Commit() {
		if (m_ovrTextureSwapChain)
			return (RESULT)ovr_CommitTextureSwapChain(m_ovrSession, m_ovrTextureSwapChain);
		else
			return R_FAIL;
	}

	RESULT SetAndClearRenderSurface(OGLDepthbuffer *oglDepthbuffer) {
		GLuint curTexId;
		if (m_ovrTextureSwapChain) {
			int curIndex;
			ovr_GetTextureSwapChainCurrentIndex(m_ovrSession, m_ovrTextureSwapChain, &curIndex);
			ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureSwapChain, curIndex, &curTexId);
		}
		else {
			curTexId = m_textureIndex;
		}

		m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex);
		m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, oglDepthbuffer->GetOGLTextureIndex, 0);
		m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);

		glViewport(0, 0, m_width, m_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	RESULT UnsetRenderSurface() {
		m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIndex);
		m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		m_pParentImp->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

		return R_PASS;
	}

private:
	OpenGLImp *m_pParentImp;

	ovrSession m_ovrSession;
	ovrTextureSwapChain  m_ovrTextureSwapChain;
	GLuint	m_textureIndex;
	GLuint	m_framebufferIndex;
	
	int m_width;
	int m_height;
};

#endif // !OVR_TEXTURE_SWAP_CHAIN_H_
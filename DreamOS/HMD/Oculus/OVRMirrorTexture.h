#ifndef OVR_MIRROR_TEXTURE_H_
#define OVR_MIRROR_TEXTURE_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVRMirrorTexture.h
// The Oculus Mirror Texture API allows for pulling out the view presented on the HMD
// so it's possible to inspect the output of the compositor on the machine (good for debug)

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLDepthbuffer.h"
#include "OVR_CAPI_GL.h"

#include "HAL/opengl/OGLTexture.h"
#include "HAL/opengl/OGLFramebuffer.h"

#define DEFAULT_MIRROR_TEXTURE_CHANNELS 3

class OVRMirrorTexture {
public:
	OVRMirrorTexture(OpenGLImp *pParentImp, ovrSession session, int width, int height) :
		m_pParentImp(pParentImp),
		m_ovrSession(session),
		m_width(width),
		m_height(height),
		m_channels(DEFAULT_MIRROR_TEXTURE_CHANNELS),
		m_ovrMirrorTexture(nullptr),
		m_pOGLFramebuffer(nullptr),
		m_pOGLTexture(nullptr)
	{
		// empty
	}

	~OVRMirrorTexture() {
		if (m_pOGLFramebuffer != nullptr) {
			delete m_pOGLFramebuffer;
			m_pOGLFramebuffer = nullptr;
		}

		if (m_pOGLTexture != nullptr) {
			delete m_pOGLTexture;
			m_pOGLTexture = nullptr;
		}
	}

	RESULT OVRInitialize() {
		RESULT r = R_PASS;

		memset(&m_ovrMirrorTextureDescription, 0, sizeof(ovrMirrorTextureDesc));

		m_ovrMirrorTextureDescription.Width = m_width;
		m_ovrMirrorTextureDescription.Height = m_height;
		m_ovrMirrorTextureDescription.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

		CR((RESULT)ovr_CreateMirrorTextureGL(m_ovrSession, &m_ovrMirrorTextureDescription, &m_ovrMirrorTexture));

		GLuint textureIndex;
		CR((RESULT)ovr_GetMirrorTextureBufferGL(m_ovrSession, m_ovrMirrorTexture, &textureIndex));

		m_pOGLTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_COLOR, textureIndex, m_width, m_height, 3);
		CN(m_pOGLTexture);
		CR(m_pOGLTexture->OGLInitialize());

		m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, textureIndex, m_width, m_height, m_channels);
		CN(m_pOGLFramebuffer);
		CR(m_pOGLFramebuffer->OGLInitialize());
		CR(m_pOGLFramebuffer->BindOGLFramebuffer(textureIndex));

		CR(m_pParentImp->glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

		// Turn off vsync to let the compositor do its magic
		CR(m_pParentImp->wglSwapIntervalEXT(0));

	Error:
		return r;
	}

	RESULT RenderMirrorToBackBuffer() {
		RESULT r = R_PASS;

		// TODO: Move this to framebuffer
		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pOGLFramebuffer->GetFramebufferIndex()));
		CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

		CR(m_pParentImp->glBlitFramebuffer(0, m_height, m_width, 0, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

	Error:
		return r;
	}

private:
	OpenGLImp *m_pParentImp;
	ovrSession m_ovrSession;

	ovrMirrorTextureDesc m_ovrMirrorTextureDescription;
	ovrMirrorTexture m_ovrMirrorTexture;

	OGLFramebuffer *m_pOGLFramebuffer;
	OGLTexture *m_pOGLTexture;

	int m_width;
	int m_height;
	int m_channels;
};

#endif // ! OVR_MIRROR_TEXTURE_H_
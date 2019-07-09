#ifndef OVR_MIRROR_TEXTURE_H_
#define OVR_MIRROR_TEXTURE_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVRMirrorTexture.h
// The Oculus Mirror Texture API allows for pulling out the view presented on the HMD
// so it's possible to inspect the output of the compositor on the machine (good for debug)

#define DEFAULT_MIRROR_TEXTURE_CHANNELS 3

#include "OVR_CAPI_GL.h"

class OpenGLImp;
class OGLFramebuffer;
class OGLTexture;

class OVRMirrorTexture {
public:
	OVRMirrorTexture(OpenGLImp *pParentImp, ovrSession session, int width, int height);
	~OVRMirrorTexture();

	RESULT OVRInitialize();
	RESULT RenderMirrorToBackBuffer();
	RESULT DestroyMirrorTexture();

private:
	OpenGLImp *m_pParentImp;
	ovrSession m_ovrSession;

	ovrMirrorTextureDesc m_ovrMirrorTextureDescription;
	ovrMirrorTexture m_pOVRMirrorTexture;

	OGLFramebuffer *m_pOGLFramebuffer;
	OGLTexture *m_pOGLTexture;

	int m_width;
	int m_height;
	int m_channels;
};

#endif // ! OVR_MIRROR_TEXTURE_H_
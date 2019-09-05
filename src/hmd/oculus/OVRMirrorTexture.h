#ifndef OVR_MIRROR_TEXTURE_H_
#define OVR_MIRROR_TEXTURE_H_

#include "core/ehm/EHM.h"

// Dream OS
// dos/src/hmd/oculus/OVRMirrorTexture.h

// The Oculus Mirror Texture API allows for pulling out the view presented on the HMD
// so it's possible to inspect the output of the compositor on the machine (good for debug)

#define DEFAULT_MIRROR_TEXTURE_CHANNELS 3

#include "OVR_CAPI_GL.h"

#include "core/types/DObject.h"

class OGLImp;
class OGLFramebuffer;
class OGLTexture;

class OVRMirrorTexture : public DObject {
public:
	OVRMirrorTexture(OGLImp *pParentImp, ovrSession session, int width, int height);
	~OVRMirrorTexture();

	RESULT OVRInitialize();
	RESULT RenderMirrorToBackBuffer();
	RESULT DestroyMirrorTexture();

private:
	OGLImp *m_pParentImp = nullptr;
	ovrSession m_ovrSession;

	ovrMirrorTextureDesc m_ovrMirrorTextureDescription;
	ovrMirrorTexture m_pOVRMirrorTexture;

	OGLFramebuffer *m_pOGLFramebuffer = nullptr;
	OGLTexture *m_pOGLTexture = nullptr;

	int m_width;
	int m_height;
	int m_channels;
};

#endif // ! OVR_MIRROR_TEXTURE_H_
#ifndef OVR_TEXTURE_SWAP_CHAIN_H_
#define OVR_TEXTURE_SWAP_CHAIN_H_

#include "core/ehm/EHM.h"

// Dream HMD Oculus
// dos/src/hmd/oculus/OVRTextureSwapChain.h

// The Oculus uses a texture swap chain to pass images to the HMD and doesn't fit 
// into a clear cut representation of a single texture / buffer so this class 
// should wrap up the respective frame and depth buffers as well as the respective textures in the 
// chain

#include <vector>

#include "OVR_CAPI_GL.h"

#include "core/types/DObject.h"

#include "hal/ogl/OGLImp.h"
#include "hal/ogl/OGLAttachment.h"
#include "hal/ogl/OGLTexture.h"
#include "hal/ogl/OGLFramebuffer.h"

#define DEFAULT_TEXTURE_SWAP_CHAIN_CHANNELS 3
#define DEFAULT_OVR_MULTI_SAMPLE 4

class OVRTextureSwapChain : public DObject {
	friend class OVRHMDSinkNode;

public:
	OVRTextureSwapChain(OGLImp *pParentImp, ovrSession session, int width, int height, int mipLevels, unsigned char *data, int sampleCount);
	~OVRTextureSwapChain();

	RESULT OVRInitialize();

	ovrTextureSwapChain GetOVRTextureSwapChain();
	
	ovrSizei GetOVRSizei();
	ovrRecti GetOVRViewportRecti();

	RESULT Commit();

	RESULT SetAndClearRenderSurface();

	RESULT UnsetRenderSurface();

	RESULT DestroySwapChainTexture();

private:
	ovrSession m_ovrSession;

	ovrTextureSwapChainDesc m_ovrTextureSwapChainDescription;
	ovrTextureSwapChain  m_pOVRTextureSwapChain;

protected:
	std::vector<OGLTexture*> m_swapChainOGLTextures;
	
	OGLFramebuffer *m_pOGLResolveFramebuffer = nullptr;
	OGLFramebuffer *m_pOGLRenderFramebuffer = nullptr;

	GLuint	m_framebufferIndex;
	
	int m_width;
	int m_height;
	int m_channels;
	int m_sampleCount;
	int m_mipLevels;

	int m_textureSwapChainLength;
	
	OGLImp *m_pParentImp = nullptr;
};

#endif // !OVR_TEXTURE_SWAP_CHAIN_H_
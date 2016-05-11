#ifndef OVR_H_
#define OVR_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVR.h
// The Oculus Rift headset class - might sub class to other rift versions over time

// TODO: Should this go into Sense?

#include "HMD/HMD.h"
#include "OVRTextureSwapChain.h"
#include "HAL/opengl/OGLDepthbuffer.h"

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMNIG

//#include "External\OCULUS\v132\LibOVR\Include\OVR_CAPI.h"
//#pragma comment(lib, "External/OCULUS/v132/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib")

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

#include <vector>
#include <algorithm>

#include "HAL/opengl/OGLFramebuffer.h"
#include "OVRMirrorTexture.h"

class OGLFramebuffer;

class OVR : public HMD {
public:
	OVR();
	~OVR();

	//RESULT InitializeHMD(HALImp *halimp);
	RESULT InitializeHMD(HALImp *halimp, int wndWidth = 0, int wndHeight = 0);
	RESULT UpdateHMD();
	RESULT ReleaseHMD();

	RESULT SetUpFrame();
	RESULT BindFramebuffer(EYE_TYPE eye);
	RESULT CommitSwapChain(EYE_TYPE eye);
	RESULT SubmitFrame();

	RESULT SetAndClearRenderSurface(EYE_TYPE eye);
	RESULT UnsetRenderSurface(EYE_TYPE eye);

	RESULT RenderHMDMirror();

public:
	ovrSession m_ovrSession;
	ovrHmdDesc m_ovrHMDDescription;
	ovrEyeRenderDesc m_ovrEyeRenderDescription[2];
	std::vector<ovrTrackerDesc>   m_TrackerDescriptions;
	ovrLayerEyeFov m_ovrLayer;

	// Texture Swap Chains
	OVRTextureSwapChain *m_ovrTextureSwapChains[HMD_NUM_EYES];
	OVRMirrorTexture *m_ovrMirrorTexture;
	//OGLDepthbuffer *m_depthbuffers[HMD_NUM_EYES];		// TODO: Push this into the swap chain
};

#endif // ! HMD_IMP_H_
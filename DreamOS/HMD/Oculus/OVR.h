#ifndef OVR_H_
#define OVR_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVR.h
// The Oculus Rift headset class - might sub class to other rift versions over time

// TODO: Should this go into Sense?

#include "HMD/HMD.h"

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMNIG

//#include "External\OCULUS\v132\LibOVR\Include\OVR_CAPI.h"
//#pragma comment(lib, "External/OCULUS/v132/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib")

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

#include <vector>
#include <algorithm>

#include "HAL/opengl/OGLFramebuffer.h"

class OVR : public HMD {
public:
	OVR();
	~OVR();

	RESULT InitializeHMD();
	RESULT UpdateHMD();
	RESULT ReleaseHMD();

public:
	ovrSession m_ovrSession;
	ovrHmdDesc m_ovrHMDDescription;
	std::vector<ovrTrackerDesc>   m_TrackerDescriptions;

	// Swap Chain
	ovrTextureSwapChainDesc m_ovrTextureSwapChainDescription;
	ovrTextureSwapChain  m_ovrTextureChain;
	int m_ovrSwapChainLength;

	// Framebuffers

};

#endif // ! HMD_IMP_H_
#ifndef OPENVR_DEVICE_H_
#define OPENVR_DEVICE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/OpenVR/OpenVRDevice.h
// OpenVR device encapsulates the OpenVR device which can be any HMD apparently.  
// Personally not a fan of this architecture, as it plugs into the SteamVR eco-system only, so it's not possible
// to use the headset stand alone - but it allows maxiumum flexibility in using Dream across Oculus/Steam regardless
// of the headset being used

#include <openvr.h>

// TODO: Should this go into Sense?

#include "HMD/HMD.h"
#include "HAL/opengl/OGLDepthbuffer.h"

#define DEFAULT_OPENVR_CHANNELS 3

#include <vector>
#include <algorithm>

#include "HAL/opengl/OGLFramebuffer.h"

// IVR EHM extension

#define CIVR(ivrr) do{ivrResult=(ivrr);if(ivrResult != vr::VRInitError_None){goto Error;}}while(0);
#define CIVRM(ivrr, msg, ...) do{ivrResult=(ivrr);if(ivrResult != vr::VRInitError_None){DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Error: %s\n",vr::VR_GetVRInitErrorAsEnglishDescription(ivrResult));goto Error;}}while(0);

class OpenVRDevice : public HMD {
public:
	OpenVRDevice();
	~OpenVRDevice();

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

	ProjectionMatrix GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar);
	ViewMatrix GetViewMatrix(EYE_TYPE eye);

private:
	std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL);
	RESULT InitializeFrameBuffer(EYE_TYPE eye, uint32_t nWidth, uint32_t nHeight);
	RESULT SetupStereoRenderTargets();

public:
	vr::IVRSystem *m_pIVRHMD;
	vr::IVRRenderModels *m_pRenderModels;
	vr::IVRCompositor *m_pCompositor;

	std::string m_strDriver;
	std::string m_strDisplay;

	OGLFramebuffer *m_pFramebufferLeft;
	OGLFramebuffer *m_pFramebufferRight;
};

#endif // ! OPENVR_DEVICE_H_
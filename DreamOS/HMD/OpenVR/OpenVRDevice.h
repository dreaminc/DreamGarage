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

#define DEFAULT_OPENVR_RENDER_CHANNELS 3
#define DEFAULT_OPENVR_RESOLVE_CHANNELS 4
#define DEFAULT_OPENVR_MULTISAMPLE 4

#include <vector>
#include <algorithm>

#include "HAL/opengl/OGLFramebuffer.h"

// TODO: Temp for testing
#include "External/Matrices/Matrices.h"

#include "Sense/SenseController.h"

// IVR EHM extension

#define CIVR(ivrr) do{ivrResult=(ivrr);if(ivrResult != vr::VRInitError_None){goto Error;}}while(0);
#define CIVRM(ivrr, msg, ...) do{ivrResult=(ivrr);if(ivrResult != vr::VRInitError_None){DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Error: %s\n",vr::VR_GetVRInitErrorAsEnglishDescription(ivrResult));goto Error;}}while(0);

class OpenVRDevice : public HMD {
public:
	OpenVRDevice(SandboxApp *pParentSandbox);
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
	RESULT AttachHand(hand *pHand, hand::HAND_TYPE type);
	hand* GetHand(hand::HAND_TYPE type);
	SenseController* GetSenseController();

private:
	std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL);
	RESULT InitializeFrameBuffer(EYE_TYPE eye, uint32_t nWidth, uint32_t nHeight);
	RESULT SetupStereoRenderTargets();
	RESULT HandleVREvent(vr::VREvent_t event);
	//float PredictSecondsToPhotons(float secondOffset = 0.0f);
	Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);
	ViewMatrix ConvertSteamVRMatrixToViewMatrix(const vr::HmdMatrix34_t &matPose);

	// Models
	RESULT InitializeRenderModels();								// This sets up the models
	RESULT InitializeRenderModel(uint32_t deviceID);	// This sets up a given model
	RESULT SetControllerModelTexture(model *pModel, texture *pTexture, vr::ETrackedControllerRole controllerRole);

	RESULT UpdateSenseController(vr::ETrackedControllerRole controllerRole, vr::VRControllerState_t state);

public:
	vr::IVRSystem *m_pIVRHMD;
	vr::IVRCompositor *m_pCompositor;

	std::string m_strDriver;
	std::string m_strDisplay;

	OGLFramebuffer *m_pFramebufferRenderLeft;
	OGLFramebuffer *m_pFramebufferResolveLeft;
	OGLFramebuffer *m_pFramebufferRenderRight;
	OGLFramebuffer *m_pFramebufferResolveRight;

	// Pose / device tracking
	int m_trackedControllerCount;
	int m_trackedControllerCount_Last;
	int m_validPoseCount;
	int m_validPoseCount_Last;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	std::string m_strPoseClasses;                            // what classes we saw poses for this frame

	bool m_fVblank;
	bool m_fGlFinishHack;

	// Device Render Models
	vr::IVRRenderModels *m_pRenderModels;
	
	model *m_pControllerModelLeft;
	texture *m_pControllerModelLeftTexture;
	hand *m_pLeftHand;
	
	model *m_pControllerModelRight;
	texture *m_pControllerModelRightTexture;
	hand *m_pRightHand;

	SenseController* m_pSenseController;

	uint32_t ovrFrame;

	model *m_pHMDModel;
};

#endif // ! OPENVR_DEVICE_H_
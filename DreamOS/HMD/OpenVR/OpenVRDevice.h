#ifndef OPENVR_DEVICE_H_
#define OPENVR_DEVICE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/OpenVR/OpenVRDevice.h
// OpenVR device encapsulates the OpenVR device which can be any HMD apparently.  
// Personally not a fan of this architecture, as it plugs into the SteamVR eco-system only, so it's not possible
// to use the headset stand alone - but it allows maxiumum flexibility in using Dream across Oculus/Steam regardless
// of the headset being used
#ifndef OCULUS_PRODUCTION_BUILD
#include <openvr.h>

// TODO: Should this go into Sense?

#include "HMD/HMD.h"
#include "HAL/opengl/OGLAttachment.h"

#define DEFAULT_OPENVR_RENDER_CHANNELS 3
#define DEFAULT_OPENVR_RESOLVE_CHANNELS 4
#define DEFAULT_OPENVR_MULTISAMPLE 4

#include <vector>
#include <algorithm>

// TODO: Temp for testing
#include "External/Matrices/Matrices.h"

#include "Sense/SenseController.h"

// IVR EHM extension

#define CIVR(ivrr) do{ivrResult=(ivrr);if(ivrResult != vr::VRInitError_None){goto Error;}}while(0);
#define CIVRM(ivrr, msg, ...) do{ivrResult=(ivrr);if(ivrResult != vr::VRInitError_None){DEBUG_OUT(CurrentFileLine);DOSLogErrorMessage("CIVRM", ivrResult, msg, ##__VA_ARGS__);DEBUG_OUT("Error: %s\n",vr::VR_GetVRInitErrorAsEnglishDescription(ivrResult));goto Error;}}while(0);

class OpenVRDevice : public HMD {
	friend class OpenVRHMDSinkNode;

public:
	OpenVRDevice(SandboxApp *pParentSandbox);
	~OpenVRDevice();

	// TODO: Do this for vive
	virtual HMDSinkNode *GetHMDSinkNode() override;
	virtual HMDSourceNode *GetHMDSourceNode() override { return nullptr; }
	virtual RESULT InitializeHMDSourceNode() override;
	virtual RESULT InitializeHMDSinkNode() override;

	virtual RESULT InitializeHMD(HALImp *halimp, int wndWidth = 0, int wndHeight = 0, bool fHMDMirror = true) override;
	virtual RESULT UpdateHMD() override;
	virtual RESULT ReleaseHMD() override;

	virtual RESULT SetUpFrame() override;
	virtual RESULT BindFramebuffer(EYE_TYPE eye) override;
	virtual RESULT CommitSwapChain(EYE_TYPE eye) override;
	virtual RESULT SubmitFrame() override;

	virtual RESULT SetAndClearRenderSurface(EYE_TYPE eye) override;
	virtual RESULT UnsetRenderSurface(EYE_TYPE eye) override;

	virtual RESULT RenderHMDMirror() override;
	virtual RESULT RecenterHMD() override;

	virtual ProjectionMatrix GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) override;
	virtual ViewMatrix GetViewMatrix(EYE_TYPE eye) override;

	virtual composite *GetSenseControllerObject(ControllerType controllerType) override;
	virtual HMDDeviceType GetDeviceType() override;
	virtual bool IsARHMD() override;
	virtual std::string GetDeviceTypeString() override;

	virtual bool IsHMDTracked() {
		return true;
	}

	virtual RESULT GetAudioDeviceOutID(std::wstring &wstrAudioDeviceOutGUID) override;
	virtual RESULT GetAudioDeviceInGUID(std::wstring &wstrAudioDeviceInGUID) override;

private:
	std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL);
	RESULT HandleVREvent(vr::VREvent_t event);
	//float PredictSecondsToPhotons(float secondOffset = 0.0f);
	Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);
	ViewMatrix ConvertSteamVRMatrixToViewMatrix(const vr::HmdMatrix34_t &matPose);

	RESULT InitializeDeviceType();

	// Models
	RESULT InitializeRenderModels();								// This sets up the models
	RESULT InitializeRenderModel(uint32_t deviceID);	// This sets up a given model
	RESULT SetControllerMeshTexture(mesh *pMesh, texture *pTexture, vr::ETrackedControllerRole controllerRole);

	RESULT UpdateSenseController(vr::ETrackedControllerRole controllerRole, vr::VRControllerState_t state);

public:
	vr::IVRSystem *m_pIVRHMD;

	std::string m_strDriver;
	std::string m_strDisplay;
	std::string m_strName;
	HMDDeviceType m_deviceType;

	// Pose / device tracking
	int m_trackedControllerCount;
	int m_trackedControllerCount_Last;
	int m_validPoseCount;
	int m_validPoseCount_Last;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	std::string m_strPoseClasses;                            // what classes we saw poses for this frame

	// Device Render Models
	vr::IVRRenderModels *m_pRenderModels;

	composite *m_pLeftController = nullptr;
	std::shared_ptr<mesh> m_pControllerMeshLeft = nullptr;
	texture *m_pControllerMeshLeftTexture = nullptr;
	
	composite *m_pRightController = nullptr;
	std::shared_ptr<mesh> m_pControllerMeshRight = nullptr;
	texture *m_pControllerMeshRightTexture = nullptr;

	uint32_t m_vrFrameCount;

	model *m_pHMDModel;

private:
	OpenVRHMDSinkNode *m_pOpenVRHMDSinkNode = nullptr;
};
#endif
#endif // ! OPENVR_DEVICE_H_
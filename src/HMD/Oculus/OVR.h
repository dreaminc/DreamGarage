#ifndef OVR_H_
#define OVR_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVR.h
// The Oculus Rift headset class - might sub class to other rift versions over time

// TODO: Should this go into Sense?

#include "HMD/HMD.h"
#include "HAL/opengl/OGLAttachment.h"

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMING

//#include "External\OCULUS\v132\LibOVR\Include\OVR_CAPI.h"
//#pragma comment(lib, "External/OCULUS/v132/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib")

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

#include <vector>
#include <algorithm>

#include "HAL/opengl/OGLFramebuffer.h"
#include "OVRMirrorTexture.h"

class OGLFramebuffer;
class OVRPlatform;

class OVRHMD : public HMD {
	friend class OVRHMDSinkNode;

public:
	OVRHMD(SandboxApp *pParentSandbox);
	~OVRHMD();

	virtual HMDSinkNode *GetHMDSinkNode() override;
	virtual HMDSourceNode *GetHMDSourceNode() override;
	virtual RESULT InitializeHMDSourceNode() override;
	virtual RESULT InitializeHMDSinkNode() override;

	//RESULT InitializeHMD(HALImp *halimp);
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

	virtual ProjectionMatrix GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) override;
	virtual ViewMatrix GetViewMatrix(EYE_TYPE eye) override;

	virtual bool IsHMDTracked() override;

	virtual composite *GetSenseControllerObject(ControllerType controllerType) override;
	virtual HMDDeviceType GetDeviceType() override;
	virtual bool IsARHMD() override;
	virtual std::string GetDeviceTypeString() override;

	virtual RESULT RecenterHMD() override;

public:

	RESULT ShutdownParentSandbox();

	virtual RESULT GetAudioDeviceOutID(std::wstring &wstrAudioDeviceOutGUID) override;
	virtual RESULT GetAudioDeviceInGUID(std::wstring &wstrAudioDeviceInGUID) override;

protected:
	inline const ovrSession &GetOVRSession() { return m_ovrSession; }
	inline const ovrHmdDesc &GetOVRHMDDescription() { return m_ovrHMDDescription; }

private:
	RESULT UpdateSenseController(ovrControllerType type, ovrInputState& inputState);

public:
	ovrSession m_ovrSession;
	ovrHmdDesc m_ovrHMDDescription;
	std::vector<ovrTrackerDesc> m_TrackerDescriptions;

	OVRPlatform* m_pOVRPlatform = nullptr;

	// Mirror Texture (TODO: Move to separate sink node)
	OVRMirrorTexture *m_pOVRMirrorTexture = nullptr;
	//OGLDepthbuffer *m_depthbuffers[HMD_NUM_EYES];		// TODO: Push this into the swap chain

	quaternion qLeftRotation;
	quaternion qRightRotation;

	composite *m_pLeftControllerModel = nullptr;
	composite *m_pRightControllerModel = nullptr;

private:
	OVRHMDSinkNode *m_pOVRHMDSinkNode = nullptr;
	//OVRHMDSourceNode *m_pOVRHMDSourceNode = nullptr;
	
};

#endif // ! HMD_IMP_H_

#ifndef OPENVR_HMD_SINK_NODE_H_
#define OPENVR_HMD_SINK_NODE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/OpenVR/OpenVRHMDSinkNode.h
// The Open VR HMD Sink Node class 

#include "HMD/HMD.h"
#include "HMD/HMDSinkNode.h"
#define MS_90_FPS (1.0f / 90.0f)

#ifndef OCULUS_PRODUCTION_BUILD

// Include the Oculus SDK
#include <openvr.h>

class OpenGLImp;
class OpenVRDevice;
class OGLFramebuffer;

class OpenVRHMDSinkNode : public HMDSinkNode {
public:
	OpenVRHMDSinkNode(OpenGLImp *pOGLImp, OpenVRDevice *pParentHMD);
	~OpenVRHMDSinkNode();

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;

	RESULT UnsetRenderSurface(EYE_TYPE eye);
	RESULT SubmitFrame();

	virtual RESULT RenderNode(long frameID = 0) override;

private:
	RESULT RenderMirrorToBackBuffer();

private:
	OpenVRDevice *m_pParentHMD = nullptr;
	OpenGLImp *m_pParentImp = nullptr;

	DConnection *m_pInputConnection[HMD_NUM_EYES] = { nullptr };

	OGLFramebuffer *m_pOGLInputFramebuffer = nullptr;

private:
	vr::IVRCompositor *m_pCompositor = nullptr;

	bool m_fVblank;
	bool m_fGlFinishHack;

	double m_msTimeSpentOnRenderAvg = 1.0;	// because starting with 0 will get us no where
	double m_weightOnAverage = 0.9f;		// how much fast or slow we respond to changes in render speed
	double m_fpsPadding = 0.9f;				// gives an extra bit of time buffer for us in case rendering takes long 
};
#endif

#endif	// ! OPENVR_HMD_SINK_NODE_H_
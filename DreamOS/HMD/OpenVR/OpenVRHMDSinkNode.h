#ifndef OPENVR_HMD_SINK_NODE_H_
#define OPENVR_HMD_SINK_NODE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/OpenVR/OpenVRHMDSinkNode.h
// The Open VR HMD Sink Node class 

#include "HMD/HMD.h"
#include "HMD/HMDSinkNode.h"

// Include the Oculus SDK
//#include "OVR_CAPI_GL.h"

class OpenGLImp;
class OpenVRDevice;

class OpenVRHMDSinkNode : public HMDSinkNode {
public:
	OpenVRHMDSinkNode(OpenGLImp *pOGLImp, OpenVRDevice *pParentHMD);
	~OpenVRHMDSinkNode();

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;

	RESULT CommitSwapChain(EYE_TYPE eye);
	RESULT SetAndClearRenderSurface(EYE_TYPE eye);
	RESULT UnsetRenderSurface(EYE_TYPE eye);
	RESULT SubmitFrame();

	virtual RESULT RenderNode(long frameID = 0) override;

private:
	OpenVRDevice *m_pParentHMD = nullptr;
	OpenGLImp *m_pParentImp = nullptr;

	DConnection *m_pInputConnection[HMD_NUM_EYES] = { nullptr };
};


#endif	// ! OPENVR_HMD_SINK_NODE_H_
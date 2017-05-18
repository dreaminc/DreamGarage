#ifndef OVR_HMD_SINK_NODE_H_
#define OVR_HMD_SINK_NODE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVRHMDSinkNode.h
// The Oculus VR HMD Sink Node class 

#include "HMD/HMD.h"
#include "HMD/HMDSinkNode.h"

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

class OpenGLImp;
class OVRHMD;
class OVRTextureSwapChain;

class OVRHMDSinkNode : public HMDSinkNode {
public:
	OVRHMDSinkNode(OpenGLImp *pOGLImp, OVRHMD *pParentHMD);
	~OVRHMDSinkNode();

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID = 0) override;

	RESULT CommitSwapChain(EYE_TYPE eye);
	RESULT SetAndClearRenderSurface(EYE_TYPE eye);
	RESULT UnsetRenderSurface(EYE_TYPE eye);
	RESULT SubmitFrame();

private:
	// Texture Swap Chains
	OVRTextureSwapChain *m_ovrTextureSwapChains[HMD_NUM_EYES] = { nullptr };
	ovrEyeRenderDesc m_ovrEyeRenderDescription[HMD_NUM_EYES];
	ovrLayerEyeFov m_ovrLayer;

private:
	OVRHMD *m_pParentHMD = nullptr;
	OpenGLImp *m_pParentImp = nullptr;
};


#endif	// ! OVR_HMD_SINK_NODE_H_
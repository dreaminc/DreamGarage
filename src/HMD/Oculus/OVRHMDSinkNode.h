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

#define MS_90_FPS (1.0f / 90.0f)

class OpenGLImp;
class OVRHMD;
class OVRTextureSwapChain;

class OVRHMDSinkNode : public HMDSinkNode {
public:
	OVRHMDSinkNode(OpenGLImp *pOGLImp, OVRHMD *pParentHMD);
	~OVRHMDSinkNode();

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;

	RESULT CommitSwapChain(EYE_TYPE eye);
	RESULT SetAndClearRenderSurface(EYE_TYPE eye);
	RESULT UnsetRenderSurface(EYE_TYPE eye);
	RESULT SubmitFrame();
	RESULT DestroySwapChainTexture();

	virtual RESULT RenderNode(long frameID = 0) override;

private:
	// Texture Swap Chains
	OVRTextureSwapChain *m_ovrTextureSwapChains[HMD_NUM_EYES] = { nullptr };
	ovrEyeRenderDesc m_ovrEyeRenderDescription[HMD_NUM_EYES];
	ovrLayerEyeFov m_ovrLayer;

private:
	OVRHMD *m_pParentHMD = nullptr;
	OpenGLImp *m_pParentImp = nullptr;

	DConnection *m_pInputConnection[HMD_NUM_EYES] = { nullptr };

	double m_msTimeSpentOnRenderAvg = 1.0;	// because starting with 0 will get us no where
	double m_weightOnAverage = 0.9f;		// how much fast or slow we respond to changes in render speed
	double m_fpsPadding = 0.9f;				// gives an extra bit of time buffer for us in case rendering takes long 
};


#endif	// ! OVR_HMD_SINK_NODE_H_
#include "OVR.h"
#include <math.h>

// TODO: Fix this encapsulation
#include "HAL/opengl/OpenGLImp.h"

#include "Primitives/stereocamera.h"
#include "Primitives/rectangle.h"

OVR::OVR() :
	m_ovrSession(nullptr)
	//m_ovrTextureChain(nullptr),
	//m_ovrSwapChainLength(0)
{
	// empty stub
}

OVR::~OVR() {
	// empty stub
}

RESULT OVR::InitializeHMD(HALImp *halimp) {
	RESULT r = R_PASS;
	ovrGraphicsLuid luid;
	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(halimp);

	// Initializes LibOVR, and the Rift
	CRM((RESULT)ovr_Initialize(nullptr), "Failed to initialize libOVR.");

	// Attempt to create OVR session
	CRM((RESULT)ovr_Create(&m_ovrSession, &luid), "Failed to create OVR session");

	m_ovrHMDDescription = ovr_GetHmdDesc(m_ovrSession);

	// Get tracker descriptions
	unsigned int trackerCount = std::max<unsigned int>(1, ovr_GetTrackerCount(m_ovrSession));
	for (unsigned int i = 0; i < trackerCount; ++i)
		m_TrackerDescriptions.push_back(ovr_GetTrackerDesc(m_ovrSession, i));

	// Configure Stereo settings.
	ovrSizei recommenedTex0Size = ovr_GetFovTextureSize(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[0], 1.0f);
	ovrSizei recommenedTex1Size = ovr_GetFovTextureSize(m_ovrSession, ovrEye_Right, m_ovrHMDDescription.DefaultEyeFov[1], 1.0f);
	
	// TODO: Check for mismatch

	//ovrSizei bufferSize;
	//bufferSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
	//bufferSize.h = fmax(recommenedTex0Size.h, recommenedTex1Size.h);
	m_eyeWidth = recommenedTex0Size.w;
	m_eyeHeight = recommenedTex0Size.h;

	/*
	// Set up the OGL Swap Chain
	// TODO: This should be done in a cross platform way
	m_ovrTextureSwapChainDescription.Type = ovrTexture_2D;
	m_ovrTextureSwapChainDescription.ArraySize = 1;
	m_ovrTextureSwapChainDescription.Width = GetEyeWidth();
	m_ovrTextureSwapChainDescription.Height = GetEyeHeight();
	m_ovrTextureSwapChainDescription.MipLevels = 1;
	m_ovrTextureSwapChainDescription.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_ovrTextureSwapChainDescription.SampleCount = 1;
	m_ovrTextureSwapChainDescription.StaticImage = ovrFalse;

	// TODO: Need to set up OpenGL first
	// TODO: This is GL specific 
	CRM((RESULT)ovr_CreateTextureSwapChainGL(m_ovrSession, &m_ovrTextureSwapChainDescription, &m_ovrTextureChain), "Failed to create Texture Swap Chain for OGL");
	CRM((RESULT)ovr_GetTextureSwapChainLength(m_ovrSession, m_ovrTextureChain, &m_ovrSwapChainLength), "Failed to get OVR Swap Chain Length");

	

	// Set up eyes
	// TODO: Use primitives for this instead
	for (int i = 0; i < 2; i++) {
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_ovrSession, ovrEyeType(i), m_ovrHMDDescription.DefaultEyeFov[i], 1);

		GLuint swapChainTextureIndex;
		ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureChain, i, &swapChainTextureIndex);

		// TODO: Set up the framebuffers here
		m_pStereoFramebuffers[i] = new OGLFramebuffer(oglimp, GetEyeWidth(), GetEyeHeight(), 3);
		CRM(m_pStereoFramebuffers[i]->OGLInitialize(swapChainTextureIndex), "Failed to initialzie eye %d framebuffer for HMD", i);
	}
	*/

	for (int i = 0; i < HMD_NUM_EYES; i++) {
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_ovrSession, ovrEyeType(i), m_ovrHMDDescription.DefaultEyeFov[i], 1);
		
		m_ovrTextureSwapChains[i] = new OVRTextureSwapChain(oglimp, m_ovrSession, idealTextureSize.w, idealTextureSize.h, 1, NULL, 1);
		m_depthbuffers[i] = new OGLDepthbuffer(oglimp, idealTextureSize.w, idealTextureSize.h, 0);
	}

	m_ovrEyeRenderDescription[0] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[0]);
	m_ovrEyeRenderDescription[1] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Right, m_ovrHMDDescription.DefaultEyeFov[1]);
	
	/*
	// Implement a single layer
	// Initialize our single full screen Fov layer.
	

	m_ovrLayer.Header.Type = ovrLayerType_EyeFov;
	m_ovrLayer.Header.Flags = 0;
	m_ovrLayer.ColorTexture[0] = m_ovrTextureChain;
	m_ovrLayer.ColorTexture[1] = m_ovrTextureChain;
	m_ovrLayer.Fov[0] = m_ovrEyeRenderDescription[0].Fov;
	m_ovrLayer.Fov[1] = m_ovrEyeRenderDescription[1].Fov;
	m_ovrLayer.Viewport[0] = *(reinterpret_cast<ovrRecti*>(&(rectangle<int>(0, 0, GetEyeWidth(), GetEyeHeight()))));
	m_ovrLayer.Viewport[1] = *(reinterpret_cast<ovrRecti*>(&(rectangle<int>(GetEyeWidth(), 0, GetEyeWidth(), GetEyeHeight()))));
	*/

Error:
	return r;
}

RESULT OVR::BindFramebuffer(EYE_TYPE eye) {
	//return m_pStereoFramebuffers[eye]->BindOGLFramebuffer();
	return R_NOT_IMPLEMENTED;
}

// Commit the changes to the texture swap chain
RESULT OVR::CommitSwapChain(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->Commit();
	/*RESULT r = R_PASS;

	CR((RESULT)ovr_CommitTextureSwapChain(m_ovrSession, m_ovrTextureChain));

Error:
	return r;*/
}

RESULT OVR::SetAndClearRenderSurface(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->SetAndClearRenderSurface(m_depthbuffers[eye]);
	/*RESULT r = R_PASS;

	GLuint curTexId;

	if (m_ovrTextureChain) {
		int currentIndex;
		ovr_GetTextureSwapChainCurrentIndex(m_ovrSession, m_ovrTextureChain, &currentIndex);
		ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureChain, currentIndex, &curTexId);
	}
	else {
		curTexId = m_pStereoFramebuffers[eye]->GetOGLTextureIndex();
	}

	CR(BindFramebuffer(eye));

Error:
	return r;*/
}

RESULT OVR::UnsetRenderSurface(EYE_TYPE eye) {
	//return m_pStereoFramebuffers[eye]->UnbindOGLFramebuffer();
	return m_ovrTextureSwapChains[eye]->UnsetRenderSurface();
}

RESULT OVR::SubmitFrame() {
	RESULT r = R_PASS;

	//ovrLayerHeader* layers = &m_ovrLayer.Header;
	long long frameIndex = 0;

	ovrLayerEyeFov ld;
	ld.Header.Type = ovrLayerType_EyeFov;
	ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

	ovrPosef EyeRenderPose[2];
	ovrVector3f HmdToEyeOffset[2] = { m_ovrEyeRenderDescription[0].HmdToEyeOffset, m_ovrEyeRenderDescription[1].HmdToEyeOffset };

	m_ovrEyeRenderDescription[0] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[0]);
	m_ovrEyeRenderDescription[1] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Right, m_ovrHMDDescription.DefaultEyeFov[1]);

	double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	ovr_GetEyePoses(m_ovrSession, frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	for (int eye = 0; eye < 2; ++eye) {
		ld.ColorTexture[eye] = reinterpret_cast<ovrTextureSwapChain>(m_ovrTextureSwapChains[eye]);
		ld.Viewport[eye] = m_ovrTextureSwapChains[eye]->GetOVRViewportRecti();
		ld.Fov[eye] = m_ovrHMDDescription.DefaultEyeFov[eye];
		ld.RenderPose[eye] = EyeRenderPose[eye];
		ld.SensorSampleTime = sensorSampleTime;
	}

	ovrLayerHeader* layers = &ld.Header;

	CR((RESULT)ovr_SubmitFrame(m_ovrSession, 0, nullptr, &layers, 1));

Error:
	return r;
}

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMNIG

RESULT OVR::UpdateHMD() {
	RESULT r = R_PASS;

#ifdef HMD_OVR_USE_PREDICTED_TIMNIG
	double fTiming = ovr_GetPredictedDisplayTime(m_ovrSession, 0);
#else
	double fTiming = ovr_GetTimeInSeconds();
#endif
	ovrTrackingState trackingState = ovr_GetTrackingState(m_ovrSession, fTiming, true);

	if (trackingState.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
		//ovrPosef headPose = trackingState.HeadPose.ThePose;
		m_ptOrigin = point(reinterpret_cast<float*>(&(trackingState.HeadPose.ThePose.Position)));
		m_qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(trackingState.HeadPose.ThePose.Orientation)));
	}

Error:
	return r;
}

RESULT OVR::ReleaseHMD() {
	RESULT r = R_PASS;

	if (m_ovrSession != nullptr) {
		ovr_Destroy(m_ovrSession);
		m_ovrSession = nullptr;
	}

	/*
	if (m_ovrTextureChain != nullptr) {
		ovr_DestroyTextureSwapChain(m_ovrSession, m_ovrTextureChain);
		m_ovrTextureChain = nullptr;
	}
	*/

	// TODO: Release the swap chain and depth buffers

	ovr_Shutdown();

Error:
	return r;
}
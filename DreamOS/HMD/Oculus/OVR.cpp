#include "OVR.h"
#include <math.h>

// TODO: Fix this encapsulation
#include "HAL/opengl/OpenGLImp.h"

#include "Primitives/stereocamera.h"

OVR::OVR() :
	m_ovrSession(nullptr),
	m_ovrTextureChain(nullptr),
	m_ovrSwapChainLength(0)
{
	// empty stub
}

OVR::~OVR() {
	// empty stub
}

RESULT OVR::InitializeHMD(HALImp *halimp) {
	RESULT r = R_PASS;
	ovrGraphicsLuid luid;

	// Initializes LibOVR, and the Rift
	CRM((RESULT)ovr_Initialize(nullptr), "Failed to initialize libOVR.");

	// Attempt to create OVR session
	CRM((RESULT)ovr_Create(&m_ovrSession, &luid), "Failed to create OVR session");

	m_ovrHMDDescription = ovr_GetHmdDesc(m_ovrSession);

	m_eyeWidth = m_ovrHMDDescription.Resolution.w / 2;
	m_eyeHeight = m_ovrHMDDescription.Resolution.h;

	// Get tracker descriptions
	unsigned int trackerCount = std::max<unsigned int>(1, ovr_GetTrackerCount(m_ovrSession));
	for (unsigned int i = 0; i < trackerCount; ++i)
		m_TrackerDescriptions.push_back(ovr_GetTrackerDesc(m_ovrSession, i));

	// Configure Stereo settings.
	ovrSizei recommenedTex0Size = ovr_GetFovTextureSize(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[0], 1.0f);
	ovrSizei recommenedTex1Size = ovr_GetFovTextureSize(m_ovrSession, ovrEye_Right, m_ovrHMDDescription.DefaultEyeFov[1], 1.0f);
	
	ovrSizei bufferSize;
	bufferSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
	bufferSize.h = fmax(recommenedTex0Size.h, recommenedTex1Size.h);

	// Set up the OGL Swap Chain
	// TODO: This should be done in a cross platform way
	m_ovrTextureSwapChainDescription.Type = ovrTexture_2D;
	m_ovrTextureSwapChainDescription.ArraySize = 1;
	
	m_ovrTextureSwapChainDescription.Width = m_eyeWidth;
	m_ovrTextureSwapChainDescription.Height = m_eyeHeight;

	//m_ovrTextureSwapChainDescription.Width = bufferSize.w;
	//m_ovrTextureSwapChainDescription.Height = bufferSize.h;

	m_ovrTextureSwapChainDescription.MipLevels = 1;
	m_ovrTextureSwapChainDescription.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_ovrTextureSwapChainDescription.SampleCount = 1;
	m_ovrTextureSwapChainDescription.StaticImage = ovrFalse;

	// TODO: Need to set up OpenGL first
	// TODO: This is GL specific 
	CRM((RESULT)ovr_CreateTextureSwapChainGL(m_ovrSession, &m_ovrTextureSwapChainDescription, &m_ovrTextureChain), "Failed to create Texture Swap Chain for OGL");
	CRM((RESULT)ovr_GetTextureSwapChainLength(m_ovrSession, m_ovrTextureChain, &m_ovrSwapChainLength), "Failed to get OVR Swap Chain Length");

	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(halimp);

	for (int i = 0; i < 2; i++) {
		GLuint swapChainTextureIndex;
		ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureChain, i, &swapChainTextureIndex);

		// TODO: Set up the framebuffers here
		m_pStereoFramebuffers[i] = new OGLFramebuffer(oglimp, GetEyeWidth(), GetEyeHeight(), 3);

	}


Error:
	return r;
}

RESULT OVR::BindFramebuffer(EYE_TYPE eye) {
	return m_pStereoFramebuffers[eye]->BindOGLFramebuffer();
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

	if (m_ovrTextureChain != nullptr) {
		ovr_DestroyTextureSwapChain(m_ovrSession, m_ovrTextureChain);
		m_ovrTextureChain = nullptr;
	}

	ovr_Shutdown();

Error:
	return r;
}
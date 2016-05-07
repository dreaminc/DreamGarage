#include "OVR.h"
#include <math.h>

// TODO: Fix this encapsulation
#include "HAL/opengl/OpenGLImp.h"

#include "Primitives/stereocamera.h"
#include "Primitives/rectangle.h"

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

	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(halimp);

	for (int i = 0; i < 2; i++) {
		GLuint swapChainTextureIndex;
		ovr_GetTextureSwapChainBufferGL(m_ovrSession, m_ovrTextureChain, i, &swapChainTextureIndex);

		// TODO: Set up the framebuffers here
		m_pStereoFramebuffers[i] = new OGLFramebuffer(oglimp, GetEyeWidth(), GetEyeHeight(), 3);
	}

	// Implement a single layer
	// Initialize our single full screen Fov layer.
	m_ovrEyeRenderDescription[0] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[0]);
	m_ovrEyeRenderDescription[1] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Right, m_ovrHMDDescription.DefaultEyeFov[1]);

	m_ovrLayer.Header.Type = ovrLayerType_EyeFov;
	m_ovrLayer.Header.Flags = 0;
	m_ovrLayer.ColorTexture[0] = m_ovrTextureChain;
	m_ovrLayer.ColorTexture[1] = m_ovrTextureChain;
	m_ovrLayer.Fov[0] = m_ovrEyeRenderDescription[0].Fov;
	m_ovrLayer.Fov[1] = m_ovrEyeRenderDescription[1].Fov;
	m_ovrLayer.Viewport[0] = *(reinterpret_cast<ovrRecti*>(&(rectangle<int>(0, 0, GetEyeWidth(), GetEyeHeight()))));
	m_ovrLayer.Viewport[1] = *(reinterpret_cast<ovrRecti*>(&(rectangle<int>(GetEyeWidth(), 0, GetEyeWidth(), GetEyeHeight()))));

Error:
	return r;
}

RESULT OVR::BindFramebuffer(EYE_TYPE eye) {
	return m_pStereoFramebuffers[eye]->BindOGLFramebuffer();
}

// Commit the changes to the texture swap chain
RESULT OVR::CommitSwapChain() {	
	RESULT r = R_PASS;

	CR((RESULT)ovr_CommitTextureSwapChain(m_ovrSession, m_ovrTextureChain));

Error:
	return r;
}

RESULT OVR::SubmitFrame() {
	RESULT r = R_PASS;

	ovrLayerHeader* layers = &m_ovrLayer.Header;
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

	if (m_ovrTextureChain != nullptr) {
		ovr_DestroyTextureSwapChain(m_ovrSession, m_ovrTextureChain);
		m_ovrTextureChain = nullptr;
	}

	ovr_Shutdown();

Error:
	return r;
}
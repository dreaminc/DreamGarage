#include "OVR.h"
#include <math.h>

// TODO: Fix this encapsulation
#include "HAL/opengl/OpenGLImp.h"

#include "Primitives/stereocamera.h"
#include "Primitives/rectangle.h"

OVR::OVR() :
	m_ovrSession(nullptr),
	m_ovrMirrorTexture(nullptr)
{
	// empty stub
}

OVR::~OVR() {
	// empty stub
}

RESULT OVR::InitializeHMD(HALImp *halimp, int wndWidth, int wndHeight) {
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

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);

	// FloorLevel will give tracking poses where the floor height is 0
	CR((RESULT)ovr_SetTrackingOriginType(m_ovrSession, ovrTrackingOrigin_FloorLevel));

	m_eyeWidth = m_ovrHMDDescription.Resolution.w / 2;
	m_eyeHeight = m_ovrHMDDescription.Resolution.h;

	for (int i = 0; i < HMD_NUM_EYES; i++) {
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_ovrSession, ovrEyeType(i), m_ovrHMDDescription.DefaultEyeFov[i], 1);
		
		m_ovrTextureSwapChains[i] = new OVRTextureSwapChain(oglimp, m_ovrSession, idealTextureSize.w, idealTextureSize.h, 1, NULL, 1);
		CR(m_ovrTextureSwapChains[i]->OVRInitialize());
	}

	// Frontload Layer Initialization
	m_ovrLayer.Header.Type = ovrLayerType_EyeFov;
	m_ovrLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

	// Set up the mirror texture
	if (wndWidth == 0)
		wndWidth = m_ovrHMDDescription.Resolution.w / 2;

	if (wndHeight == 0)
		wndHeight = m_ovrHMDDescription.Resolution.h / 2;

	m_ovrMirrorTexture = new OVRMirrorTexture(oglimp, m_ovrSession, wndWidth, wndHeight);
	
	CN(m_ovrMirrorTexture);
	CR(m_ovrMirrorTexture->OVRInitialize());
	
Error:
	return r;
}

RESULT OVR::SetUpFrame() {
	RESULT r = R_PASS;

	// TODO: Set up frame

Error:
	return r;
}

RESULT OVR::RenderHMDMirror() {
	return m_ovrMirrorTexture->RenderMirrorToBackBuffer();
}

RESULT OVR::BindFramebuffer(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

// Commit the changes to the texture swap chain
RESULT OVR::CommitSwapChain(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->Commit();
}

RESULT OVR::SetAndClearRenderSurface(EYE_TYPE eye) {
	m_ovrEyeRenderDescription[eye] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[eye]);

	return m_ovrTextureSwapChains[eye]->SetAndClearRenderSurface();
}

RESULT OVR::UnsetRenderSurface(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->UnsetRenderSurface();
}

RESULT OVR::SubmitFrame() {
	RESULT r = R_PASS;

	// TODO: Split this across the eyes 
	long long frameIndex = 0;
	ovrPosef EyeRenderPose[2];
	ovrVector3f HmdToEyeOffset[2] = { m_ovrEyeRenderDescription[0].HmdToEyeOffset, m_ovrEyeRenderDescription[1].HmdToEyeOffset };

	double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	ovr_GetEyePoses(m_ovrSession, frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	for (int eye = 0; eye < 2; ++eye) {
		m_ovrLayer.ColorTexture[eye] = m_ovrTextureSwapChains[eye]->GetOVRTextureSwapChain();
		m_ovrLayer.Viewport[eye] = m_ovrTextureSwapChains[eye]->GetOVRViewportRecti();
		m_ovrLayer.Fov[eye] = m_ovrHMDDescription.DefaultEyeFov[eye];
		m_ovrLayer.RenderPose[eye] = EyeRenderPose[eye];
		m_ovrLayer.SensorSampleTime = sensorSampleTime;
	}

	ovrLayerHeader* layers = &m_ovrLayer.Header;
	CR((RESULT)ovr_SubmitFrame(m_ovrSession, 0, nullptr, &layers, 1));

	/* TODO: Might want to check on session
	ovrSessionStatus sessionStatus;
		ovr_GetSessionStatus(session, &sessionStatus);
		if (sessionStatus.ShouldQuit)
			goto Done;
		if (sessionStatus.ShouldRecenter)
			ovr_RecenterTrackingOrigin(session);
	*/

Error:
	return r;
}

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMING

RESULT OVR::UpdateHMD() {
	RESULT r = R_PASS;

#ifdef HMD_OVR_USE_PREDICTED_TIMING
	double fTiming = ovr_GetPredictedDisplayTime(m_ovrSession, 0);
#else
	double fTiming = ovr_GetTimeInSeconds();
#endif
	ovrTrackingState trackingState = ovr_GetTrackingState(m_ovrSession, fTiming, true);

	if (trackingState.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
		//ovrPosef headPose = trackingState.HeadPose.ThePose;
		m_ptOrigin = point(reinterpret_cast<float*>(&(trackingState.HeadPose.ThePose.Position)));
		m_qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(trackingState.HeadPose.ThePose.Orientation)));
		m_qOrientation.Reverse();
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

	for (int i = 0; i < HMD_NUM_EYES; i++) {
		if (m_ovrTextureSwapChains[i] != nullptr) {
			delete m_ovrTextureSwapChains[i];
			m_ovrTextureSwapChains[i] = nullptr;
		}

		/*
		if (m_depthbuffers[i] != nullptr) {
			delete m_depthbuffers[i];
			m_depthbuffers[i] = nullptr;
		}
		*/
	}

	ovr_Shutdown();

Error:
	return r;
}
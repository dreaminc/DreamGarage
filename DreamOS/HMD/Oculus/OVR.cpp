#include "OVR.h"
#include <math.h>

#include "Sandbox/SandboxApp.h"

// TODO: Fix this encapsulation
#include "HAL/opengl/OpenGLImp.h"

#include "Primitives/stereocamera.h"
#include "Primitives/rectangle.h"

#include "Extras/OVR_Math.h"

#include "DreamConsole/DreamConsole.h"

OVRHMD::OVRHMD(SandboxApp *pParentSandbox) :
	HMD(pParentSandbox),
	m_ovrSession(nullptr),
	m_ovrMirrorTexture(nullptr)
{
	// empty stub
}

OVRHMD::~OVRHMD() {
	// empty stub
}

RESULT OVRHMD::InitializeHMD(HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;
	ovrGraphicsLuid luid;
	m_pHALImp = halimp;
	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(halimp);

	// Initializes LibOVR, and the Rift
	// TODO: may be important to make an OVR Logger.  
	// use it as an arg to ovr_Initialize
	CRM((RESULT)ovr_Initialize(nullptr), "Failed to initialize libOVR.");

	// Attempt to create OVR session
	CRM((RESULT)ovr_Create(&m_ovrSession, &luid), "Failed to create OVR session");

	m_ovrHMDDescription = ovr_GetHmdDesc(m_ovrSession);

	// Get tracker descriptions
	unsigned int trackerCount = std::max<unsigned int>(1, ovr_GetTrackerCount(m_ovrSession));
	for (unsigned int i = 0; i < trackerCount; ++i)
		m_TrackerDescriptions.push_back(ovr_GetTrackerDesc(m_ovrSession, i));

	// FloorLevel will give tracking poses where the floor height is 0
	CR((RESULT)ovr_SetTrackingOriginType(m_ovrSession, ovrTrackingOrigin_FloorLevel));

	for (int i = 0; i < HMD_NUM_EYES; i++) {
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_ovrSession, ovrEyeType(i), m_ovrHMDDescription.DefaultEyeFov[i], 1);
		
		m_eyeWidth = idealTextureSize.w;
		m_eyeHeight = idealTextureSize.h;

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
	
	// Turn off vsync to let the compositor do its magic
	oglimp->wglSwapIntervalEXT(0);

	m_pSenseController = new SenseController();

	m_pLeftRotation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)(M_PI / 2.0f));
	m_pRightRotation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)(-M_PI / 2.0f));

	OVERLAY_DEBUG_OUT("HMD Oculus Rift - On");

Error:
	return r;
}

ProjectionMatrix OVRHMD::GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) {
	ovrEyeType eyeType = (eye == EYE_LEFT) ? ovrEye_Left : ovrEye_Right;
	
	ovrMatrix4f OVRProjMatrix = ovrMatrix4f_Projection(m_ovrHMDDescription.DefaultEyeFov[eyeType], znear, zfar, ovrProjection_None);

	ProjectionMatrix projMat;
	//memcpy(&projMat, &OVRProjMatrix, sizeof(ovrMatrix4f));
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			projMat(i, j) = OVRProjMatrix.M[i][j];

	return projMat;
}

ViewMatrix OVRHMD::GetViewMatrix(EYE_TYPE eye) {
	ovrEyeType eyeType = (eye == EYE_LEFT) ? ovrEye_Left : ovrEye_Right;

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Left, m_ovrHMDDescription.DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Right, m_ovrHMDDescription.DefaultEyeFov[1]);

	// Get eye poses, feeding in correct IPD offset
	ovrPosef EyeRenderPose[2];
	ovrVector3f HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset, eyeRenderDesc[1].HmdToEyeOffset };

	double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	ovr_GetEyePoses(m_ovrSession, 0, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	point ptPosition = point(reinterpret_cast<float*>(&(EyeRenderPose[eyeType].Position)));
	quaternion qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(EyeRenderPose[eyeType].Orientation)));
	qOrientation.Reverse();
	ptPosition *= -1.0f;

	//return ViewMatrix(ptPosition, qOrientation);

	/*
	// Get view and projection matrices
	OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(0);
	OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(EyeRenderPose[eyeType].Orientation);
	OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
	OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
	OVR::Vector3f shiftedEyePos = rollPitchYaw.Transform(EyeRenderPose[eyeType].Position);

	OVR::Matrix4f view = OVR::Matrix4f::LookAtLH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);

	ViewMatrix vewMat;
	memcpy(&vewMat, &view, sizeof(ovrMatrix4f));
	return vewMat;
	*/

	
	//return ViewMatrix(point(), qOrientation);
	
	///*
	static float yaw = 0.15f;
	//yaw += 0.001f;

	if (eye != EYE_LEFT)
		return ViewMatrix(ptPosition, 0.0f, -yaw, 0.0f);
	else
		return ViewMatrix(ptPosition, 0.0f, yaw, 0.0f);
	//*/
}

RESULT OVRHMD::SetUpFrame() {
	RESULT r = R_PASS;

	// TODO: Set up frame

//Error:
	return r;
}

RESULT OVRHMD::RenderHMDMirror() {
	return m_ovrMirrorTexture->RenderMirrorToBackBuffer();
}

RESULT OVRHMD::BindFramebuffer(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

// Commit the changes to the texture swap chain
RESULT OVRHMD::CommitSwapChain(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->Commit();
}

RESULT OVRHMD::SetAndClearRenderSurface(EYE_TYPE eye) {
	ovrEyeType eyeType = (eye == EYE_LEFT) ? ovrEye_Left : ovrEye_Right;

	m_ovrEyeRenderDescription[eyeType] = ovr_GetRenderDesc(m_ovrSession, eyeType, m_ovrHMDDescription.DefaultEyeFov[eyeType]);

	return m_ovrTextureSwapChains[eye]->SetAndClearRenderSurface();
}

RESULT OVRHMD::UnsetRenderSurface(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->UnsetRenderSurface();
}

RESULT OVRHMD::SubmitFrame() {
	RESULT r = R_PASS;

	// TODO: Split this across the eyes 
	long long frameIndex = 0;
	ovrPosef EyeRenderPose[2];
	ovrVector3f HmdToEyeOffset[2] = { m_ovrEyeRenderDescription[0].HmdToEyeOffset, m_ovrEyeRenderDescription[1].HmdToEyeOffset };

	double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	ovr_GetEyePoses(m_ovrSession, frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	for (int eye = 0; eye < 2; eye++) {
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

RESULT OVRHMD::UpdateHMD() {
	RESULT r = R_PASS;

	//ovr_RecenterTrackingOrigin(m_ovrSession);

#ifdef HMD_OVR_USE_PREDICTED_TIMING
	double fTiming = ovr_GetPredictedDisplayTime(m_ovrSession, 0);
#else
	double fTiming = ovr_GetTimeInSeconds();
#endif
	ovrTrackingState trackingState = ovr_GetTrackingState(m_ovrSession, fTiming, true);

	if (trackingState.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
		//ovrPosef headPose = trackingState.HeadPose.ThePose;
		m_ptOrigin = point(reinterpret_cast<float*>(&(trackingState.HeadPose.ThePose.Position)));

		RotationMatrix qOffset = RotationMatrix();
		quaternion qRotation = m_pHALImp->GetCamera()->GetOffsetOrientation();
		qRotation.Reverse();
		qOffset.SetQuaternionRotationMatrix(qRotation);
		m_ptOrigin = qOffset * m_ptOrigin;

		//m_ptOrigin *= -1.0f;	// TODO: This is an issue with the OVR position 

		m_qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(trackingState.HeadPose.ThePose.Orientation)));
		m_qOrientation.Reverse();
	}

	if (trackingState.HandStatusFlags) {

		ovrInputState inputState;
		ovr_GetInputState(m_ovrSession, ovrControllerType::ovrControllerType_Touch, &inputState);

		point offset = m_pParentSandbox->GetCamera()->camera::GetPosition();

		RotationMatrix qOffset = RotationMatrix();
		quaternion qRotation = m_pParentSandbox->GetCamera()->GetOffsetOrientation();
		qRotation.Reverse();
		qOffset.SetQuaternionRotationMatrix(qRotation);

		int i = 0;
		for (auto& hand : { m_pLeftHand, m_pRightHand }) {
			if (trackingState.HandStatusFlags[i] != 3) continue;

			point ptControllerPosition = point(reinterpret_cast<float*>(&(trackingState.HandPoses[i].ThePose.Position)));
			ptControllerPosition = qOffset * ptControllerPosition;
			hand->SetPosition(ptControllerPosition + offset);

			quaternion qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(trackingState.HandPoses[i].ThePose.Orientation)));
			// Act like this doesn't exist
			qOrientation.Reverse();
			qRotation.Reverse();
			qOrientation *= qRotation;
			qOrientation.Reverse();

			quaternion base = i == 0 ? m_pLeftRotation : m_pRightRotation;
			hand->SetOrientation(qOrientation * base);
			hand->SetLocalOrientation(qOrientation);
			
			hand::HAND_TYPE hType = i == 0 ? hand::HAND_TYPE::HAND_LEFT : hand::HAND_TYPE::HAND_RIGHT;
			hand->SetHandModel(hType);
			hand->SetTracked(true);

			ovrControllerType type = i == 0 ? ovrControllerType_LTouch : ovrControllerType_RTouch; 
			UpdateSenseController(type, inputState);

			i += 1;
		}
	}


//Error:
	return r;
}

RESULT OVRHMD::UpdateSenseController(ovrControllerType type, ovrInputState& inputState) {
	RESULT r = R_PASS;

	ControllerState cState;
	CN(&inputState);

	//ovrControllerType buttons -
	//0x0001 - A
	//0x0002 - B
	//0x0004 - Right stick press
	//0x0100 - X
	//0x0200 - Y
	//0x0400 - Left stick press
	//0x100000 - Left menu

	cState.fMenu = (inputState.Buttons & 1<<20) != 0;
	cState.fMenu = ((inputState.Buttons & 1<<1) != 0) || cState.fMenu;

	// TODO: should probably change this value in controllerState to 'fSelected'
	cState.triggerRange = ((inputState.Buttons & 1) != 0) ? 1.0f : 0.0f;
	//cState.triggerRange = (inputState.IndexTrigger[cState.type]);

	if (type == ovrControllerType::ovrControllerType_LTouch) {
		cState.type = CONTROLLER_LEFT;
	}
	else if (type == ovrControllerType::ovrControllerType_RTouch) {
		cState.type = CONTROLLER_RIGHT;
	}
	else {
		return r;
	}

	cState.fGrip = (inputState.HandTrigger[cState.type] > 0.9f);

	cState.ptTouchpad = point(inputState.Thumbstick[cState.type].x, inputState.Thumbstick[cState.type].y, 0.0f);
		
	m_pSenseController->SetControllerState(cState);

Error:
	return r;
}

RESULT OVRHMD::ReleaseHMD() {
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

//Error:
	return r;
}
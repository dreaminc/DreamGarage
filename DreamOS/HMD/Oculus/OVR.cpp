#include "OVR.h"
#include <math.h>

#include "Sandbox/SandboxApp.h"

// TODO: Fix this encapsulation
#include "HAL/opengl/OpenGLImp.h"

#include "Primitives/stereocamera.h"
#include "Primitives/rectangle.h"

#include "Extras/OVR_Math.h"

#include "DreamLogger/DreamLogger.h"

#include "OVRHMDSinkNode.h"

#include "OVRTouchController.h"

#include "OVRPlatform.h"

#include "OVR_CAPI_Audio.h"
#include "core/Utilities.h"

OVRHMD::OVRHMD(SandboxApp *pParentSandbox) :
	HMD(pParentSandbox),
	m_ovrSession(nullptr),
	m_pOVRMirrorTexture(nullptr)
{
	// empty stub
}

OVRHMD::~OVRHMD() {
	// empty stub
}

RESULT OVRHMD::InitializeHMDSourceNode() {
	return R_NOT_IMPLEMENTED;
}

RESULT OVRHMD::InitializeHMDSinkNode() {
	RESULT r = R_PASS;

	OpenGLImp *pOGLImp = dynamic_cast<OpenGLImp*>(m_pHALImp);
	CN(pOGLImp);

	m_pOVRHMDSinkNode = new OVRHMDSinkNode(pOGLImp, this);
	CN(m_pOVRHMDSinkNode);

	CR(m_pOVRHMDSinkNode->OGLInitialize());
	CR(m_pOVRHMDSinkNode->SetupConnections());

Error:
	return r;
}

HMDSinkNode* OVRHMD::GetHMDSinkNode() {
	return (HMDSinkNode*)(m_pOVRHMDSinkNode);
}

HMDSourceNode* OVRHMD::GetHMDSourceNode() {
	// TODO:
	return nullptr;
}

RESULT OVRHMD::InitializeHMD(HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;

	ovrGraphicsLuid luid;
	m_pHALImp = halimp;
	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(halimp);

	// Initializes Oculus Platform
	m_pOVRPlatform = new OVRPlatform();
	CN(m_pOVRPlatform);
	CRM(m_pOVRPlatform->InitializePlatform(), "Failed to initialize Oculus Platform");
	
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
	CR((RESULT)ovr_SetTrackingOriginType(m_ovrSession, ovrTrackingOrigin_EyeLevel));

	// Set up the mirror texture
	if (wndWidth == 0)
		wndWidth = m_ovrHMDDescription.Resolution.w / 2;

	if (wndHeight == 0)
		wndHeight = m_ovrHMDDescription.Resolution.h / 2;

	m_pOVRMirrorTexture = new OVRMirrorTexture(oglimp, m_ovrSession, wndWidth, wndHeight);
	
	CN(m_pOVRMirrorTexture);
	CR(m_pOVRMirrorTexture->OVRInitialize());
	
	// Turn off vsync to let the compositor do its magic
	oglimp->wglSwapIntervalEXT(0);

	//OVERLAY_DEBUG_OUT("HMD Oculus Rift - On");
	DOSLOG(INFO, "HMD Oculus Rift Initialized ");

	// Controller

	m_pSenseController = new OVRTouchController(m_ovrSession);
	CN(m_pSenseController);
	CR(m_pSenseController->Initialize());

	qLeftRotation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)(M_PI / 2.0f));
	qRightRotation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)(-M_PI / 2.0f));

#ifdef _USE_TEST_APP
	// In testing we just use spheres to speed up on testing
	// TODO: Add this to config instead

	m_pLeftControllerModel = m_pParentSandbox->AddComposite();
	CN(m_pLeftControllerModel);
	m_pLeftControllerModel->AddSphere(0.05f, 10, 10);

	m_pRightControllerModel = m_pParentSandbox->AddComposite();
	CN(m_pRightControllerModel);
	m_pRightControllerModel->AddSphere(0.05f, 10, 10);
#else 
	// Oculus controller dimensions: 4.1 x 4.5 x 3.8 in.
	// Initial point displacement taken from SteamVR JSON file, 
	// ptAdjust created from manual testing
	{
		point ptAdjust = point(0.0f, 0.01f, -0.0057f);
		m_pLeftControllerModel = m_pParentSandbox->MakeModel(L"\\OculusTouch\\LeftController\\oculus_cv1_controller_left.obj");
		auto pMesh = m_pLeftControllerModel->GetFirstChild<mesh>();
		pMesh->SetPosition(point(-0.00629f, 0.02522f, -0.03469f) + ptAdjust);
		pMesh->SetOrientation(quaternion::MakeQuaternionWithEuler(39.4f * (float)(M_PI) / 180.0f, 0.0f, 0.0f));
		//pMesh->SetOrientationOffsetDeg(39.4f, 0.0f, 0.0f);
		//m_pParentSandbox->AddObject(pMesh.get());
		//pMesh->SetVisible(false);

		m_pRightControllerModel = m_pParentSandbox->MakeModel(L"\\OculusTouch\\RightController\\oculus_cv1_controller_right.obj");
		pMesh = m_pRightControllerModel->GetFirstChild<mesh>();
		pMesh->SetPosition(point(0.00629f, 0.02522f, -0.03469f) + ptAdjust);
		pMesh->SetOrientation(quaternion::MakeQuaternionWithEuler(39.4f * (float)(M_PI) / 180.0f, 0.0f, 0.0f));
		//pMesh->SetOrientationOffsetDeg(39.4f, 0.0f, 0.0f);
		//m_pParentSandbox->AddObject(pMesh.get());
		//pMesh->SetVisible(false);
	}

#endif

	if(m_pLeftControllerModel != nullptr)
		m_pLeftControllerModel->SetVisible(false);

	if(m_pRightControllerModel)
		m_pRightControllerModel->SetVisible(false);

Error:
	return r;
}



composite *OVRHMD::GetSenseControllerObject(ControllerType controllerType) {
	switch (controllerType) {
		case CONTROLLER_LEFT: {
			return m_pLeftControllerModel;
		} break;

		case CONTROLLER_RIGHT: {
			return m_pRightControllerModel;
		} break;
	}

	return nullptr;
}

HMDDeviceType OVRHMD::GetDeviceType() {
	return HMDDeviceType::OCULUS;
}

std::string OVRHMD::GetDeviceTypeString() {
	return "HMDType.OculusRift";
}

RESULT OVRHMD::RecenterHMD() {
	RESULT r = R_PASS;

	m_fShouldRecenterHMD = true;

	return r;
}

bool OVRHMD::ShouldRecenterHMD(ovrSessionStatus sessionStatus) {
	return (m_fShouldRecenterHMD || sessionStatus.ShouldRecenter) && sessionStatus.HmdMounted;
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

// TODO: This might be dead code
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
	
	return ViewMatrix(ptPosition, 0.0f, 0.0f, 0.0f);

	/*
	//static float yaw = 0.15f;
	//static float yaw = 0.0f;
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
	return m_pOVRMirrorTexture->RenderMirrorToBackBuffer();
}

RESULT OVRHMD::BindFramebuffer(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

// Commit the changes to the texture swap chain
RESULT OVRHMD::CommitSwapChain(EYE_TYPE eye) {
	return m_pOVRHMDSinkNode->CommitSwapChain(eye);
}

RESULT OVRHMD::SetAndClearRenderSurface(EYE_TYPE eye) {
	return m_pOVRHMDSinkNode->SetAndClearRenderSurface(eye);
}

RESULT OVRHMD::UnsetRenderSurface(EYE_TYPE eye) {
	return m_pOVRHMDSinkNode->UnsetRenderSurface(eye);
}

RESULT OVRHMD::SubmitFrame() {
	return m_pOVRHMDSinkNode->SubmitFrame();
}

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMING

RESULT OVRHMD::UpdateHMD() {
	RESULT r = R_PASS;

	ovrSessionStatus OVRSessionStatus;
	ovr_GetSessionStatus(m_ovrSession, &OVRSessionStatus);

	if (OVRSessionStatus.ShouldQuit) {
		DOSLOG(INFO, "ShouldQuit received from Oculus, shutting down sandbox")
		m_pParentSandbox->PendShutdown();
	}

	if (OVRSessionStatus.ShouldRecenter) {
		DOSLOG(INFO, "ShouldRecenter");
		CRM((RESULT)ovr_RecenterTrackingOrigin(m_ovrSession), "Failed to recenter OVRHMD");
	}

	if (ShouldRecenterHMD(OVRSessionStatus)) {
		CRM((RESULT)ovr_RecenterTrackingOrigin(m_ovrSession), "Failed to recenter OVRHMD");
		m_fShouldRecenterHMD = false;
	}

	CRM(m_pOVRPlatform->Update(), "Oculus Platform passed an error");

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

		UpdateSenseController(ovrControllerType_LTouch, inputState);
		UpdateSenseController(ovrControllerType_RTouch, inputState);

		point offset = m_pParentSandbox->GetCamera()->camera::GetPosition();

		RotationMatrix qOffset = RotationMatrix();
		quaternion qRotation = m_pParentSandbox->GetCamera()->GetOffsetOrientation();
		qRotation.Reverse();
		qOffset.SetQuaternionRotationMatrix(qRotation);
		qRotation.Reverse();

		for (int i = 0; i < 2; i++) {

			auto& hand = i == 0 ? m_pLeftHand : m_pRightHand;
			auto& pModel = i == 0 ? m_pLeftControllerModel : m_pRightControllerModel;

			if (trackingState.HandStatusFlags[i] != 3) {
				hand->SetTracked(false);
				//hand->SetVisible(false);
				//pModel->SetVisible(false);
				continue;
			}

			point ptControllerPosition = point(reinterpret_cast<float*>(&(trackingState.HandPoses[i].ThePose.Position)));
			ptControllerPosition = qOffset * ptControllerPosition;
			hand->SetPosition(ptControllerPosition + offset);
			pModel->SetPosition(ptControllerPosition + offset);

			quaternion qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(trackingState.HandPoses[i].ThePose.Orientation)));
			// Act like this doesn't exist
			qOrientation.Reverse();
			qOrientation *= qRotation;
			qOrientation.Reverse();

			quaternion base = i == 0 ? qLeftRotation : qRightRotation;
			hand->SetOrientation(qOrientation);
			//hand->SetOrientation(qOrientation * base);
			hand->SetLocalOrientation(qOrientation);
			pModel->SetOrientation(qOrientation);
			
			HAND_TYPE hType = i == 0 ? HAND_TYPE::HAND_LEFT : HAND_TYPE::HAND_RIGHT;
			hand->SetTracked(true);
			//pModel->SetVisible(true);
		}
	}


Error:
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

	// B and Y spawn the menu event
	cState.fMenu = ((inputState.Buttons & 1<<1) != 0);
	cState.fMenu = ((inputState.Buttons & 1<<9) != 0) || cState.fMenu;
	cState.fMenu = (inputState.Buttons & 1<<20) != 0 || cState.fMenu;

	// TODO: should probably change this value in controllerState to 'fSelected'
	//cState.triggerRange = ((inputState.Buttons & 1) != 0) ? 1.0f : 0.0f;
	
	
	switch(type) {
		case ovrControllerType::ovrControllerType_LTouch: {
			cState.type = CONTROLLER_LEFT;
		} break;
	
		case ovrControllerType::ovrControllerType_RTouch: {
			cState.type = CONTROLLER_RIGHT;
		} break;
			
		default: {
			return r;
		} break;
	}

	cState.triggerRange = inputState.IndexTrigger[cState.type];

	cState.fGrip = (inputState.HandTrigger[cState.type] > 0.9f);

	cState.ptTouchpad = point(inputState.Thumbstick[cState.type].x, inputState.Thumbstick[cState.type].y, 0.0f);
		
	m_pSenseController->SetControllerState(cState);

Error:
	return r;
}

RESULT OVRHMD::GetAudioDeviceOutID(std::wstring &wstrAudioDeviceOutGUID) {
	RESULT r = R_PASS;

	WCHAR wszDeviceOutStrBuffer[OVR_AUDIO_MAX_DEVICE_STR_SIZE];
	CRM((RESULT)ovr_GetAudioDeviceOutGuidStr(wszDeviceOutStrBuffer), "Failed to retrieve OVR audio device out GUID");
	wstrAudioDeviceOutGUID = std::wstring(wszDeviceOutStrBuffer);

	//GUID deviceOutGuid;
	//CRM((RESULT)ovr_GetAudioDeviceOutGuid(&deviceOutGuid), "Failed to retrieve OVR audio device out GUID");
	//wstrAudioDeviceOutGUID = util::GuidToWideString(deviceOutGuid);
	
Error:
	return r;
}

RESULT OVRHMD::GetAudioDeviceInGUID(std::wstring &wstrAudioDeviceInGUID) {
	RESULT r = R_PASS;

	//WCHAR wszDeviceInStrBuffer[OVR_AUDIO_MAX_DEVICE_STR_SIZE];
	//CRM((RESULT)ovr_GetAudioDeviceInGuidStr(wszDeviceInStrBuffer), "Failed to retrieve OVR audio device out GUID");
	//wstrAudioDeviceInGUID = std::wstring(wszDeviceInStrBuffer);

	// TODO:

Error:
	return r;
}

RESULT OVRHMD::ShutdownParentSandbox() {
	RESULT r = R_PASS;
	CR(m_pParentSandbox->Shutdown());

Error:
	return r;
}

RESULT OVRHMD::ReleaseHMD() {
	RESULT r = R_PASS;

	if (m_pOVRMirrorTexture != nullptr) {
		delete m_pOVRMirrorTexture;
		m_pOVRMirrorTexture = nullptr;
	}

	if (m_pOVRHMDSinkNode != nullptr) {
		delete m_pOVRHMDSinkNode;
		m_pOVRHMDSinkNode = nullptr;
	}

	if (m_ovrSession != nullptr) {
		ovr_Destroy(m_ovrSession);
		m_ovrSession = nullptr;
	}

	ovr_Shutdown();

//Error:
	return r;
}
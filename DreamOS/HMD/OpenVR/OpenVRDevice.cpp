#include "OpenVRDevice.h"
#include "HAL/opengl/OGLTexture.h"

OpenVRDevice::OpenVRDevice() :
	m_pIVRHMD(nullptr),
	m_pRenderModels(nullptr),
	m_pCompositor(nullptr),
	m_pFramebufferRenderLeft(nullptr),
	m_pFramebufferResolveLeft(nullptr),
	m_pFramebufferRenderRight(nullptr),
	m_pFramebufferResolveRight(nullptr),
	m_pControllerModelLeft(nullptr),
	m_pControllerModelRight(nullptr),
	m_pHMDModel(nullptr)
{
	// TODO
}
OpenVRDevice::~OpenVRDevice() {
	if (m_pIVRHMD != nullptr) {
		delete m_pIVRHMD;
		m_pIVRHMD = nullptr;
	}

	vr::VR_Shutdown();
}

std::string OpenVRDevice::GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError) {
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
RESULT OpenVRDevice::InitializeFrameBuffer(EYE_TYPE eye, uint32_t nWidth, uint32_t nHeight) {
	RESULT r = R_PASS;
	
	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(m_pHALImp);

	OGLFramebuffer *pOGLRenderFramebuffer = nullptr;
	OGLFramebuffer *pOGLResolveFramebuffer = nullptr;

	if (eye == EYE_LEFT) {
		m_pFramebufferRenderLeft = new OGLFramebuffer(oglimp, m_eyeWidth, m_eyeHeight, DEFAULT_OPENVR_RENDER_CHANNELS);
		m_pFramebufferResolveLeft = new OGLFramebuffer(oglimp, m_eyeWidth, m_eyeHeight, DEFAULT_OPENVR_RESOLVE_CHANNELS);
		pOGLRenderFramebuffer = m_pFramebufferRenderLeft;
		pOGLResolveFramebuffer = m_pFramebufferResolveLeft;
	}
	else if (eye == EYE_RIGHT) {
		m_pFramebufferRenderRight = new OGLFramebuffer(oglimp, m_eyeWidth, m_eyeHeight, DEFAULT_OPENVR_RENDER_CHANNELS);
		m_pFramebufferResolveRight = new OGLFramebuffer(oglimp, m_eyeWidth, m_eyeHeight, DEFAULT_OPENVR_RESOLVE_CHANNELS);
		pOGLRenderFramebuffer = m_pFramebufferRenderRight;
		pOGLResolveFramebuffer = m_pFramebufferResolveRight;
	}
	else {
		CBM((0), "Invalid Eye Passed");
	}

	CR(pOGLRenderFramebuffer->OGLInitialize());
	CR(pOGLRenderFramebuffer->BindOGLFramebuffer());

	CR(pOGLRenderFramebuffer->MakeOGLDepthbuffer());		// Note: This will create a new depth buffer
	CR(pOGLRenderFramebuffer->InitializeRenderBufferMultisample(GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT, DEFAULT_OPENVR_MULTISAMPLE));

	CR(pOGLRenderFramebuffer->MakeOGLTextureMultisample());
	CR(pOGLRenderFramebuffer->SetOGLTextureToFramebuffer2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE));

	CR(pOGLResolveFramebuffer->OGLInitialize());
	CR(pOGLResolveFramebuffer->BindOGLFramebuffer());

	CR(pOGLResolveFramebuffer->MakeOGLTexture());
	OGLTexture* pOGLTexture = pOGLResolveFramebuffer->GetOGLTexture();
	pOGLTexture->SetGLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	pOGLTexture->SetGLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	CR(pOGLResolveFramebuffer->SetOGLTextureToFramebuffer2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D));

	// Check FBO status and unbind
	CR(oglimp->CheckFramebufferStatus(GL_FRAMEBUFFER));
	CR(oglimp->glBindFramebuffer(GL_FRAMEBUFFER, 0));

Error:
	return r;
}

RESULT OpenVRDevice::InitializeHMD(HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;
	vr::EVRInitError ivrResult = vr::VRInitError_None;
	m_pHALImp = halimp;

	m_pIVRHMD = vr::VR_Init(&ivrResult, vr::VRApplication_Scene);
	CNM(m_pIVRHMD, "Failed to initialize and allocate IVR HMD");
	CIVRM(ivrResult, "Unable to initialize IVR runtime");

	m_strDriver = "No Driver";
	m_strDisplay = "No Display";

	m_strDriver = GetTrackedDeviceString(m_pIVRHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	m_strDisplay = GetTrackedDeviceString(m_pIVRHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

	CRM(SetupStereoRenderTargets(), "Failed to get render models");

	m_pCompositor = vr::VRCompositor();
	CNM(m_pCompositor, "Failed to initialize IVR compositor");

Error:
	return r;
}

void IVRThreadSleep(unsigned long nMilliseconds) {
#if defined(_WIN32)
	::Sleep(nMilliseconds);
#elif defined(POSIX)
	usleep(nMilliseconds * 1000);
#endif
}

// TODO: Might not want to have this here (move to sandbox or sense)
RESULT OpenVRDevice::InitializeRenderModels() {
	RESULT r = R_PASS;
	vr::EVRInitError ivrResult = vr::VRInitError_None;

	CN(m_pHALImp);

	m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &ivrResult);
	CIVRM(ivrResult, "Unable to get render model interface");

	for (uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++) {
		std::string sRenderModelName = GetTrackedDeviceString(m_pIVRHMD, unTrackedDevice, vr::Prop_RenderModelName_String);
		//CGLRenderModel *pRenderModel = FindOrLoadRenderModel(sRenderModelName.c_str());

		vr::RenderModel_t *pRenderModel = nullptr;
		vr::EVRRenderModelError error = vr::VRRenderModelError_None;

		while (1) {
			error = vr::VRRenderModels()->LoadRenderModel_Async(sRenderModelName.c_str(), &pRenderModel);
			if (error == vr::VRRenderModelError_Loading) {
				IVRThreadSleep(1);
			}
			else {
				break;
			}
		}

		CBM((error == vr::VRRenderModelError_None), "Failed to load %s model", sRenderModelName);


	}

Error:
	return r;
}

RESULT OpenVRDevice::SetupStereoRenderTargets() {
	RESULT r = R_PASS;

	CN(m_pIVRHMD);

	m_pIVRHMD->GetRecommendedRenderTargetSize(&m_eyeWidth, &m_eyeHeight);

	for (int i = 0; i < 2; i++) {
		CR(InitializeFrameBuffer(static_cast<EYE_TYPE>(i), m_eyeWidth, m_eyeHeight));
	}

Error:
	return r;
}

RESULT OpenVRDevice::HandleVREvent(vr::VREvent_t event) {
	RESULT r = R_PASS;

	switch (event.eventType) {
		case vr::VREvent_TrackedDeviceActivated: {
			//SetupRenderModelForTrackedDevice(event.trackedDeviceIndex);
			//dprintf("Device %u attached. Setting up render model.\n", event.trackedDeviceIndex);
			DEBUG_LINEOUT("Device %u attached", event.trackedDeviceIndex);
		} break;

		case vr::VREvent_TrackedDeviceDeactivated: {
			DEBUG_LINEOUT("Device %u detached.\n", event.trackedDeviceIndex);
		} break;

		case vr::VREvent_TrackedDeviceUpdated: {
			DEBUG_LINEOUT("Device %u updated.\n", event.trackedDeviceIndex);
		} break;

		// TODO: Lots more events to ultimately map...
	}

Error:
	return r;
}

Matrix4 OpenVRDevice::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose) {

	Matrix4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
		);

	return matrixObj;
}

ViewMatrix OpenVRDevice::ConvertSteamVRMatrixToViewMatrix(const vr::HmdMatrix34_t &matPose) {
	Matrix4 mat4 = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
	mat4.invert();

	ViewMatrix viewMat;
	viewMat.identity();

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			viewMat(j, i) = mat4[i * 4 + j];

	return viewMat;
}

RESULT OpenVRDevice::UpdateHMD() {
	RESULT r = R_PASS;

	// Process SteamVR events
	vr::VREvent_t event;
	while (m_pIVRHMD->PollNextEvent(&event, sizeof(event))) {
		HandleVREvent(event);
	}

	// Process SteamVR controller state
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++) {
		vr::VRControllerState_t state;

		if (m_pIVRHMD->GetControllerState(unDevice, &state)) {
			//m_rbShowTrackedDevice[unDevice] = state.ulButtonPressed == 0;
			// TODO: do stuff
		}
	}

	CN(m_pIVRHMD);

	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	m_validPoseCount = 0;
	m_strPoseClasses = "";

	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
		if (m_rTrackedDevicePose[nDevice].bPoseIsValid) {
			
			m_validPoseCount++;

				switch (m_pIVRHMD->GetTrackedDeviceClass(nDevice)) {
					case vr::TrackedDeviceClass_Controller: {
						Matrix4 poseMat4 = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
						Vector4 centerVec4 = poseMat4 * Vector4(0, 0, 0, 1);
						
						point ptControllerPosition = point(centerVec4.x, centerVec4.y, centerVec4.z);
						ptControllerPosition.Print();

					} break;

					case vr::TrackedDeviceClass_HMD: {
						ViewMatrix viewMat = ConvertSteamVRMatrixToViewMatrix(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
						m_ptOrigin = viewMat.GetPosition();

						m_qOrientation = viewMat.GetOrientation();
						m_qOrientation.Reverse();
					} break;

					case vr::TrackedDeviceClass_Invalid: {
						// TODO: Handle invalid
					} break;

					case vr::TrackedDeviceClass_Other: {
						// TODO: Handle other
					} break;

					case vr::TrackedDeviceClass_TrackingReference: {
						// TODO: Handle tracking reference
					} break;

					default: {
						// TODO: Default handling
					} break;
				}
			
		}
	}

Error:
	return r;
}

RESULT OpenVRDevice::ReleaseHMD() {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::SetUpFrame() {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::BindFramebuffer(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::CommitSwapChain(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::SubmitFrame() {
	RESULT r = R_PASS;
	vr::EVRCompositorError ivrResult = vr::VRCompositorError_None;

	// Left Eye
	vr::Texture_t leftEyeTexture;
	leftEyeTexture.handle = (void*)(m_pFramebufferResolveLeft->GetOGLTextureIndex());
	leftEyeTexture.eType = vr::API_OpenGL;
	leftEyeTexture.eColorSpace = vr::ColorSpace_Gamma;
	ivrResult = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	CB((ivrResult == vr::VRCompositorError_None));

	// Right Eye
	vr::Texture_t rightEyeTexture;
	rightEyeTexture.handle = (void*)(m_pFramebufferResolveRight->GetOGLTextureIndex());
	rightEyeTexture.eType = vr::API_OpenGL;
	rightEyeTexture.eColorSpace = vr::ColorSpace_Gamma;
	ivrResult = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	CB((ivrResult == vr::VRCompositorError_None));
	
	if (m_fVblank && m_fGlFinishHack) {
		//$ HACKHACK. From gpuview profiling, it looks like there is a bug where two renders and a present
		// happen right before and after the vsync causing all kinds of jittering issues. This glFinish()
		// appears to clear that up. Temporary fix while I try to get nvidia to investigate this problem.
		// 1/29/2014 mikesart
		glFinish();
	}

	/*
	// SwapWindow
	{
		SDL_GL_SwapWindow(m_pWindow);
	}
	*/

	// Clear
	{
		// We want to make sure the glFinish waits for the entire present to complete, not just the submission
		// of the command. So, we do a clear here right here so the glFinish will wait fully for the swap.
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

// Flush and wait for swap.
	if (m_fVblank) {
		glFlush();
		glFinish();
	}

	/*
	// Spew out the controller and pose count whenever they change.
	if (m_iTrackedControllerCount != m_iTrackedControllerCount_Last || m_iValidPoseCount != m_iValidPoseCount_Last)
	{
		m_iValidPoseCount_Last = m_iValidPoseCount;
		m_iTrackedControllerCount_Last = m_iTrackedControllerCount;

		dprintf("PoseCount:%d(%s) Controllers:%d\n", m_iValidPoseCount, m_strPoseClasses.c_str(), m_iTrackedControllerCount);
	}
	*/

	glFinish();

Error:
	return r;
}

RESULT OpenVRDevice::SetAndClearRenderSurface(EYE_TYPE eye) {
	RESULT r = R_PASS;

	glEnable(GL_MULTISAMPLE);

	if (eye == EYE_LEFT) {
		m_pFramebufferRenderLeft->BindOGLFramebuffer();
		m_pFramebufferRenderLeft->SetAndClearViewportDepthBuffer();
	}
	else if (eye == EYE_RIGHT) {
		m_pFramebufferRenderRight->BindOGLFramebuffer();
		m_pFramebufferRenderRight->SetAndClearViewportDepthBuffer();
	}

Error:	
	return r;
}

RESULT OpenVRDevice::UnsetRenderSurface(EYE_TYPE eye) {
	RESULT r = R_PASS;
	OpenGLImp *oglimp = dynamic_cast<OpenGLImp*>(m_pHALImp);

	oglimp->glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_MULTISAMPLE);

	if (eye == EYE_LEFT) {
		oglimp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pFramebufferRenderLeft->GetFramebufferIndex());
		oglimp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pFramebufferResolveLeft->GetFramebufferIndex());
	}
	else if (eye == EYE_RIGHT) {
		oglimp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pFramebufferRenderRight->GetFramebufferIndex());
		oglimp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pFramebufferResolveRight->GetFramebufferIndex());
	}

	oglimp->glBlitFramebuffer(0, 0, m_eyeWidth, m_eyeHeight, 0, 0, m_eyeWidth, m_eyeHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	oglimp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	oglimp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


Error:
	return r;
}

RESULT OpenVRDevice::RenderHMDMirror() {
	return R_NOT_IMPLEMENTED;
}

ProjectionMatrix OpenVRDevice::GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) {
	vr::EVREye eyeType = (eye == EYE_LEFT) ? vr::Eye_Left : vr::Eye_Right;

	if (m_pIVRHMD == nullptr)
		return ProjectionMatrix();

	vr::HmdMatrix44_t mat = m_pIVRHMD->GetProjectionMatrix(eyeType, znear, zfar, vr::API_OpenGL);

	ProjectionMatrix projMat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			projMat(i, j) = mat.m[i][j];

	return projMat;
}

ViewMatrix OpenVRDevice::GetViewMatrix(EYE_TYPE eye) {
	vr::EVREye eyeType = (eye == EYE_LEFT) ? vr::Eye_Left : vr::Eye_Right;

	if (m_pIVRHMD == nullptr)
		return ViewMatrix();

	vr::HmdMatrix34_t matEye = m_pIVRHMD->GetEyeToHeadTransform(eyeType);
	Matrix4 mat4Eye = ConvertSteamVRMatrixToMatrix4(matEye);
	mat4Eye.invert();

	/*
	matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
	matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
	matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
	matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	*/

	ViewMatrix viewMat;
	viewMat.identity();

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			viewMat(j, i) = mat4Eye[i * 4 + j];

	Matrix4 mat4View = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
	mat4View.invert();

	viewMat.identity();

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			viewMat(j, i) = mat4View[i * 4 + j];

	return viewMat;
}
#include "OpenVRDevice.h"
#include "HAL/opengl/OGLTexture.h"

OpenVRDevice::OpenVRDevice() :
	m_pIVRHMD(nullptr),
	m_pRenderModels(nullptr),
	m_pCompositor(nullptr),
	m_pFramebufferRenderLeft(nullptr),
	m_pFramebufferResolveLeft(nullptr),
	m_pFramebufferRenderRight(nullptr),
	m_pFramebufferResolveRight(nullptr)
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

	//glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	CR(pOGLRenderFramebuffer->OGLInitialize());
	CR(pOGLRenderFramebuffer->BindOGLFramebuffer());

	/*
	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	*/

	CR(pOGLRenderFramebuffer->MakeOGLDepthbuffer());		// Note: This will create a new depth buffer
	CR(pOGLRenderFramebuffer->InitializeRenderBufferMultisample(GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT, DEFAULT_OPENVR_MULTISAMPLE));

	/*
	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);
	*/

	CR(pOGLRenderFramebuffer->MakeOGLTextureMultisample());
	CR(pOGLRenderFramebuffer->SetOGLTextureToFramebuffer2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE));

	/*
	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);
	*/

	CR(pOGLResolveFramebuffer->OGLInitialize());
	CR(pOGLResolveFramebuffer->BindOGLFramebuffer());

	/*
	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);
	*/

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

	m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &ivrResult);
	CIVRM(ivrResult, "Unable to get render model interface");

	m_strDriver = "No Driver";
	m_strDisplay = "No Display";

	m_strDriver = GetTrackedDeviceString(m_pIVRHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	m_strDisplay = GetTrackedDeviceString(m_pIVRHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

	CR(SetupStereoRenderTargets());

	m_pCompositor = vr::VRCompositor();
	CNM(m_pCompositor, "Failed to initialize IVR compositor");

Error:
	return r;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
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

RESULT OpenVRDevice::UpdateHMD() {
	return R_NOT_IMPLEMENTED;
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
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::SetAndClearRenderSurface(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::UnsetRenderSurface(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
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

	ViewMatrix viewMat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			viewMat(i, j) = matEye.m[i][j];

	return viewMat;
}
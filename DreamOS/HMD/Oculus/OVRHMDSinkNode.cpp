#include "OVRHMDSinkNode.h"

#include "OVRTextureSwapChain.h"
#include "OVR.h"

OVRHMDSinkNode::OVRHMDSinkNode(OpenGLImp *pOGLImp, OVRHMD *pParentHMD) :
	HMDSinkNode("ovrhmdsinknode"),
	m_pParentImp(pOGLImp),
	m_pParentHMD(pParentHMD)
{
	// empty
}

OVRHMDSinkNode::~OVRHMDSinkNode() {
	for (int i = 0; i < HMD_NUM_EYES; i++) {
		if (m_ovrTextureSwapChains[i] != nullptr) {
			delete m_ovrTextureSwapChains[i];
			m_ovrTextureSwapChains[i] = nullptr;
		}
	}
}

RESULT OVRHMDSinkNode::OGLInitialize() {
	RESULT r = R_PASS;

	// Texture Swap Chain
	for (int i = 0; i < HMD_NUM_EYES; i++) {
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_pParentHMD->GetOVRSession(), 
														  ovrEyeType(i), 
														  m_pParentHMD->GetOVRHMDDescription().DefaultEyeFov[i], 1);

		m_pParentHMD->SetEyeWidth(idealTextureSize.w);
		m_pParentHMD->SetEyeHeight(idealTextureSize.h);

		m_ovrTextureSwapChains[i] = new OVRTextureSwapChain(m_pParentImp, m_pParentHMD->GetOVRSession(), idealTextureSize.w, idealTextureSize.h, 1, NULL, 1);
		CR(m_ovrTextureSwapChains[i]->OVRInitialize());
	}

	// Front load Layer Initialization
	m_ovrLayer.Header.Type = ovrLayerType_EyeFov;
	m_ovrLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

Error:
	return r;
}

RESULT OVRHMDSinkNode::SetupConnections() {
	RESULT r = R_PASS;

	OGLFramebuffer **ppInputFramebufferLeft = &(m_ovrTextureSwapChains[EYE_LEFT]->m_pOGLRenderFramebuffer);
	OGLFramebuffer **ppInputFramebufferRight = &(m_ovrTextureSwapChains[EYE_RIGHT]->m_pOGLRenderFramebuffer);

	CR(MakeInput<OGLFramebuffer>("input_framebuffer_lefteye", ppInputFramebufferLeft));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer_righteye", ppInputFramebufferRight));

	m_pInputConnection[EYE_LEFT] = Connection("input_framebuffer_lefteye", CONNECTION_TYPE::INPUT);
	m_pInputConnection[EYE_RIGHT] = Connection("input_framebuffer_righteye", CONNECTION_TYPE::INPUT);

Error:
	return r;
}

RESULT OVRHMDSinkNode::RenderNode(long frameID) {
	RESULT r = R_PASS;

	auto pCamera = m_pParentImp->GetCamera();
	int pxViewportWidth = m_pParentImp->GetViewport().Width();
	int pxViewportHeight = m_pParentImp->GetViewport().Height();
	int channels = 4;

	pCamera->ResizeCamera(m_pParentHMD->GetEyeWidth(), m_pParentHMD->GetEyeHeight());

	for (int i = 0; i < HMD_NUM_EYES; i++) {
		pCamera->SetCameraEye((EYE_TYPE)(i));

		m_pParentImp->ClearHALBuffers();
		m_pParentImp->ConfigureHAL();

		m_pParentHMD->SetAndClearRenderSurface((EYE_TYPE)(i));

		CR(m_pInputConnection[i]->RenderConnections(frameID));

		// Commit Frame to HMD
		m_pParentHMD->UnsetRenderSurface((EYE_TYPE)(i));
		m_pParentHMD->CommitSwapChain((EYE_TYPE)(i));
	}

	m_pParentHMD->SubmitFrame();
	m_pParentHMD->RenderHMDMirror();

Error:
	return r;
}

RESULT OVRHMDSinkNode::CommitSwapChain(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->Commit();
}

RESULT OVRHMDSinkNode::SetAndClearRenderSurface(EYE_TYPE eye) {
	ovrSession OVRSession = m_pParentHMD->GetOVRSession();
	ovrHmdDesc OVRHMDDesc = m_pParentHMD->GetOVRHMDDescription();

	ovrEyeType eyeType = (eye == EYE_LEFT) ? ovrEye_Left : ovrEye_Right;

	m_ovrEyeRenderDescription[eyeType] = ovr_GetRenderDesc(OVRSession, eyeType, OVRHMDDesc.DefaultEyeFov[eyeType]);

	return m_ovrTextureSwapChains[eye]->SetAndClearRenderSurface();
}

RESULT OVRHMDSinkNode::UnsetRenderSurface(EYE_TYPE eye) {
	return m_ovrTextureSwapChains[eye]->UnsetRenderSurface();
}

RESULT OVRHMDSinkNode::SubmitFrame() {
	RESULT r = R_PASS;

	ovrSession OVRSession = m_pParentHMD->GetOVRSession();
	ovrHmdDesc OVRHMDDesc = m_pParentHMD->GetOVRHMDDescription();

	// TODO: Split this across the eyes 
	long long frameIndex = 0;
	ovrPosef EyeRenderPose[2];
	ovrVector3f HmdToEyeOffset[2] = { m_ovrEyeRenderDescription[0].HmdToEyeOffset, m_ovrEyeRenderDescription[1].HmdToEyeOffset };

	double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	ovr_GetEyePoses(OVRSession, frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	for (int eye = 0; eye < 2; eye++) {
		m_ovrLayer.ColorTexture[eye] = m_ovrTextureSwapChains[eye]->GetOVRTextureSwapChain();
		m_ovrLayer.Viewport[eye] = m_ovrTextureSwapChains[eye]->GetOVRViewportRecti();
		m_ovrLayer.Fov[eye] = OVRHMDDesc.DefaultEyeFov[eye];
		m_ovrLayer.RenderPose[eye] = EyeRenderPose[eye];
		m_ovrLayer.SensorSampleTime = sensorSampleTime;
	}

	ovrLayerHeader* layers = &m_ovrLayer.Header;

	CR((RESULT)ovr_SubmitFrame(OVRSession, 0, nullptr, &layers, 1));

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
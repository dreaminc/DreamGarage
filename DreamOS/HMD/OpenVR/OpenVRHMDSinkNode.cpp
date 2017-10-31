#include "OpenVRHMDSinkNode.h"
#include "OpenVRDevice.h"

#include "HAL/opengl/OGLFramebuffer.h"

#include "HAL/opengl/OGLTexture.h"

OpenVRHMDSinkNode::OpenVRHMDSinkNode(OpenGLImp *pOGLImp, OpenVRDevice *pParentHMD) :
	HMDSinkNode("openvrhmdsinknode"),
	m_pParentImp(pOGLImp),
	m_pParentHMD(pParentHMD)
{
	// empty
}

OpenVRHMDSinkNode::~OpenVRHMDSinkNode() {
	// TODO:
}

RESULT OpenVRHMDSinkNode::OGLInitialize() {
	RESULT r = R_PASS;

	m_pCompositor = vr::VRCompositor();
	CNM(m_pCompositor, "Failed to initialize IVR compositor");

Error:
	return r;
}

RESULT OpenVRHMDSinkNode::SetupConnections() {
	RESULT r = R_PASS;

	CR(MakeInput<OGLFramebuffer>("input_framebuffer_lefteye", &m_pOGLInputFramebuffer));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer_righteye", &m_pOGLInputFramebuffer));

	m_pInputConnection[EYE_LEFT] = Connection("input_framebuffer_lefteye", CONNECTION_TYPE::INPUT);
	m_pInputConnection[EYE_RIGHT] = Connection("input_framebuffer_righteye", CONNECTION_TYPE::INPUT);

Error:
	return r;
}

// TODO: this should go up into the sink node I reckon 
RESULT OpenVRHMDSinkNode::RenderNode(long frameID) {
	RESULT r = R_PASS;

	auto pCamera = m_pParentImp->GetCamera();

	pCamera->ResizeCamera(m_pParentHMD->GetEyeWidth(), m_pParentHMD->GetEyeHeight());

	for (int i = 0; i < HMD_NUM_EYES; i++) {
		m_pParentImp->ClearHALBuffers();
		m_pParentImp->ConfigureHAL();

		pCamera->SetCameraEye((EYE_TYPE)(i));

		CR(m_pInputConnection[i]->RenderConnections(frameID));

		CR(UnsetRenderSurface((EYE_TYPE)(i)));
	}

	m_pParentHMD->SubmitFrame();

	//m_pParentHMD->RenderHMDMirror();
	RenderMirrorToBackBuffer();

Error:
	return r;
}

RESULT OpenVRHMDSinkNode::UnsetRenderSurface(EYE_TYPE eye) {
	RESULT r = R_PASS;
	vr::EVRCompositorError ivrResult = vr::VRCompositorError_None;

	vr::Texture_t eyeTexture = { 
		(void*)(uintptr_t)m_pOGLInputFramebuffer->GetColorAttachment()->GetOGLTextureIndex(), 
		vr::TextureType_OpenGL, 
		vr::ColorSpace_Gamma 
	};

	ivrResult = vr::VRCompositor()->Submit((vr::EVREye)(eye), &eyeTexture);
	CB((ivrResult == vr::VRCompositorError_None));


	//vr::Texture_t leftEyeTexture 
	//vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	//vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	//vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

Error:
	return r;
}

RESULT OpenVRHMDSinkNode::RenderMirrorToBackBuffer() {
	RESULT r = R_PASS;

	int fbWidth = m_pOGLInputFramebuffer->GetWidth();

	// TODO: Move this to framebuffer
	CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pOGLInputFramebuffer->GetFramebufferIndex()));
	CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

	CR(m_pParentImp->glBlitFramebuffer(0, m_pOGLInputFramebuffer->GetHeight(), 
									   m_pOGLInputFramebuffer->GetWidth(), 0, 
									   0, m_pParentImp->GetViewport().Height(), m_pParentImp->GetViewport().Width(), 0,
								       GL_COLOR_BUFFER_BIT, GL_NEAREST));

	CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

Error:
	return r;
}

RESULT OpenVRHMDSinkNode::SubmitFrame() {
	RESULT r = R_PASS;

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

//Error:
	return r;
}
#include "OGLCameraViewportDisplay.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"

OGLCameraViewportDisplay::OGLCameraViewportDisplay(OpenGLImp *pParentImp) :
	SinkNode("oglviewportdisplaycamera"),
	m_pParentImp(pParentImp)
{
	// empty
}

RESULT OGLCameraViewportDisplay::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));

	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLInputFramebuffer));

Error:
	return r;
}

RESULT OGLCameraViewportDisplay::PreProcessNode(long frameID) {
	RESULT r = R_PASS;

	CNR(m_pCamera, R_SKIPPED);

	m_pCamera->SetCameraEye(EYE_MONO);

	m_pParentImp->SetViewTarget(EYE_MONO, m_pCamera->GetViewWidth(), m_pCamera->GetViewHeight());

Error:
	return r;
}

RESULT OGLCameraViewportDisplay::ProcessNode(long frameID) {
	RESULT r = R_PASS;
	
	// TODO: Implement this
	// A lot of this is rendering the inputs, this should actually go into DNode

	//int pxViewportWidth = m_pCamera->GetViewWidth();
	//int pxViewportHeight = m_pCamera->GetViewHeight();

	int pxViewportWidth = m_pOGLInputFramebuffer->GetWidth();
	int pxViewportHeight = m_pOGLInputFramebuffer->GetHeight();

	int channels = 4;

	m_pParentImp->SetViewTarget(EYE_MONO, pxViewportWidth, pxViewportHeight);

	if (m_pOGLInputFramebuffer != nullptr) {
		// present to display

		m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pOGLInputFramebuffer->GetFramebufferIndex());
		m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// Copy to Destination 
		m_pParentImp->glBlitFramebuffer(0, 0, m_pOGLInputFramebuffer->GetWidth(), m_pOGLInputFramebuffer->GetHeight(), 
										0, 0, pxViewportWidth, pxViewportHeight, 
										GL_COLOR_BUFFER_BIT, GL_NEAREST);

		m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

Error:
	return r;
}
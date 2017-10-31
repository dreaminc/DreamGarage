#include "OGLHMDDisplay.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"

OGLHMDDisplay::OGLHMDDisplay(OpenGLImp *pParentImp) :
	SinkNode("oglhmddisplay"),
	m_pParentImp(pParentImp)
{
	// empty
}

RESULT OGLHMDDisplay::SetupConnections() {
	RESULT r = R_PASS;

	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLInputFramebuffer));

Error:
	return r;
}

RESULT OGLHMDDisplay::ProcessNode(long frameID) {
	RESULT r = R_PASS;
	
	// TODO: Implement this
	// A lot of this is rendering the inputs, this should actually go into DNode

	auto pCamera = m_pParentImp->GetCamera();

	int pxViewportWidth = m_pParentImp->GetViewport().Width();
	int pxViewportHeight = m_pParentImp->GetViewport().Height();
	int channels = 4;

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


	CR(r);

Error:
	return r;
}
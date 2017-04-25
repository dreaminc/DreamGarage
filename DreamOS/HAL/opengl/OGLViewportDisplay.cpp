#include "OGLViewportDisplay.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"

OGLViewportDisplay::OGLViewportDisplay(OpenGLImp *pParentImp) :
	SinkNode("oglviewportdisplay"),
	m_pParentImp(pParentImp)
{
	// empty
}

RESULT OGLViewportDisplay::SetupConnections() {
	RESULT r = R_PASS;

	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLInputFramebuffer));

Error:
	return r;
}

RESULT OGLViewportDisplay::ProcessNode(long frameID) {
	RESULT r = R_PASS;
	
	// TODO: Implement this
	// A lot of this is rendering the inputs, this should actually go into DNode

	if (m_pOGLInputFramebuffer != nullptr) {
		// present to display

		m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pOGLInputFramebuffer->GetFramebufferIndex());
		m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// Copy to dest 
		m_pParentImp->glBlitFramebuffer(0, 0, m_pOGLInputFramebuffer->GetWidth(), m_pOGLInputFramebuffer->GetHeight(), 
										0, 0, m_pOGLInputFramebuffer->GetWidth(), m_pOGLInputFramebuffer->GetHeight(), 
										GL_COLOR_BUFFER_BIT, GL_NEAREST);

		m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}


	CR(r);

Error:
	return r;
}
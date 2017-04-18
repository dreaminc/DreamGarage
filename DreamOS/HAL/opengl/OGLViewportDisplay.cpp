#include "OGLViewportDisplay.h"

OGLViewportDisplay::OGLViewportDisplay(OpenGLImp *pParentImp) :
	SinkNode("oglviewportdisplay"),
	m_pParentImp(pParentImp)
{
	// empty
}

RESULT OGLViewportDisplay::SetupConnections() {
	RESULT r = R_PASS;

	CR(MakeInput("input_framebuffer"));

Error:
	return r;
}

RESULT OGLViewportDisplay::ProcessNode() {
	RESULT r = R_PASS;
	
	// TODO: Implement this
	// A lot of this is rendering the inputs, this should actually go into DNode

	CR(r);

Error:
	return r;
}
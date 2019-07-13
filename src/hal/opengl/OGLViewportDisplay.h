#ifndef OGL_VIEWPORT_DISPLAY_H_
#define OGL_VIEWPORT_DISPLAY_H_

// Dream OS
// DreamOS/HAL/opengl/OGLViewportDisplay.h
// This represents a view port display and respect OGL contexts as needed

// TODO: Move more of the OpenGLImp stuff into here (like the context)

#include "HAL/Pipeline/SinkNode.h"

class OpenGLImp;
class OGLFramebuffer;

class OGLViewportDisplay : public SinkNode {
public:
	OGLViewportDisplay(OpenGLImp *pParentImp);

	virtual RESULT SetupConnections() override;
	virtual RESULT PreProcessNode(long frameID = 0) override;
	virtual RESULT ProcessNode(long frameID = 0) override;

private:
	OGLFramebuffer *m_pOGLInputFramebuffer = nullptr;

private:
	OpenGLImp* m_pParentImp = nullptr;
};

#endif	// ! OGL_VIEWPORT_DISPLAY_H_
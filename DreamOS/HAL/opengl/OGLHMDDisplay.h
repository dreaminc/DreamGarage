#ifndef OGL_HMD_DISPLAY_H_
#define OGL_HMD_DISPLAY_H_

// Dream OS
// DreamOS/HAL/opengl/OGLHMDDisplay.h
// This represents a generic OpenGL HMD 

// TODO: Move more of the OpenGLImp stuff into here (like the context)

#include "HAL/Pipeline/SinkNode.h"

class OpenGLImp;
class OGLFramebuffer;

class OGLHMDDisplay : public SinkNode {
public:
	OGLHMDDisplay(OpenGLImp *pParentImp);

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID = 0) override;

private:
	OGLFramebuffer *m_pOGLInputFramebuffer = nullptr;

private:
	OpenGLImp* m_pParentImp = nullptr;
};

#endif	// ! OGL_HMD_DISPLAY_H_
#ifndef OGL_HMD_DISPLAY_H_
#define OGL_HMD_DISPLAY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLHMDDisplay.h

// This represents a generic OpenGL HMD 

// TODO: Move more of the OGLImp stuff into here (like the context)

#include "pipeline/SinkNode.h"

class OGLImp;
class OGLFramebuffer;

class OGLHMDDisplay : public SinkNode {
public:
	OGLHMDDisplay(OGLImp *pParentImp);

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID = 0) override;

private:
	OGLFramebuffer *m_pOGLInputFramebuffer = nullptr;

private:
	OGLImp* m_pParentImp = nullptr;
};

#endif	// ! OGL_HMD_DISPLAY_H_
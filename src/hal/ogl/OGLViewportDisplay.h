#ifndef OGL_VIEWPORT_DISPLAY_H_
#define OGL_VIEWPORT_DISPLAY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLViewportDisplay.h

// This represents a view port display and respect OGL contexts as needed

// TODO: Move more of the OGLImp stuff into here (like the context)

#include "pipeline/SinkNode.h"

class OGLImp;
class OGLFramebuffer;

class OGLViewportDisplay : public SinkNode {
public:
	OGLViewportDisplay(OGLImp *pParentImp);

	virtual RESULT SetupConnections() override;
	virtual RESULT PreProcessNode(long frameID = 0) override;
	virtual RESULT ProcessNode(long frameID = 0) override;

private:
	OGLFramebuffer *m_pOGLInputFramebuffer = nullptr;

private:
	OGLImp* m_pParentImp = nullptr;
};

#endif	// ! OGL_VIEWPORT_DISPLAY_H_
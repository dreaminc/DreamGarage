#ifndef OGL_CAMERA_VIEWPORT_DISPLAY_H_
#define OGL_CAMERA_VIEWPORT_DISPLAY_H_

// Dream OS
// DreamOS/HAL/opengl/OGLCameraViewportDisplay.h
// This represents a view port display and respect OGL contexts as needed
// but uses an external camera (vs. the HAL implementation based camera)

#include "HAL/Pipeline/SinkNode.h"

class OpenGLImp;
class OGLFramebuffer;

class stereocamera;

class OGLCameraViewportDisplay : public SinkNode {
public:
	OGLCameraViewportDisplay(OpenGLImp *pParentImp);

	virtual RESULT SetupConnections() override;
	virtual RESULT PreProcessNode(long frameID = 0) override;
	virtual RESULT ProcessNode(long frameID = 0) override;

private:
	OGLFramebuffer *m_pOGLInputFramebuffer = nullptr;
	stereocamera *m_pCamera = nullptr;

private:
	OpenGLImp* m_pParentImp = nullptr;
};

#endif	// ! OGL_CAMERA_VIEWPORT_DISPLAY_H_
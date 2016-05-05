#ifndef OGL_FRAMEBUFFER_H_
#define OGL_FRAMEBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLFramebuffer.h
// OpenGL Framebuffer Primitve

#include "OpenGLCommon.h"

class OpenGLImp;
class OGLTexture;

class OGLFramebuffer {
	OGLFramebuffer(OpenGLImp *pParentImp);
	~OGLFramebuffer();

	RESULT OGLInitialize();

private:
	OpenGLImp *m_pParentImp;

	GLuint m_framebufferIndex;

	OGLTexture *m_pOGLTexture;
};

#endif // ! OGL_FRAMEBUFFER_H_


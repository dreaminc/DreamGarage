#ifndef OGL_FRAMEBUFFER_H_
#define OGL_FRAMEBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLFramebuffer.h
// OpenGL Framebuffer Primitve

#include "OpenGLCommon.h"
#include "Primitives/Framebuffer.h"

class OpenGLImp;
class OGLTexture;

#define NUM_OGL_DRAW_BUFFERS 1

class OGLFramebuffer : public framebuffer {
public:
	OGLFramebuffer(OpenGLImp *pParentImp, int width, int height, int channels);
	~OGLFramebuffer();

	RESULT OGLInitialize();
	RESULT BindOGLFramebuffer();

private:
	OpenGLImp *m_pParentImp;

	GLenum m_drawBuffers[NUM_OGL_DRAW_BUFFERS];
	GLuint m_framebufferIndex;
	GLuint m_renderbufferIndex;

	OGLTexture *m_pOGLTexture;
};

#endif // ! OGL_FRAMEBUFFER_H_


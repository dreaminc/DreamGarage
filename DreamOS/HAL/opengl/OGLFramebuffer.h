#ifndef OGL_FRAMEBUFFER_H_
#define OGL_FRAMEBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLFramebuffer.h
// OpenGL Framebuffer Primitve

#include "OpenGLCommon.h"
#include "Primitives/Framebuffer.h"

#include "OGLDepthbuffer.h"

class OpenGLImp;
class OGLTexture;

#define NUM_OGL_DRAW_BUFFERS 1

class OGLFramebuffer : public framebuffer {
public:

	OGLFramebuffer(OpenGLImp *pParentImp);
	OGLFramebuffer(OpenGLImp *pParentImp, int width, int height, int channels);
	OGLFramebuffer(OpenGLImp *pParentImp, GLuint textureID, int width, int height, int channels);

	~OGLFramebuffer();

	RESULT OGLInitialize();
	RESULT BindOGLFramebuffer(GLuint textureIndex);
	RESULT UnbindOGLFramebuffer();

	RESULT SetOGLTexture(GLuint textureIndex = NULL);
	RESULT SetOGLDrawBuffers(int numDrawBuffers);
	RESULT SetOGLDepthbuffer(OGLDepthbuffer *pOGLDepthbuffer = nullptr);

	GLuint GetOGLTextureIndex();

	RESULT SetAndClearViewport();

	GLuint GetFramebufferIndex();

private:
	OpenGLImp *m_pParentImp;

	//GLenum m_drawBuffers[NUM_OGL_DRAW_BUFFERS];
	GLenum *m_pDrawBuffers;
	int m_pDrawBuffers_n;
	
	GLuint m_framebufferIndex;
	GLuint m_renderbufferIndex;

	OGLDepthbuffer *m_pOGLDepthbuffer;

	OGLTexture *m_pOGLTexture;
};

#endif // ! OGL_FRAMEBUFFER_H_


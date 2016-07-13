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

	
	// TODO: Consolidate - there's a conflict with the texture swap chain stuff and this, so the code paths are different 
	RESULT OGLInitialize(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT);
	RESULT BindOGLFramebuffer();
	RESULT BindOGLDepthBuffer();
	RESULT UnbindOGLFramebuffer();

	RESULT AttachOGLTexture(GLuint textureIndex);
	RESULT AttachOGLDepthbuffer();

	RESULT SetOGLTextureToFramebuffer(GLenum target, GLenum attachment);
	RESULT MakeOGLTexture();
	RESULT SetOGLTexture(GLuint textureIndex = NULL);
	RESULT SetOGLDrawBuffers(int numDrawBuffers);
	RESULT SetOGLDepthbuffer(OGLDepthbuffer *pOGLDepthbuffer);
	RESULT MakeOGLDepthbuffer();

	GLuint GetOGLTextureIndex();

	RESULT SetAndClearViewport();

	GLuint GetFramebufferIndex();

	RESULT SetAndClearViewportDepthBuffer();
	RESULT BindToScreen(int pxWidth, int pxHeight);

	RESULT InitializeDepthBuffer(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT);
	RESULT InitializeRenderBuffer(GLenum internalDepthFormat, GLenum typeDepth);

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


#ifndef OGL_DEPTHBUFFER_H_
#define OGL_DEPTHBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLDepthbuffer.h
// OpenGL Depthbuffer Primitive

#include "HAL/opengl/OpenGLImp.h"
//#include "Primitives/Framebuffer.h"

class OGLRenderbuffer;

class OGLAttachment {
public:
	OGLAttachment (OpenGLImp *pParentImp, int width, int height, int sampleCount = 1);
	~OGLAttachment ();

	// TODO: This is a temporary approach
	RESULT OGLInitializeRenderBuffer();

	// TODO: Potentially combine with the upper function - use mutli sample or not based on multisample value
	RESULT OGLInitializeRenderBufferMultisample(GLenum internalDepthFormat, GLenum typeDepth, int multisample = 4);
	RESULT OGLInitialize(GLenum internalFormat = GL_DEPTH_COMPONENT24, GLenum type = GL_UNSIGNED_INT);
	
	GLuint GetOGLRenderbufferIndex();
	GLuint GetOGLTextureIndex();

	RESULT Resize(int pxWidth, int pxHeight);

	bool HasRenderbuffer() {
		return (m_pOGLRenderbuffer != nullptr);
	}

	bool HasTexture() {
		return (m_pOGLTexture != nullptr);
	}

private:
	int m_width;
	int m_height;
	int m_sampleCount;

private:
	OpenGLImp *m_pParentImp = nullptr;

	OGLRenderbuffer *m_pOGLRenderbuffer = nullptr;
	OGLTexture *m_pOGLTexture = nullptr;
};

#endif // ! OGL_DEPTHBUFFER_H_
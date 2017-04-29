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
class OGLAttachment;

#define NUM_OGL_DRAW_BUFFERS 1

class OGLFramebuffer : public framebuffer {
public:

	OGLFramebuffer(OpenGLImp *pParentImp);
	OGLFramebuffer(OpenGLImp *pParentImp, int width, int height, int channels);
	OGLFramebuffer(OpenGLImp *pParentImp, GLuint textureID, int width, int height, int channels);

	~OGLFramebuffer();

	// TODO: Consolidate - there's a conflict with the texture swap chain stuff and this, so the code paths are different 
	RESULT OGLInitialize(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT);
	
	RESULT Bind();
	RESULT Unbind();
	RESULT UnbindAttachments();

	RESULT SetAndClearViewport(bool fColor = true, bool fDepth = true);
	RESULT BindToScreen(int pxWidth, int pxHeight);

	RESULT SetOGLTextureToFramebuffer2D(GLenum target, GLenum attachment, GLenum textarget);
	RESULT SetOGLDepthbufferTextureToFramebuffer(GLenum target, GLenum attachment);
	RESULT SetDepthTexture(int textureNumber);
	
	RESULT MakeOGLTextureMultisample();
	RESULT InitializeOGLDrawBuffers(int numDrawBuffers);

	// TOOD: 
	RESULT InitializeDepthAttachment(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT);
	RESULT InitializeRenderBuffer(GLenum internalDepthFormat, GLenum typeDepth);
	RESULT InitializeRenderBufferMultisample(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT, int multisample = 4);

	RESULT ResizeFramebuffer(int pxWidth, int pxHeight);

	GLuint GetFramebufferIndex();
private:
	// Common attachments: GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0
	RESULT AttachRenderBuffer(OGLRenderbuffer *pOGLRenderbuffer, GLenum attachment, GLenum target = GL_FRAMEBUFFER, GLenum renderbuffertarget = GL_RENDERBUFFER);
	RESULT AttachTexture(OGLTexture *pOGLTexture, GLenum attachment, GLenum target = GL_FRAMEBUFFER, GLenum renderbuffertarget = GL_TEXTURE_2D, GLint levels = 0);

private:
	OpenGLImp *m_pParentImp;

	GLenum *m_pDrawBuffers;
	int m_pDrawBuffers_n;
	
	GLuint m_framebufferIndex;

	OGLAttachment *m_pOGLDepthAttachment = nullptr;
	OGLAttachment* m_pOGLColorAttachment = nullptr;
	
	// TODO: Stencil attachments 
};

#endif // ! OGL_FRAMEBUFFER_H_


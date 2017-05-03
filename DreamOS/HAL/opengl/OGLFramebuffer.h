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

	~OGLFramebuffer();

	// TODO: Consolidate - there's a conflict with the texture swap chain stuff and this, so the code paths are different 
	RESULT OGLInitialize(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT);
	
	RESULT Bind();
	RESULT Unbind();
	RESULT UnbindAttachments();

	RESULT SetAndClearViewport(bool fColor = true, bool fDepth = true);
	RESULT BindToScreen(int pxWidth, int pxHeight);

	// TODO: These should not be directly used 
	RESULT SetOGLTextureToFramebuffer2D(GLenum target, GLenum attachment, GLenum textarget);
	RESULT SetDepthTexture(int textureNumber);
	
	RESULT InitializeOGLDrawBuffers(int numDrawBuffers);

	RESULT MakeDepthAttachment();
	RESULT MakeColorAttachment();
	RESULT DeleteColorAttachment();
	RESULT DeleteDepthAttachment();

	OGLAttachment* GetDepthAttachment() { return m_pOGLDepthAttachment; }
	OGLAttachment* GetColorAttachment() { return m_pOGLColorAttachment; }

	// TODO: 
	RESULT InitializeColorAttachment(OGLTexture *pOGLTexture);
	RESULT InitializeRenderBuffer(GLenum internalDepthFormat, GLenum typeDepth);

	RESULT Resize(int pxWidth, int pxHeight, GLenum internalDepthFormat = GL_DEPTH_COMPONENT16, GLenum typeDepth = GL_FLOAT);

	GLuint GetFramebufferIndex();

private:
	OpenGLImp *m_pParentImp;

	GLenum *m_pDrawBuffers;
	int m_pDrawBuffers_n;
	
	GLuint m_framebufferIndex;

	OGLAttachment* m_pOGLDepthAttachment = nullptr;
	OGLAttachment* m_pOGLColorAttachment = nullptr;
	
	// TODO: Stencil attachments 
};

#endif // ! OGL_FRAMEBUFFER_H_


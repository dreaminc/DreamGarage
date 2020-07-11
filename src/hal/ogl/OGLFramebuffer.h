#ifndef OGL_FRAMEBUFFER_H_
#define OGL_FRAMEBUFFER_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLFramebuffer.h

// OpenGL Framebuffer Primitve

#include <map>

#include "OGLCommon.h"

#include "core/primitives/framebuffer.h"

class OGLImp;
class OGLTexture;
class OGLAttachment;

#define NUM_OGL_DRAW_BUFFERS 1

class OGLFramebuffer : public framebuffer {
public:

	OGLFramebuffer(OGLImp *pParentImp);
	OGLFramebuffer(OGLImp *pParentImp, int width, int height, int channels);

	virtual ~OGLFramebuffer() override;

	virtual texture *GetColorTexture() override;
	virtual RESULT ClearAttachments() override;

	// TODO: Consolidate - there's a conflict with the texture swap chain stuff and this, so the code paths are different 
	RESULT OGLInitialize(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT);
	
	RESULT Bind();
	RESULT Unbind();
	RESULT UnbindAttachments();

	//RESULT SetAndClearViewport(bool fColor = true, bool fDepth = true);
	RESULT SetAndClearViewport(bool fColor = true, bool fDepth = true, bool fBind = false);
	RESULT BindToScreen(int pxWidth, int pxHeight);

	// TODO: These should not be directly used 
	RESULT SetOGLCubemapToFramebuffer2D(GLenum target, GLenum attachment, GLenum textarget);
	RESULT SetOGLTextureToFramebuffer2D(GLenum target, GLenum attachment, GLenum textarget);
	RESULT SetDepthTexture(int textureNumber);
	
	RESULT InitializeOGLDrawBuffers(int numDrawBuffers);

	//RESULT AddColorAttachment(int index);
	//RESULT ClearColorAttachments();
	//OGLAttachment* GetColorAttachment(int index);

	RESULT MakeColorAttachment();
	RESULT DeleteColorAttachment();

	RESULT MakeDepthAttachment();
	RESULT DeleteDepthAttachment();

	OGLAttachment* GetDepthAttachment() { return m_pOGLDepthAttachment; }
	OGLAttachment* GetColorAttachment() { return m_pOGLColorAttachment; }

	// TODO: 
	RESULT InitializeColorAttachment(OGLTexture *pOGLTexture);
	RESULT InitializeRenderBuffer(GLenum internalDepthFormat, GLenum typeDepth);

	RESULT Resize(int pxWidth, int pxHeight, GLenum internalDepthFormat = GL_DEPTH_COMPONENT16, GLenum typeDepth = GL_FLOAT);

	GLuint GetFramebufferIndex();

	RESULT SetSampleCount(int samples) { m_samples = samples; return R_PASS; }
	int GetSampleCount() { return m_samples; }

	// Always check that our framebuffer is ok
	RESULT CheckStatus();

	RESULT SetClearColor(color c);

private:
	OGLImp *m_pParentImp = nullptr;

	GLenum *m_pDrawBuffers = nullptr;
	int m_pDrawBuffers_n;
	
	GLuint m_framebufferIndex;

	OGLAttachment* m_pOGLDepthAttachment = nullptr;

	OGLAttachment* m_pOGLColorAttachment = nullptr;
	//std::map<int, OGLAttachment*> m_oglColorAttachments;
	
	// TODO: Stencil attachments 

	color m_colorClear;

private:
	int m_samples = 1;
};

#endif // ! OGL_FRAMEBUFFER_H_


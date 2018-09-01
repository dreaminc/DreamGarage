#ifndef OGL_DEPTHBUFFER_H_
#define OGL_DEPTHBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLDepthbuffer.h
// OpenGL Depthbuffer Primitive

#include "HAL/opengl/OpenGLImp.h"
//#include "Primitives/Framebuffer.h"

class OGLRenderbuffer;
class OGLTexture;
class OGLCubemap;

class OGLAttachment {
public:
	OGLAttachment(OpenGLImp *pParentImp, int width, int height, int channels = 3, int sampleCount = 1);
	~OGLAttachment ();

	// TODO: This is a temporary approach
	RESULT OGLInitializeRenderBuffer();

	// TODO: Potentially combine with the upper function - use mutli sample or not based on multisample value
	RESULT OGLInitialize(OGLTexture *pOGLTexture);
	//RESULT OGLInitializeRenderBufferMultisample(GLenum internalDepthFormat = GL_DEPTH_COMPONENT24, GLenum typeDepth = GL_UNSIGNED_INT, int multisample = 4);
	
	RESULT AttachToFramebuffer(GLenum target, GLenum attachment);
	RESULT AttachTextureToFramebuffer(GLenum target, GLenum attachment);
	RESULT AttachRenderBufferToFramebuffer(GLenum target = GL_FRAMEBUFFER, GLenum attachment = GL_DEPTH_ATTACHMENT, GLenum renderbuffertarget = GL_RENDERBUFFER);

	RESULT MakeOGLTextureMultisample();
	RESULT MakeOGLDepthTexture(texture::type type = texture::type::TEXTURE_2D, GLenum internalGLFormat = GL_DEPTH_COMPONENT24, GLenum pixelDataType = GL_UNSIGNED_INT);
	RESULT MakeOGLTexture(texture::type type = texture::type::TEXTURE_2D);

	GLuint GetOGLRenderbufferIndex();
	GLuint GetOGLTextureIndex();
	GLenum GetOGLTextureTarget();

	// Cubemap
	RESULT MakeOGLCubemap();
	RESULT AttachCubemapToFramebuffer(GLenum target, GLenum attachment);
	GLuint GetOGLCubemapIndex();
	GLenum GetOGLCubemapTarget();

	OGLTexture *GetOGLTexture() { return m_pOGLTexture; }
	OGLCubemap *GetOGLCubemap() { return m_pOGLCubemap; }
	OGLRenderbuffer *GetOGLRenderBuffer() { return m_pOGLRenderbuffer; }

	RESULT Resize(int pxWidth, int pxHeight);

	bool HasRenderbuffer() {
		return (m_pOGLRenderbuffer != nullptr);
	}

	bool HasTexture() {
		return (m_pOGLTexture != nullptr);
	}

	RESULT SetSampleCount(int samples) {
		if (m_sampleCount != 0) {
			return R_FAIL;
		}
		
		m_sampleCount = samples;
		return R_PASS;
	}

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	int GetChannels() { return m_channels; }
	int GetSampleCount() { return m_sampleCount; }

private:
	int m_width;
	int m_height;
	int m_channels;
	int m_sampleCount = 0;

private:
	OpenGLImp *m_pParentImp = nullptr;

	OGLRenderbuffer *m_pOGLRenderbuffer = nullptr;
	OGLTexture *m_pOGLTexture = nullptr;
	OGLCubemap *m_pOGLCubemap = nullptr;
};

#endif // ! OGL_DEPTHBUFFER_H_
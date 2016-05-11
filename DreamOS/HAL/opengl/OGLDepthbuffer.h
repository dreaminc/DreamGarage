#ifndef OGL_DEPTHBUFFER_H_
#define OGL_DEPTHBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLDepthbuffer.h
// OpenGL Depthbuffer Primitive

#include "HAL/opengl/OpenGLImp.h"
//#include "Primitives/Framebuffer.h"

class OGLDepthbuffer {
public:
	OGLDepthbuffer(OpenGLImp *pParentImp, int width, int height/*, int sampleCount*/) :
		m_width(width),
		m_height(height),
		//m_sampleCount(sampleCount),
		m_pParentImp(pParentImp)
	{
		//assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.
		
		// Empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		// TODO: Replace with texture object instead?
		CR(m_pParentImp->GenerateTextures(1, &m_depthbufferIndex));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_depthbufferIndex));

		CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		CR(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLenum internalFormat = GL_DEPTH_COMPONENT24;
		GLenum type = GL_UNSIGNED_INT;

		/*
		if (GLE_ARB_depth_buffer_float) {
		internalFormat = GL_DEPTH_COMPONENT32F;
		type = GL_FLOAT;
		}
		*/

		/*
		// The depth buffer
		// Implement OGLRenderbuffer
		// TODO: Create a depth buffer object (like OGLTexture / Framebuffer
		CR(m_pParentImp->glGenRenderbuffers(1, &m_renderbufferIndex));
		CR(m_pParentImp->glBindRenderbuffer(GL_RENDERBUFFER, m_renderbufferIndex));
		CR(m_pParentImp->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));
		CR(m_pParentImp->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbufferIndex));
		*/

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, type, NULL);

	Error:
		return r;
	}

	GLuint GetOGLDepthbufferIndex() {
		return m_depthbufferIndex;
	}

	~OGLDepthbuffer() {
		if (m_depthbufferIndex) {
			glDeleteTextures(1, &m_depthbufferIndex);
			m_depthbufferIndex = 0;
		}
	}

private:
	OpenGLImp *m_pParentImp;

	GLuint m_depthbufferIndex;

	int m_width;
	int m_height;
	//int m_sampleCount;
};

#endif // ! OGL_DEPTHBUFFER_H_
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
	OGLDepthbuffer(OpenGLImp *pParentImp, int width, int height, int sampleCount) :
		m_width(width),
		m_height(height),
		m_sampleCount(sampleCount),
		m_pParentImp(pParentImp)
	{
		assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		glGenTextures(1, &m_textureIndex);
		glBindTexture(GL_TEXTURE_2D, m_textureIndex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLenum internalFormat = GL_DEPTH_COMPONENT24;
		GLenum type = GL_UNSIGNED_INT;

		/*
		if (GLE_ARB_depth_buffer_float) {
		internalFormat = GL_DEPTH_COMPONENT32F;
		type = GL_FLOAT;
		}
		*/

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, type, NULL);

	Error:
		return r;
	}

	GLuint GetOGLTextureIndex() {
		return m_textureIndex;
	}

	~OGLDepthbuffer() {
		if (m_textureIndex) {
			glDeleteTextures(1, &m_textureIndex);
			m_textureIndex = 0;
		}
	}

private:
	OpenGLImp *m_pParentImp;

	GLuint m_textureIndex;

	int m_width;
	int m_height;
	int m_sampleCount;
};

#endif // ! OGL_DEPTHBUFFER_H_
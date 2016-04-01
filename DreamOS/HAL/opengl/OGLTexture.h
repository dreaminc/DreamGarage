#ifndef OGL_TEXTURE_H_
#define OGL_TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/opengl/OGLTexture.h
// Base type for texture

#include "Primitives/texture.h"
#include "OpenGLCommon.h"

class OGLTexture : public texture {
public: 
	OGLTexture() :
		texture(),
		m_textureIndex(0)
	{
		// empty
	}

	OGLTexture(wchar_t *pszFilename) :
		texture(pszFilename),
		m_textureIndex(0)
	{
		// empty for now
	}

	~OGLTexture() {
		// empty stub
	}

	RESULT InitializeFromFile(const wchar_t *pszFileName) {
		RESULT r = R_PASS;



	Error:
		return r;
	}

private:
	GLuint m_textureIndex;
};

#endif // !OGL_TEXTURE_H_
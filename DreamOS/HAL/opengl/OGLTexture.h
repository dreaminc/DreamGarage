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
	OGLTexture(OpenGLImp *pParentImp) :
		texture(),
		m_textureIndex(0),
		m_pParentImp(pParentImp)
	{
		// empty
	}

	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilename) :
		texture(pszFilename),
		m_textureIndex(0),
		m_pParentImp(pParentImp)
	{
		RESULT r = OGLInitialize();
	}

	~OGLTexture() {
		// empty stub
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;
		
		CR(m_pParentImp->MakeCurrentContext());
		
		CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

		CR(m_pParentImp->glActiveTexture(GL_TEXTURE0));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));


	Error:
		return r;
	}

private:
	OpenGLImp *m_pParentImp;

	GLuint m_textureIndex;
};

#endif // !OGL_TEXTURE_H_
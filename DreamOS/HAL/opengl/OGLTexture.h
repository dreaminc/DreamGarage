#ifndef OGL_TEXTURE_H_
#define OGL_TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/opengl/OGLTexture.h
// Base type for texture

#include "Primitives/texture.h"

#include "OpenGLCommon.h"
#include "OpenGLImp.h"

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
		texture::~texture();
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;
		
		CR(m_pParentImp->MakeCurrentContext());
		
		CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

		CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine()));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));
		//CR(m_pParentImp->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height));
		//CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pImageBuffer));

		// Texture params TODO: Add controls for these 
		CR(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		CR(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CR(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
		CR(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

		CR(m_pParentImp->TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pImageBuffer));

		// TODO: We can technically release the image data here if we want

	Error:
		return r;
	}

	RESULT OGLActivateTexture() {
		RESULT r = R_PASS;

		CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine()));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));

	Error:
		return r;
	}

	GLenum GetGLTextureNumberDefine() {
		return (GLenum)((GL_TEXTURE0) + m_textureNumber);
	}

private:
	OpenGLImp *m_pParentImp;
	GLuint m_textureIndex;
};

#endif // !OGL_TEXTURE_H_
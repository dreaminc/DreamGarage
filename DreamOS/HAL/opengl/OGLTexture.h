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

	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilename, texture::TEXTURE_TYPE type) :
		texture(pszFilename, type),
		m_textureIndex(0),
		m_pParentImp(pParentImp)
	{
		RESULT r = OGLInitialize();
	}

	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight) :
		texture(pszFilenameFront, pszFilenameBack, pszFilenameTop, pszFilenameBottom, pszFilenameLeft, pszFilenameRight),
		m_textureIndex(0),
		m_pParentImp(pParentImp)
	{
		RESULT r = OGLInitialize();
	}

	~OGLTexture() {
		texture::~texture();
	}

	// These may ultimately be programatic
	GLint GetColorTextureNumber() {
		return 0;
	}

	GLint GetBumpTextureNumber() {
		return 1;
	}

	// TODO: Is this even needed?
	GLint GetCubeTextureNumber() {
		return 0;
	}

	GLint GetTextureNumber() {
		switch(GetTextureType()) {
			case texture::TEXTURE_TYPE::TEXTURE_COLOR: {
				return GetColorTextureNumber();
			} break; 

			case TEXTURE_TYPE::TEXTURE_BUMP: {
				return GetBumpTextureNumber();
			} break;
			
			// TODO: Do we need this?
			case TEXTURE_TYPE::TEXTURE_CUBE: {
				return 0;
			} break; 

			default:
			case TEXTURE_TYPE::TEXTURE_INVALID: {
				return -1;
			} break;
		}

		return -1;
	}

	RESULT OGLInitializeTexture(GLuint *pTextureIndex, GLenum textureNumber, GLenum textureTarget) {
		RESULT r = R_PASS;

		CR(m_pParentImp->MakeCurrentContext());
		CR(m_pParentImp->GenerateTextures(1, pTextureIndex));

		CR(m_pParentImp->glActiveTexture(textureNumber));
		CR(m_pParentImp->BindTexture(textureTarget, *pTextureIndex));
		//CR(m_pParentImp->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height));
		//CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pImageBuffer));

		// Texture params TODO: Add controls for these 
		CRM(m_pParentImp->TexParamteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
		CRM(m_pParentImp->TexParamteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");
		//CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
		//CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");

		// TODO: Pull deeper settings from texture object
		CR(m_pParentImp->TexImage2D(textureTarget, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pImageBuffer));

	Error:
		return r;
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;
		
		/*
		CR(m_pParentImp->MakeCurrentContext());
		
		CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

		CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine()));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));
		//CR(m_pParentImp->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height));
		//CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pImageBuffer));

		// Texture params TODO: Add controls for these 
		CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
		CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");
		//CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
		//CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");

		CR(m_pParentImp->TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pImageBuffer));

		// TODO: We can technically release the image data here if we want
		*/

		GLenum textureTarget = GL_TEXTURE_2D;

		if(GetTextureType() == texture::TEXTURE_TYPE::TEXTURE_CUBE)
			textureTarget = GL_TEXTURE_CUBE_MAP;

		CRM(OGLInitializeTexture(&m_textureIndex, GetGLTextureNumberDefine(), textureTarget), "Failed to initialize texture");

	Error:
		return r;
	}

	RESULT OGLActivateTexture() {
		RESULT r = R_PASS;

		if (GetTextureType() == TEXTURE_TYPE::TEXTURE_CUBE) {
			// TODO:
		}
		else {
			CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine()));
			CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));
		}

	Error:
		return r;
	}

	GLenum GetGLTextureNumberDefine() {
		return (GLenum)((GL_TEXTURE0) + GetTextureNumber());
	}

private:
	OpenGLImp *m_pParentImp;
	GLuint m_textureIndex;
};

#endif // !OGL_TEXTURE_H_
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
	const GLenum GLCubeMapEnums[NUM_CUBE_MAP_TEXTURES] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	};

public: 
	OGLTexture(OpenGLImp *pParentImp);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLuint textureID, int width, int height, int channels);

	// From Buffers
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n);

	// Load from File 
	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles);

	~OGLTexture();

	GLint GetColorTextureNumber();
	GLint GetBumpTextureNumber();
	GLint GetCubeTextureNumber();
	GLint GetTextureNumber();

	RESULT BindTexture(GLenum textureTarget);
	RESULT SetTextureParameter(GLenum textureTarget, GLenum paramName, GLint paramVal);
	
	// border will be zero more often than the buffer is 
	RESULT OGLInitialize(GLuint textureID = NULL);
	RESULT OGLInitializeTexture(GLenum textureTarget, GLint level, GLint internalformat, GLenum format, GLenum type, const void *pBuffer = nullptr, GLint border = 0);
	RESULT OGLInitializeTexture(GLuint *pTextureIndex, GLenum textureNumber, GLenum textureTarget);
	RESULT OGLInitializeCubeMap(GLuint *pTextureIndex, GLenum textureNumber);
	RESULT OGLInitializeTexture(GLuint textureIndex, GLenum textureNumber, GLenum textureTarget);
	RESULT OGLInitializeMultisample(int multisample = 4);
	
	RESULT OGLActivateTexture();

	RESULT Resize(int pxWidth, int pxHeight);

	GLenum GetGLTextureNumberDefine();

	GLuint GetOGLTextureIndex();

	RESULT Update(unsigned char* pBuffer, int width, int height, texture::PixelFormat pixelFormat) override;

private:
	GLenum GetOGLPixelFormat(texture::PixelFormat format, int channels = 3);

private:
	OpenGLImp *m_pParentImp = nullptr;

	GLuint m_textureIndex = 0;
};

#endif // !OGL_TEXTURE_H_
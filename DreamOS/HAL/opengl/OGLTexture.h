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

	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLenum textureTarget = GL_TEXTURE_2D);
	OGLTexture(const OGLTexture &pOGLTexture);
private: 

	// TODO: remove all these
	/*
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLuint textureID, int width, int height, int channels);

	// From Buffers
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n);
	OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n);

	// Load from File 
	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	OGLTexture(OpenGLImp *pParentImp, wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles);
	*/


public:
	~OGLTexture();

	RESULT Bind();
	RESULT SetTextureParameter(GLenum paramName, GLint paramVal);
	
	// border will be zero more often than the buffer is 
	RESULT OGLTexture::OGLInitialize(GLuint textureID);

	RESULT OGLInitializeTexture(GLenum textureTarget, GLint level, GLint internalformat, GLenum format, GLenum type, const void *pBuffer = nullptr, GLint border = 0);
	RESULT OGLInitializeCubeMap(GLuint *pTextureIndex, GLenum textureNumber);
	RESULT OGLInitializeMultisample(int multisample = 4);

	RESULT SetDefaultTextureParams();
	RESULT SetDefaultDepthTextureParams();
	RESULT SetDefaultCubeMapParams();
	
	RESULT OGLActivateTexture(int value);

	RESULT Resize(int pxWidth, int pxHeight);

	GLenum GetGLTextureNumberDefine(int value);

	GLuint GetOGLTextureIndex();

	RESULT AllocateGLTexture(size_t optOffset = 0);
	RESULT OGLTexture::AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType);

	RESULT Update(unsigned char* pBuffer, int width, int height, texture::PixelFormat pixelFormat) override;
	virtual RESULT UpdateDimensions(int width, int height) override;

	static GLenum GetOGLPixelFormat(texture::PixelFormat pixelFormat, int channels = 3);

	GLenum GetOGLTextureTarget() { return m_textureTarget; }
	GLint GetOGLInternalFormat() { return m_glInternalFormat; }
	GLenum GetOGLFormat() { return m_glFormat; }

public:
	virtual RESULT LoadImageFromTexture(int level, texture::PixelFormat pixelFormat) override;

private:
	GLenum GetOGLPixelFormat();
	RESULT CopyTextureBufferFromTexture(OGLTexture *pTexture);
	size_t GetTextureByteSize();

public:
	static OGLTexture *MakeTexture(const texture &srcTexture);
	static OGLTexture *MakeTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, int levels = 0, int samples = 1);
	static OGLTexture *MakeTextureWithFormat(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, GLint internalGLFormat = GL_DEPTH_COMPONENT24, GLenum glFormat = GL_DEPTH_COMPONENT, GLenum pixelDataType = GL_UNSIGNED_INT, int levels = 1, int samples = 0);
	static OGLTexture *MakeTextureFromAllocatedTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLenum textureTarget, GLuint textureID, int width, int height, int channels, int levels = 0, int samples = 1);
	static OGLTexture *MakeCubeMap(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels);
	static OGLTexture *MakeTextureFromPath(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, std::wstring wstrFilename);
	static OGLTexture *MakeTextureFromBuffer(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, texture::PixelFormat format, void *pBuffer, size_t pBuffer_n);
	static OGLTexture *MakeTextureFromFileBuffer(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, void *pBuffer, size_t pBuffer_n);

private:
	OpenGLImp *m_pParentImp = nullptr;

	GLuint m_textureIndex = 0;
	GLenum m_textureTarget = 0;

	// Note: this will work as long as the client is the one to allocate the texture
	// if not these values may be incorrect 
	GLint m_glInternalFormat; 
	GLenum m_glFormat;
	GLenum m_glPixelDataType;
};

#endif // !OGL_TEXTURE_H_
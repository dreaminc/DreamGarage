#ifndef OGL_TEXTURE_H_
#define OGL_TEXTURE_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLTexture.h

// Base type for texture

#include "OGLCommon.h"

#include "core/primitives/texture.h"

#define NUM_PACK_BUFFERS 2
#define NUM_UNPACK_BUFFERS 2

class OpenGLImp;

class OGLTexture : public texture {
public:
	OGLTexture(const OGLTexture &pOGLTexture);
	OGLTexture(OpenGLImp *pParentImp, texture::params *pTextureParams);
	OGLTexture(OpenGLImp *pParentImp, texture::type type, GLenum textureTarget = GL_TEXTURE_2D);


public:
	~OGLTexture();

	RESULT Bind();
	RESULT BindPixelUnpackBuffer(int index = 0);
	RESULT BindPixelPackBuffer(int index);

	RESULT SetTextureParameter(GLenum paramName, GLint paramVal);
	
	// border will be zero more often than the buffer is 
	RESULT OGLInitialize();
	RESULT OGLInitialize(GLuint textureID);

	RESULT OGLInitializeTexture(GLenum textureTarget, GLint level, GLint internalformat, GLenum format, GLenum type, const void *pBuffer = nullptr, GLint border = 0);
	
	RESULT OGLInitializeMultisample(int multisample = 4);

	RESULT SetDefaultTextureParams();
	RESULT SetDefaultDepthTextureParams();
	
	// TODO: remove?
	RESULT OGLActivateTexture(int value);

	RESULT Resize(int pxWidth, int pxHeight);

	GLenum GetGLTextureNumberDefine(int value);

	GLuint GetOGLTextureIndex();

	RESULT AllocateGLTexture(size_t optOffset = 0);
	RESULT AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType);

	virtual RESULT Update(unsigned char* pBuffer, int width, int height, PIXEL_FORMAT pixelFormat) override;
	virtual RESULT UpdateTextureFromBuffer(void *pBuffer, size_t pBuffer_n) override;
	virtual RESULT UpdateTextureRegionFromBuffer(void *pBuffer, int x, int y, int width, int height) override;
	virtual RESULT UpdateDimensions(int width, int height) override;

	GLenum GetOGLTextureTarget() { return m_glTextureTarget; }
	GLint GetOGLInternalFormat() { return m_glInternalFormat; }
	GLenum GetOGLFormat() { return m_glFormat; }

public:
	virtual RESULT LoadImageFromTexture(int level, PIXEL_FORMAT pixelFormat) override;
	virtual RESULT LoadBufferFromTexture(void *pBuffer, size_t pBuffer_n) override;

	virtual RESULT LoadFlippedBufferFromTexture(void *pBuffer, size_t pBuffer_n) override;

private:
	RESULT CopyTextureBufferFromTexture(OGLTexture *pTexture);
	size_t GetTextureByteSize();

public:
	static OGLTexture *MakeTexture(const texture &srcTexture);
	static OGLTexture *MakeTexture(OpenGLImp *pParentImp, texture::type type, int width, int height, int channels, int levels = 0, int samples = 1);
	static OGLTexture *MakeTextureWithFormat(OpenGLImp *pParentImp, texture::type type, int width, int height, int channels, GLint internalGLFormat = GL_DEPTH_COMPONENT24, GLenum glFormat = GL_DEPTH_COMPONENT, GLenum pixelDataType = GL_UNSIGNED_INT, int levels = 1, int samples = 0);
	static OGLTexture *MakeTextureFromAllocatedTexture(OpenGLImp *pParentImp, texture::type type, GLenum textureTarget, GLuint textureID, int width, int height, int channels, int levels = 0, int samples = 1);
	static OGLTexture *MakeTextureFromPath(OpenGLImp *pParentImp, texture::type type, std::wstring wstrFilename);
	static OGLTexture *MakeTextureFromBuffer(OpenGLImp *pParentImp, texture::type type, int width, int height, int channels, PIXEL_FORMAT format, void *pBuffer, size_t pBuffer_n);
	static OGLTexture *MakeTextureFromFileBuffer(OpenGLImp *pParentImp, texture::type type, void *pBuffer, size_t pBuffer_n);

private:
	OpenGLImp *m_pParentImp = nullptr;

	GLuint m_glFramebufferIndex = 0;
	GLuint m_glFlippedTextureIndex = 0;

	GLuint m_glTextureIndex = 0;
	GLenum m_glTextureTarget = 0;

	// Note: this will work as long as the client is the one to allocate the texture
	// if not these values may be incorrect 
	GLint m_glInternalFormat = GL_BGRA; 
	GLenum m_glFormat = GL_BGRA;
	GLenum m_glPixelDataType = GL_UNSIGNED_BYTE;

public:
	RESULT EnableOGLPBOUnpack();
	RESULT EnableOGLPBOPack();

	RESULT DeallocateOGLPBOPack();
	RESULT DeallocateOGLPBOUnpack();

	bool IsOGLPBOUnpackEnabled();
	bool IsOGLPBOPackEnabled();

private:
	// TODO: More?
	int m_unpackBufferIndex = 0;
	GLuint m_glPixelUnpackBufferIndex[NUM_UNPACK_BUFFERS] = { 0 };

	int m_packBufferIndex = 0;
	GLuint m_glPixelPackBufferIndex[NUM_PACK_BUFFERS] = { 0 };
};

#endif // !OGL_TEXTURE_H_
#ifndef OGL_CUBE_MAP_H_
#define OGL_CUBE_MAP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/opengl/OGLCubemap.h
// Base type for cube map

#include "Primitives/cubemap.h"

#include "OpenGLCommon.h"

class OpenGLImp;

class OGLCubemap : public cubemap {
public:
	const GLenum m_kGLCubeMapEnums[NUM_CUBE_MAP_TEXTURES] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	};

public:
	OGLCubemap(OpenGLImp *pParentImp);
	OGLCubemap(const OGLCubemap &oglCubemap);

	~OGLCubemap();

	//RESULT OGLInitializeCubeMap(GLuint *pTextureIndex, GLenum textureNumber);

	// Note: these are shared in OGLTexture - not a lot of code
	// so not sure if it's worth going through the virtual inheritance gauntlet yet
	RESULT Bind();
	RESULT SetTextureParameter(GLenum paramName, GLint paramVal);
	RESULT OGLInitialize(GLuint textureID);
	RESULT AllocateGLTexture();
	//RESULT AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType);

	RESULT SetDefaultCubeMapParams();

	//static OGLTexture *MakeCubeMap(OpenGLImp *pParentImp, texture::type type, int width, int height, int channels);
	static OGLCubemap *MakeCubemapFromName(OpenGLImp *pParentImp, const std::wstring &wstrCubemapNam);

public:
	OpenGLImp * m_pParentImp = nullptr;

	GLuint m_textureIndex = 0;
	GLenum m_textureTarget = GL_TEXTURE_CUBE_MAP;

	// Note: this will work as long as the client is the one to allocate the texture
	// if not these values may be incorrect 
	GLint m_glInternalFormat;
	GLenum m_glFormat;
	GLenum m_glPixelDataType;
};

#endif // !OGL_CUBE_MAP_H_
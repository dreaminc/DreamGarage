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
	static const GLenum kGLCubeMapEnums[NUM_CUBE_MAP_TEXTURES];

public:
	OGLCubemap(OpenGLImp *pParentImp);
	OGLCubemap(const OGLCubemap &oglCubemap);

	~OGLCubemap();

	// Note: these are shared in OGLTexture - not a lot of code
	// so not sure if it's worth going through the virtual inheritance gauntlet yet
	RESULT Bind();
	RESULT SetTextureParameter(GLenum paramName, GLint paramVal);
	RESULT OGLInitialize(GLuint textureID);
	
	RESULT AllocateGLCubemap();
	RESULT AllocateGLCubemap(size_t pxWidth, size_t pxHeight, int numChannels);

	GLenum GetOGLTextureTarget() { return m_glTextureTarget; }
	GLint GetOGLInternalFormat() { return m_glInternalFormat; }
	GLenum GetOGLFormat() { return m_glFormat; }
	GLuint GetOGLTextureIndex() { return m_glTextureIndex; }

	RESULT SetDefaultCubeMapParams();

	static OGLCubemap *MakeCubemap(OpenGLImp *pParentImp, int width, int height, int channels);
	static OGLCubemap *MakeCubemapFromName(OpenGLImp *pParentImp, const std::wstring &wstrCubemapNam);

	static GLenum GetGLCubeMapEnums(int lookup) {
		return (kGLCubeMapEnums[lookup]);
	}

public:
	OpenGLImp *m_pParentImp = nullptr;

	GLuint m_glTextureIndex = 0;
	GLenum m_glTextureTarget = GL_TEXTURE_CUBE_MAP;

	// Note: this will work as long as the client is the one to allocate the texture
	// if not these values may be incorrect 
	GLint m_glInternalFormat;
	GLenum m_glFormat;
	GLenum m_glPixelDataType;
};

#endif // !OGL_CUBE_MAP_H_
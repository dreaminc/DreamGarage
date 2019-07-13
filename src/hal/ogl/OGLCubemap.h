#ifndef OGL_CUBE_MAP_H_
#define OGL_CUBE_MAP_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLCubemap.h

// Base type for cube map

#include "OGLCommon.h"

#include "core/primitives/cubemap.h"

class OGLImp;

class OGLCubemap : public cubemap {
public:
	static const GLenum kGLCubeMapEnums[NUM_CUBE_MAP_TEXTURES];

public:
	OGLCubemap(OGLImp *pParentImp);
	OGLCubemap(const OGLCubemap &oglCubemap);

	~OGLCubemap();

	// Note: these are shared in OGLTexture - not a lot of code
	// so not sure if it's worth going through the virtual inheritance gauntlet yet
	RESULT Bind();
	RESULT SetTextureParameter(GLenum paramName, GLint paramVal);
	RESULT OGLInitialize(GLuint textureID);
	
	RESULT AllocateGLCubemap();
	RESULT AllocateGLCubemap(size_t pxWidth, size_t pxHeight, int numChannels);

	// TODO: this is getting messed up somewhere
	//GLenum GetOGLTextureTarget() { return m_glTextureTarget; }
	GLenum GetOGLTextureTarget() { return GL_TEXTURE_CUBE_MAP; }
	GLint GetOGLInternalFormat() { return m_glInternalFormat; }
	GLenum GetOGLFormat() { return m_glFormat; }
	GLuint GetOGLTextureIndex() { return m_glTextureIndex; }

	RESULT SetDefaultCubeMapParams();

	static OGLCubemap *MakeCubemap(OGLImp *pParentImp, int width, int height, int channels);
	static OGLCubemap *MakeCubemapFromName(OGLImp *pParentImp, const std::wstring &wstrCubemapNam);

	static GLenum GetGLCubeMapEnums(int lookup) {
		return (kGLCubeMapEnums[lookup]);
	}

public:
	OGLImp *m_pParentImp = nullptr;

	GLuint m_glTextureIndex = 0;
	GLenum m_glTextureTarget = GL_TEXTURE_CUBE_MAP;

	// Note: this will work as long as the client is the one to allocate the texture
	// if not these values may be incorrect 
	GLint m_glInternalFormat;
	GLenum m_glFormat;
	GLenum m_glPixelDataType;
};

#endif // !OGL_CUBE_MAP_H_
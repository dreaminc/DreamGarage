#include "OGLCubemap.h"

#include "Primitives/image/image.h"
#include "Primitives/image/ImageFactory.h"

#include "OpenGLImp.h"

OGLCubemap::OGLCubemap(OpenGLImp *pParentImp) :
	cubemap(),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// This constructor should be used when deeper configuration is sought 
}

OGLCubemap::OGLCubemap(const OGLCubemap &pOGLCubemap) :
	cubemap((const cubemap&)(pOGLCubemap)),
	m_textureIndex(0),
	m_pParentImp(pOGLCubemap.m_pParentImp),
	m_glFormat(pOGLCubemap.m_glFormat),
	m_glInternalFormat(pOGLCubemap.m_glInternalFormat),
	m_glPixelDataType(pOGLCubemap.m_glPixelDataType)
{
	// empty
	// NOTE: this will not copy buffers on either GPU or CPU side
}

OGLCubemap::~OGLCubemap() {
	cubemap::~cubemap();

	if (m_textureIndex != 0) {
		m_pParentImp->DeleteTextures(1, &m_textureIndex);
		m_textureIndex = 0;
	}
}

RESULT OGLCubemap::Bind() {
	return m_pParentImp->BindTexture(m_textureTarget, m_textureIndex);
}

RESULT OGLCubemap::SetTextureParameter(GLenum paramName, GLint paramVal) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	CR(Bind());
	CR(m_pParentImp->TexParameteri(m_textureTarget, paramName, paramVal));

Error:
	return r;
}

RESULT OGLCubemap::OGLInitialize(GLuint textureID) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	if (textureID == NULL) {
		CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));
	}
	else {
		m_textureIndex = textureID;
	}

Error:
	return r;
}

// TODO: Border?
RESULT OGLCubemap::AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType) {
	RESULT r = R_PASS;

	CR(Bind());

	// TODO: Pull deeper settings from texture object
	CR(m_pParentImp->TexImage2D(m_textureTarget, 0, internalGLFormat, m_width, m_height, 0, glFormat, pixelDataType, pImageBuffer));

	m_glInternalFormat = internalGLFormat;
	m_glFormat = glFormat;
	m_glPixelDataType = pixelDataType;

Error:
	return r;
}

RESULT OGLCubemap::AllocateGLTexture(size_t optOffset) {
	RESULT r = R_PASS;

	GLenum glFormat = GetOpenGLPixelFormat(m_pixelFormat, m_channels);
	GLint internalGLFormat = GetOpenGLPixelFormat(PIXEL_FORMAT::Unspecified, m_channels);
	
	unsigned char *pImageBuffer = nullptr;

	if (m_pImage != nullptr) {
		pImageBuffer = m_pImage->GetImageBuffer() + (optOffset);
	}

	CR(AllocateGLTexture(pImageBuffer, internalGLFormat, glFormat, GL_UNSIGNED_BYTE));

Error:
	return r;
}


RESULT OGLCubemap::OGLInitializeCubeMap(GLuint *pTextureIndex, GLenum textureNumber) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, pTextureIndex));

	unsigned char *pImageBuffer = nullptr;

	if (m_pImage != nullptr) {
		m_pImage->GetImageBuffer();
	}

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		//size_t sizeSide = m_width * m_height * sizeof(unsigned char);
		size_t sizeSide = GetTextureSize();
		unsigned char *ptrOffset = pImageBuffer + (i * (sizeSide));

		CR(m_pParentImp->TexImage2D(GLCubeMapEnums[i], 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, ptrOffset));

		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
	}

	// TODO: Delete the image data here?

Error:
	return r;
}


RESULT OGLCubemap::SetDefaultCubeMapParams() {
	RESULT r = R_PASS;
	
	CRM(SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");
	
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");

Error:
	return r;
}

// TODO: Move to OGLCubeMap
//OGLTexture* OGLTexture::MakeCubeMap(OpenGLImp *pParentImp, texture::type type, int width, int height, int channels) {
//	RESULT r = R_PASS;
//	
//	OGLTexture *pTexture = nullptr;
//
//	pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_CUBE_MAP);
//	CN(pTexture);
//
//	GLenum textureTarget = GL_TEXTURE_CUBE_MAP;

//for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
//	size_t pCubeMapSideOffset = pTexture->GetTextureSize();
//	CR(pTexture->AllocateGLTexture(pCubeMapSideOffset));
//
//	// TODO: Is this needed here?  I think it can be out of the for loop
//	// TODO: Rename or remove this / specialize more
//	CR(pTexture->SetDefaultCubeMapParams());
//}

//
//Error:
//	return pTexture;
//}

OGLCubemap* OGLCubemap::MakeCubemapFromName(OpenGLImp *pParentImp, const std::wstring &wstrCubemapName) {
	RESULT r = R_PASS;

	OGLCubemap *pCubemap = nullptr;

	pCubemap = new OGLCubemap(pParentImp);
	CN(pCubemap);
	
	CR(pCubemap->LoadCubeMapByName(wstrCubemapName.c_str()));

	CR(pCubemap->OGLInitialize(NULL));
	CR(pCubemap->AllocateGLTexture());
	
	CR(pCubemap->SetDefaultCubeMapParams());

Error:
	return pCubemap;
}
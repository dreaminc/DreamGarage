#include "OGLCubemap.h"

#include "Primitives/image/image.h"
#include "Primitives/image/ImageFactory.h"

#include "OpenGLImp.h"

const GLenum OGLCubemap::kGLCubeMapEnums[NUM_CUBE_MAP_TEXTURES] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z
};

OGLCubemap::OGLCubemap(OpenGLImp *pParentImp) :
	cubemap(),
	m_glTextureIndex(0),
	m_pParentImp(pParentImp)
{
	// This constructor should be used when deeper configuration is sought 
}

OGLCubemap::OGLCubemap(const OGLCubemap &pOGLCubemap) :
	cubemap((const cubemap&)(pOGLCubemap)),
	m_glTextureIndex(0),
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

	if (m_glTextureIndex != 0) {
		m_pParentImp->DeleteTextures(1, &m_glTextureIndex);
		m_glTextureIndex = 0;
	}
}

RESULT OGLCubemap::Bind() {
	return m_pParentImp->BindTexture(m_glTextureTarget, m_glTextureIndex);
}

RESULT OGLCubemap::SetTextureParameter(GLenum paramName, GLint paramVal) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	CR(Bind());
	CR(m_pParentImp->TexParameteri(m_glTextureTarget, paramName, paramVal));

Error:
	return r;
}

RESULT OGLCubemap::OGLInitialize(GLuint textureID) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	if (textureID == NULL) {
		CR(m_pParentImp->GenerateTextures(1, &m_glTextureIndex));
	}
	else {
		m_glTextureIndex = textureID;
	}

Error:
	return r;
}

RESULT OGLCubemap::AllocateGLCubemap() {
	RESULT r = R_PASS;

	//CN(m_pImageBuffer);

	//GLenum glFormat = GetOpenGLPixelFormat(m_pixelFormat, m_channels);
	GLenum glFormat = GL_BGRA;

	GLint internalGLFormat = GetOpenGLPixelFormat(PIXEL_FORMAT::Unspecified, m_channels);	
	//GLint internalGLFormat = GL_RGB;

	//CR(AllocateGLTexture(pImageBuffer, internalGLFormat, glFormat, GL_UNSIGNED_BYTE));

	m_glInternalFormat = internalGLFormat;
	m_glFormat = glFormat;
	m_glPixelDataType = GL_UNSIGNED_BYTE;

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {

		//size_t sizeSide = m_pCubemapImages[i]->GetImageBufferSize();

		//size_t sizeSide = m_width * m_height * m_channels * sizeof(uint8_t);
		uint8_t *ptrOffset = m_pCubemapImages[i]->GetImageBuffer();

		CN(ptrOffset);

		CR(m_pParentImp->TexImage2D(kGLCubeMapEnums[i], 
									0,				// level
									m_glInternalFormat, 
									m_width, 
									m_height, 
									0,				// border
									m_glFormat, 
									m_glPixelDataType, 
									ptrOffset));

		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
	}

	// Release the images 
	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		if(m_pCubemapImages[i] != nullptr) {
			delete m_pCubemapImages[i];
			m_pCubemapImages[i] = nullptr;
		}
	}

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

RESULT OGLCubemap::AllocateGLCubemap(size_t pxWidth, size_t pxHeight, int numChannels) {
	RESULT r = R_PASS;

	m_width = (int)pxWidth;
	m_height = (int)pxHeight;

	//m_pParentImp->glActiveTexture(GL_TEXTURE1);

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		// Allocate the texture GPU side
		CR(m_pParentImp->TexImage2D(kGLCubeMapEnums[i],
									0,		// level
									GL_RGBA, 
									(GLsizei)m_width,
									(GLsizei)m_height,
									0,		// border
									GL_RGBA, 
									//GL_UNSIGNED_BYTE, 
									GL_FLOAT,
									nullptr));

		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
		CRM(m_pParentImp->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");

		//glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

Error:
	return r;
}

OGLCubemap* OGLCubemap::MakeCubemap(OpenGLImp *pParentImp, int width, int height, int channels) {
	RESULT r = R_PASS;
	
	OGLCubemap *pCubemap = nullptr;

	pCubemap = new OGLCubemap(pParentImp);
	CN(pCubemap);

	CR(pCubemap->OGLInitialize(NULL));
	CR(pCubemap->Bind());

	CR(pCubemap->AllocateGLCubemap(width, height, channels));
		
	CR(pCubemap->SetDefaultCubeMapParams());

Error:
	return pCubemap;
}

OGLCubemap* OGLCubemap::MakeCubemapFromName(OpenGLImp *pParentImp, const std::wstring &wstrCubemapName) {
	RESULT r = R_PASS;

	OGLCubemap *pCubemap = nullptr;

	pCubemap = new OGLCubemap(pParentImp);
	CN(pCubemap);
	
	CR(pCubemap->LoadCubeMapByName(wstrCubemapName.c_str()));

	CR(pCubemap->OGLInitialize(NULL));
	CR(pCubemap->AllocateGLCubemap());
	
	CR(pCubemap->SetDefaultCubeMapParams());

Error:
	return pCubemap;
}
#include "OGLTexture.h"

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLenum textureTarget) :
	texture(type),
	m_textureIndex(0),
	m_textureTarget(textureTarget),
	m_pParentImp(pParentImp)
{
	// This constructor should be used when deeper configuration is sought 
}

/*
OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n) :
	texture(type, width, height, format, channels, pBuffer, pBuffer_n),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n) :
	texture(type, width, height, channels, pBuffer, pBuffer_n),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

// Load from File Buffer (file loaded into buffer)
OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n) :
	texture(type, pBuffer, pBuffer_n),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels) :
	texture(type, width, height, channels),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLuint textureID, int width, int height, int channels) :
	texture(type, width, height, channels),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilename, texture::TEXTURE_TYPE type) :
	texture(pszFilename, type),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles) :
	texture(pszName, vstrCubeMapFiles),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}
*/

OGLTexture::~OGLTexture() {
	texture::~texture();

	if (m_textureIndex != 0) {
		m_pParentImp->DeleteTextures(1, &m_textureIndex);
		m_textureIndex = 0;
	}
}

RESULT OGLTexture::Bind() {
	return m_pParentImp->BindTexture(m_textureTarget, m_textureIndex);
}

RESULT OGLTexture::SetTextureParameter(GLenum paramName, GLint paramVal) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	CR(Bind());
	CR(m_pParentImp->TexParameteri(m_textureTarget, paramName, paramVal));

Error:
	return r;
}

RESULT OGLTexture::AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType) {
	RESULT r = R_PASS;
	
	CR(Bind());

	// TODO: Pull deeper settings from texture object
	CR(m_pParentImp->TexImage2D(m_textureTarget, 0, internalGLFormat, m_width, m_height, 0, glFormat, pixelDataType, pImageBuffer));

Error:
	return r;
}

RESULT OGLTexture::AllocateGLTexture(size_t optOffset) {
	RESULT r = R_PASS;

	GLenum glFormat = GetOGLPixelFormat();
	GLint internalGLFormat = static_cast<GLint>(glFormat);

	unsigned char *pImageBuffer = nullptr;

	if (m_pImageBuffer != nullptr) {
		pImageBuffer = m_pImageBuffer + (optOffset);
	}

	CR(AllocateGLTexture(pImageBuffer, internalGLFormat, glFormat, GL_UNSIGNED_BYTE));

Error:
	return r;
}

RESULT OGLTexture::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CBR(((pxWidth != m_width) || (pxHeight != m_height)), R_SKIPPED);

	m_width = pxWidth;
	m_height = pxHeight;

	CR(AllocateGLTexture());

Error:
	return r;
}

OGLTexture* OGLTexture::MakeTextureWithFormat(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type,
											  int width, int height, int channels, 
											  GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType, 
											  int levels, int samples) 
{
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	// TODO: Get rid of other texture targets and do 2d, cube, rectangle (others?)
	if (type == texture::TEXTURE_TYPE::TEXTURE_RECTANGLE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_RECTANGLE);
		CN(pTexture);
	}
	else {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
		CN(pTexture);
	}

	CR(pTexture->OGLInitialize(NULL));
	CR(pTexture->SetParams(width, height, channels, samples, levels));

	CR(pTexture->AllocateGLTexture(nullptr, internalGLFormat, glFormat, pixelDataType));

	// TODO: Rename or remove this / specialize more
	CR(pTexture->SetDefaultDepthTextureParams());

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, int levels, int samples) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	// TODO: Get rid of other texture targets and do 2d, cube, rectangle (others?)
	if (type == texture::TEXTURE_TYPE::TEXTURE_RECTANGLE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_RECTANGLE);
		CN(pTexture);
	}
	else {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
		CN(pTexture);
	}

	CR(pTexture->OGLInitialize(NULL));
	CR(pTexture->SetParams(width, height, channels, samples, levels));
	CR(pTexture->AllocateGLTexture());

	// TODO: Rename or remove this / specialize more
	CR(pTexture->SetDefaultTextureParams());

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromAllocatedTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLenum textureTarget, GLuint textureID, int width, int height, int channels, int levels, int samples) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
	CN(pTexture);

	CR(pTexture->OGLInitialize(textureID));
	CR(pTexture->SetParams(width, height, channels, samples, levels));

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeCubeMap(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels) {
	RESULT r = R_PASS;
	
	OGLTexture *pTexture = nullptr;

	pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_CUBE_MAP);
	CN(pTexture);

	GLenum textureTarget = GL_TEXTURE_CUBE_MAP;

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		size_t pCubeMapSideOffset = pTexture->GetTextureSize();
		CR(pTexture->AllocateGLTexture(pCubeMapSideOffset));
		
		// TODO: Is this needed here?  I think it can be out of the for loop
		// TODO: Rename or remove this / specialize more
		CR(pTexture->SetDefaultCubeMapParams());
	}

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromPath(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, std::wstring wstrFilename) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_CUBE_MAP);
		CN(pTexture);

		CR(pTexture->LoadCubeMapByName(wstrFilename.c_str()));
	}
	else if (type == texture::TEXTURE_TYPE::TEXTURE_RECTANGLE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_RECTANGLE);
		CN(pTexture);

		CR(pTexture->LoadTextureFromFile(wstrFilename.c_str()));
	}
	else {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
		CN(pTexture);

		CR(pTexture->LoadTextureFromFile(wstrFilename.c_str()));
	}

	CR(pTexture->OGLInitialize(NULL));
	CR(pTexture->AllocateGLTexture());

	// TODO: Rename or remove this / specialize more
	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		CR(pTexture->SetDefaultCubeMapParams());
	}
	else {
		CR(pTexture->SetDefaultTextureParams());
	}

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromBuffer(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, texture::PixelFormat format, void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_CUBE_MAP);
	}
	else {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
	}

	CN(pTexture);

	CR(pTexture->OGLInitialize(NULL));
	CR(pTexture->SetParams(width, height, channels));
	CR(pTexture->SetFormat(format));

	CR(pTexture->CopyTextureBuffer(width, height, channels, pBuffer, (int)(pBuffer_n)));
	CR(pTexture->AllocateGLTexture());

	// TODO: Rename or remove this / specialize more
	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		CR(pTexture->SetDefaultCubeMapParams());
	}
	else {
		CR(pTexture->SetDefaultTextureParams());
	}

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromFileBuffer(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_CUBE_MAP);
	}
	else {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
	}

	CN(pTexture);

	CR(pTexture->OGLInitialize(NULL));

	CR(pTexture->LoadTextureFromFileBuffer((uint8_t*)pBuffer, pBuffer_n));
	CR(pTexture->AllocateGLTexture());

	// TODO: Rename or remove this / specialize more
	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		CR(pTexture->SetDefaultCubeMapParams());
	}
	else {
		CR(pTexture->SetDefaultTextureParams());
	}

Error:
	return pTexture;
}

RESULT OGLTexture::OGLInitializeCubeMap(GLuint *pTextureIndex, GLenum textureNumber) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, pTextureIndex));

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		//size_t sizeSide = m_width * m_height * sizeof(unsigned char);
		size_t sizeSide = GetTextureSize();
		unsigned char *ptrOffset = m_pImageBuffer + (i * (sizeSide));
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

RESULT OGLTexture::SetDefaultDepthTextureParams() {
	RESULT r = R_PASS;

	CRM(SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

	CRM(SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");

Error:
	return r;
}

RESULT OGLTexture::SetDefaultTextureParams() {
	RESULT r = R_PASS;

	CRM(SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST), "Failed to set GL_TEXTURE_MIN_FILTER");

	CRM(SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");

Error:
	return r;
}

RESULT OGLTexture::SetDefaultCubeMapParams() {
	RESULT r = R_PASS;

	CRM(SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

	CRM(SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");

Error:
	return r;
}

RESULT OGLTexture::OGLInitialize(GLuint textureID) {
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

// border will be zero more often than the buffer is 
RESULT OGLTexture::OGLInitializeTexture(GLenum textureTarget, GLint level, GLint internalformat, GLenum format, GLenum type, const void *pBuffer, GLint border) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

	CR(m_pParentImp->BindTexture(textureTarget, m_textureIndex));

	CR(m_pParentImp->TexImage2D(textureTarget, level, internalformat, m_width, m_height, border, format, type, pBuffer));

Error:
	return r;
}

// TODO: Move up 
RESULT OGLTexture::OGLInitializeMultisample(int multisample) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

	CB((m_textureTarget == GL_TEXTURE_2D_MULTISAMPLE));

	CR(m_pParentImp->BindTexture(m_textureTarget, m_textureIndex));
	CR(m_pParentImp->glTexImage2DMultisample(m_textureTarget, multisample, GL_RGBA8, m_width, m_height, true));

Error:
	return r;
}

RESULT OGLTexture::OGLActivateTexture(int value) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine(value)));
	CR(m_pParentImp->BindTexture(m_textureTarget, m_textureIndex));

Error:
	return r;
}

GLenum OGLTexture::GetGLTextureNumberDefine(int value) {
	return (GLenum)((GL_TEXTURE0) + value);
}

GLuint OGLTexture::GetOGLTextureIndex() {
	return m_textureIndex;
}

RESULT OGLTexture::Update(unsigned char* pBuffer, int width, int height, texture::PixelFormat pixelFormat) {
	RESULT r = R_PASS;

	CR(Bind());

	CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GetOGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, pBuffer));

Error:
	return r;
}

GLenum OGLTexture::GetOGLPixelFormat() {
	return GetOGLPixelFormat(m_format, m_channels);
}

GLenum OGLTexture::GetOGLPixelFormat(texture::PixelFormat pixelFormat, int channels) {
	switch (pixelFormat) {
		case texture::PixelFormat::Unspecified: {
			if (channels == 3)
				return GL_RGB; 
			else 
				return GL_RGBA;
		} break;

		case texture::PixelFormat::RGB: {
			return GL_RGB; 
		} break;

		case texture::PixelFormat::RGBA: {
			return GL_RGBA;
		} break;

		case texture::PixelFormat::BGR: {
			return GL_BGR;
		} break;

		case texture::PixelFormat::BGRA: {
			return GL_BGRA;
		} break;
	}

	return 0; // no format for unknown
}
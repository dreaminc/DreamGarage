#include "OGLTexture.h"

OGLTexture::OGLTexture(OpenGLImp *pParentImp) :
	texture(),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// empty
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type) :
	texture(type),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	// This constructor should be used when deeper configuration is sought 
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n) :
	texture(type, width, height, format, channels, pBuffer, pBuffer_n),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize();
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n) :
	texture(type, width, height, channels, pBuffer, pBuffer_n),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize();
}

// Load from File Buffer (file loaded into buffer)
OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n) :
	texture(type, pBuffer, pBuffer_n),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize();
	CR(r);

	// Success:
	Validate();
	return;

Error:
	InvalidateOverride();
	return;
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels) :
	texture(type, width, height, channels),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize();
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLuint textureID, int width, int height, int channels) :
	texture(type, width, height, channels),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize(textureID);
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, wchar_t *pszFilename, texture::TEXTURE_TYPE type) :
	texture(pszFilename, type),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize();
}

OGLTexture::OGLTexture(OpenGLImp *pParentImp, wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles) :
	texture(pszName, vstrCubeMapFiles),
	m_textureIndex(0),
	m_pParentImp(pParentImp)
{
	RESULT r = OGLInitialize();
}

OGLTexture::~OGLTexture() {
	texture::~texture();
}

// These may ultimately be programatic
GLint OGLTexture::GetColorTextureNumber() {
	return 0;
}

GLint OGLTexture::GetBumpTextureNumber() {
	return 1;
}

// TODO: Is this even needed?
GLint OGLTexture::GetCubeTextureNumber() {
	return 2;
}

// Get rid of these - push into programs
GLint OGLTexture::GetTextureNumber() {
	switch (GetTextureType()) {
	case texture::TEXTURE_TYPE::TEXTURE_COLOR: {
		return GetColorTextureNumber();
	} break;

	case TEXTURE_TYPE::TEXTURE_BUMP: {
		return GetBumpTextureNumber();
	} break;

		// TODO: Do we need this?
	case TEXTURE_TYPE::TEXTURE_CUBE: {
		return GetCubeTextureNumber();
	} break;

	default:
	case TEXTURE_TYPE::TEXTURE_INVALID: {
		return -1;
	} break;
	}

	return -1;
}

RESULT OGLTexture::BindTexture(GLenum textureTarget) {
	return m_pParentImp->BindTexture(textureTarget, m_textureIndex);
}

RESULT OGLTexture::SetTextureParameter(GLenum textureTarget, GLenum paramName, GLint paramVal) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(BindTexture(textureTarget));
	CR(m_pParentImp->TexParameteri(textureTarget, paramName, paramVal));

Error:
	return r;
}

RESULT OGLTexture::OGLInitialize(GLuint textureID = NULL) {
	RESULT r = R_PASS;

	if (GetTextureType() == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		CRM(OGLInitializeCubeMap(&m_textureIndex, GetGLTextureNumberDefine()), "Failed to initialize texture");
	}
	else {
		if (textureID == NULL) {
			CRM(OGLInitializeTexture(&m_textureIndex, GetGLTextureNumberDefine(), GL_TEXTURE_2D), "Failed to initialize texture");
		}
		else {
			CRM(OGLInitializeTexture(textureID, GetGLTextureNumberDefine(), GL_TEXTURE_2D), "Failed to initialize texture");
		}
	}

Error:
	return r;
}

RESULT OGLTexture::OGLInitializeTexture(GLuint textureIndex, GLenum textureNumber, GLenum textureTarget) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	m_textureIndex = textureIndex;
	CR(m_pParentImp->glActiveTexture(textureNumber));

	// Texture params TODO: Add controls for these 
	CRM(m_pParentImp->TexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(m_pParentImp->TexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");
	CRM(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");
	CRM(m_pParentImp->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");

Error:
	return r;
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

RESULT OGLTexture::OGLInitializeMultisample(int multisample = 4) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

	//CR(m_pParentImp->glActiveTexture(textureNumber));
	CR(m_pParentImp->BindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureIndex));
	CR(m_pParentImp->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisample, GL_RGBA8, m_width, m_height, true));

Error:
	return r;
}

// border will be zero more often than the buffer is 
RESULT OGLTexture::OGLInitializeTexture(GLenum textureTarget, GLint level, GLint internalformat, GLenum format, GLenum type, const void *pBuffer = nullptr, GLint border = 0) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, &m_textureIndex));

	CR(m_pParentImp->BindTexture(textureTarget, m_textureIndex));

	CR(m_pParentImp->TexImage2D(textureTarget, level, internalformat, m_width, m_height, border, format, type, pBuffer));

Error:
	return r;
}

RESULT OGLTexture::OGLInitializeTexture(GLuint *pTextureIndex, GLenum textureNumber, GLenum textureTarget) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, pTextureIndex));

	//CR(m_pParentImp->glActiveTexture(textureNumber));
	CR(m_pParentImp->BindTexture(textureTarget, *pTextureIndex));

	GLenum	format = GetOGLPixelFormat(m_format, m_channels);
	GLint	internalFormat = static_cast<GLint>(format);

	// TODO: Pull deeper settings from texture object
	if (m_pImageBuffer != NULL) {
		CR(m_pParentImp->TexImage2D(textureTarget, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, m_pImageBuffer));
	}
	else {
		CR(m_pParentImp->TexImage2D(textureTarget, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, NULL));
	}

	// Texture params TODO: Add controls for these 
	CRM(m_pParentImp->TexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(m_pParentImp->TexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST), "Failed to set GL_TEXTURE_MIN_FILTER");
	//CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_S");
	//CRM(m_pParentImp->TexParamteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set GL_TEXTURE_WRAP_T");

	// TODO: Delete the image data here?

Error:
	return r;
}

RESULT OGLTexture::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

Error:
	return r;
}

RESULT OGLTexture::OGLActivateTexture() {
	RESULT r = R_PASS;

	if (GetTextureType() == TEXTURE_TYPE::TEXTURE_CUBE) {
		CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine()));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_CUBE_MAP, m_textureIndex));
	}
	else {
		CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine()));
		CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));
	}

Error:
	return r;
}

GLenum OGLTexture::GetGLTextureNumberDefine() {
	return (GLenum)((GL_TEXTURE0) + GetTextureNumber());
}

GLuint OGLTexture::GetOGLTextureIndex() {
	return m_textureIndex;
}

RESULT OGLTexture::Update(unsigned char* pBuffer, int width, int height, texture::PixelFormat pixelFormat) override {
	RESULT r = R_PASS;

	//CR(m_pParentImp->MakeCurrentContext());

	CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_textureIndex));
	CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GetOGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, pBuffer));

Error:
	return r;
}

GLenum OGLTexture::GetOGLPixelFormat(texture::PixelFormat format, int channels = 3) {
	switch (format) {
	case texture::PixelFormat::Unspecified: return (channels == 3) ? GL_RGB : GL_RGBA;
	case texture::PixelFormat::RGB:			return GL_RGB;
	case texture::PixelFormat::RGBA:		return GL_RGBA;
	case texture::PixelFormat::BGR:			return GL_BGR;
	case texture::PixelFormat::BGRA:		return GL_BGRA;
	}

	return 0; // no format for unknown
}
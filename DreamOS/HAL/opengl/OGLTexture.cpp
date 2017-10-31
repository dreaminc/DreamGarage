#include "OGLTexture.h"

#include "Primitives/image/image.h"
#include "Primitives/image/ImageFactory.h"

OGLTexture::OGLTexture(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, GLenum textureTarget) :
	texture(type),
	m_textureIndex(0),
	m_textureTarget(textureTarget),
	m_pParentImp(pParentImp)
{
	// This constructor should be used when deeper configuration is sought 
}

OGLTexture::OGLTexture(const OGLTexture &pOGLTexture) :
	texture((const texture&)(pOGLTexture)),
	m_textureIndex(0),
	m_textureTarget(pOGLTexture.m_textureTarget),
	m_pParentImp(pOGLTexture.m_pParentImp),
	m_glFormat(pOGLTexture.m_glFormat),
	m_glInternalFormat(pOGLTexture.m_glInternalFormat),
	m_glPixelDataType(pOGLTexture.m_glPixelDataType)
{
	// empty
	// NOTE: this will not copy buffers on either GPU or CPU side
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

// TODO: Border?
RESULT OGLTexture::AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType) {
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

RESULT OGLTexture::AllocateGLTexture(size_t optOffset) {
	RESULT r = R_PASS;

	GLenum glFormat = GetOGLPixelFormat();

	//GLint internalGLFormat = static_cast<GLint>(glFormat);
	GLint internalGLFormat = GetOGLPixelFormat(PIXEL_FORMAT::Unspecified, m_channels);

	unsigned char *pImageBuffer = nullptr;

	if (m_pImage != nullptr) {
		pImageBuffer = m_pImage->GetImageBuffer() + (optOffset);
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

size_t OGLTexture::GetTextureByteSize() {
	size_t sizeVal = 0;

	switch (m_glPixelDataType) {
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV: {
			sizeVal = sizeof(char);
		} break;
		
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV: {
			sizeVal = sizeof(short);
		} break;

		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_24_8:
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV: {
			sizeVal = sizeof(int);
		} break;
		
		case GL_HALF_FLOAT:
		case GL_FLOAT:
		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: {
			sizeVal = sizeof(float);
		} break;
	}

	sizeVal = sizeVal * m_width * m_channels * m_height;

	return sizeVal;
}

// TODO: Border?
RESULT OGLTexture::CopyTextureBufferFromTexture(OGLTexture *pTexture) {
	RESULT r = R_PASS;

	void *pTextureBuffer = nullptr;
	size_t pTextureBuffer_n = GetTextureByteSize();

	pTextureBuffer = (unsigned char*)malloc(pTextureBuffer_n);
	CN(pTextureBuffer);
	memset(pTextureBuffer, 0, pTextureBuffer_n);

	CR(pTexture->Bind());
	glGetTexImage(pTexture->GetOGLTextureTarget(), 0, pTexture->GetOGLFormat(), pTexture->m_glPixelDataType, pTextureBuffer);
	CRM(m_pParentImp->CheckGLError(), "glGetTexImage failed");

	// Bind Texture
	CR(Bind());
	CR(m_pParentImp->TexImage2D(m_textureTarget, 0, m_glInternalFormat, m_width, m_height, 0, m_glFormat, m_glPixelDataType, pTextureBuffer));

	CRM(m_pParentImp->CheckGLError(), "glCopyTexImage2D failed");

Error:
	if (pTextureBuffer != nullptr) {
		delete pTextureBuffer;
		pTextureBuffer = nullptr;
	}

	return r;
}

OGLTexture* OGLTexture::MakeTexture(const texture &srcTexture) {
	RESULT r = R_PASS;

	OGLTexture *pDestTexture = nullptr;
	OGLTexture *pSrcTexture = nullptr;

	pSrcTexture = dynamic_cast<OGLTexture*>((texture*)(&srcTexture));
	CNM(pSrcTexture, "Source texture not of type OGLTexture");

	pDestTexture = new OGLTexture(*pSrcTexture);
	CN(pDestTexture);

	CR(pDestTexture->OGLInitialize(NULL));
	//CR(pDestTexture->AllocateGLTexture());

	CR(pDestTexture->CopyTextureBufferFromTexture(pSrcTexture));

	// TODO: Rename or remove this / specialize more
	CR(pDestTexture->SetDefaultTextureParams());

	return pDestTexture;

Error:

	if (pDestTexture != nullptr) {
		delete pDestTexture;
		pDestTexture = nullptr;
	}

	return nullptr;
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

OGLTexture* OGLTexture::MakeTextureFromBuffer(OpenGLImp *pParentImp, texture::TEXTURE_TYPE type, int width, int height, int channels, PIXEL_FORMAT pixelFormat, void *pBuffer, size_t pBuffer_n) {
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
	CR(pTexture->SetFormat(pixelFormat));

	GLenum glFormat = pTexture->GetOGLPixelFormat();
	GLint internalGLFormat = GetOGLPixelFormat(PIXEL_FORMAT::Unspecified, channels);

	//CR(pTexture->CopyTextureImageBuffer(width, height, channels, pBuffer, (int)(pBuffer_n)));
	//CR(pTexture->AllocateGLTexture());
	CR(pTexture->AllocateGLTexture((unsigned char*)(pBuffer), internalGLFormat, glFormat, GL_UNSIGNED_BYTE));

	// TODO: Rename or remove this / specialize more
	if (type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		CR(pTexture->SetDefaultCubeMapParams());
	}
	else {
		CR(pTexture->SetDefaultTextureParams());
	}

	// TODO: Temp
	CRM(pTexture->SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(pTexture->SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

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

RESULT OGLTexture::LoadImageFromTexture(int level, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	// Create the buffer in memory
	if (m_pImage == nullptr) {
		m_pImage = ImageFactory::MakeMemoryImage(IMAGE_TYPE::IMAGE_MEMORY, m_width, m_height, m_channels);
		CN(m_pImage);
	}

	uint8_t *pBuffer = m_pImage->GetImageBuffer();
	size_t pBuffer_n = m_pImage->GetImageBufferSize();

	m_pParentImp->GetTextureImage(m_textureIndex, 0, GetOGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), (GLvoid*)(pBuffer));

	CN(pBuffer);

	// TODO: Update the texture image here (call super?)

Error:
	return r;
}

RESULT OGLTexture::UpdateDimensions(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CBR((pxWidth != m_width || pxHeight != m_height), R_NOT_HANDLED);

	// Re-alloc the texture
	m_width = pxWidth;
	m_height = pxHeight;

	CR(Bind());

	// TODO: Pull deeper settings from texture object
	CR(m_pParentImp->TexImage2D(m_textureTarget, 0, m_glInternalFormat, m_width, m_height, 0, m_glFormat, m_glPixelDataType, nullptr));			 

Error:
	return r;
}

RESULT OGLTexture::Update(unsigned char* pBuffer, int width, int height, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	CR(Bind());

	// Protect against copying larger than texture
	int pxWidth = width;
	int pxHeight = height;

	// TODO: Flag an issue
	if (pxWidth > m_width)
		pxWidth = m_width;

	if (pxHeight > m_height)
		pxHeight = m_height;

	CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pxWidth, pxHeight, GetOGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, pBuffer));

Error:
	return r;
}

GLenum OGLTexture::GetOGLPixelFormat() {
	return GetOGLPixelFormat(m_pixelFormat, m_channels);
}

GLenum OGLTexture::GetOGLPixelFormat(PIXEL_FORMAT pixelFormat, int channels) {
	switch (pixelFormat) {
		case PIXEL_FORMAT::Unspecified: {
			if (channels == 3)
				return GL_RGB; 
			else 
				return GL_RGBA;
		} break;

		case PIXEL_FORMAT::RGB: {
			return GL_RGB; 
		} break;

		case PIXEL_FORMAT::RGBA: {
			return GL_RGBA;
		} break;

		case PIXEL_FORMAT::BGR: {
			return GL_BGR;
		} break;

		case PIXEL_FORMAT::BGRA: {
			return GL_BGRA;
		} break;
	}

	return 0; // no format for unknown
}
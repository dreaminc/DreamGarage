#include "OGLTexture.h"

#include "core/image/image.h"
#include "core/image/ImageFactory.h"

#include "OGLImp.h"

OGLTexture::OGLTexture(OGLImp *pParentImp, texture::type type, GLenum textureTarget) :
	texture(type),
	m_glTextureIndex(0),
	m_glTextureTarget(textureTarget),
	m_pParentImp(pParentImp)
{
	// This constructor should be used when deeper configuration is sought 
}

OGLTexture::OGLTexture(const OGLTexture &pOGLTexture) :
	texture((const texture&)(pOGLTexture)),
	m_glTextureIndex(0),
	m_glTextureTarget(pOGLTexture.m_glTextureTarget),
	m_pParentImp(pOGLTexture.m_pParentImp),
	m_glFormat(pOGLTexture.m_glFormat),
	m_glInternalFormat(pOGLTexture.m_glInternalFormat),
	m_glPixelDataType(pOGLTexture.m_glPixelDataType)
{
	// empty
	// NOTE: this will not copy buffers on either GPU or CPU side
}

OGLTexture::OGLTexture(OGLImp *pParentImp, texture::params *pTextureParams) :
	texture(pTextureParams),
	m_glTextureIndex(0),
	m_pParentImp(pParentImp)
{
	switch(pTextureParams->textureType) {
		case texture::type::RECTANGLE: {
			m_glTextureTarget = GL_TEXTURE_RECTANGLE;
		} break;

		case texture::type::TEXTURE_2D: {
			m_glTextureTarget = GL_TEXTURE_2D;
		} break;
	}
}

OGLTexture::~OGLTexture() {
	RESULT r = R_PASS;

	texture::~texture();

	if (m_glTextureIndex != 0) {
		m_pParentImp->DeleteTextures(1, &m_glTextureIndex);
		m_glTextureIndex = 0;
	}

	if (m_glFramebufferIndex != 0) {
		m_pParentImp->glDeleteFramebuffers(1, &m_glFramebufferIndex);
		m_glFramebufferIndex = 0;
	}

	if (m_glFlippedTextureIndex != 0) {
		m_pParentImp->DeleteTextures(1, &m_glFlippedTextureIndex);
		m_glFlippedTextureIndex = 0;
	}

	CR(DeallocateOGLPBOPack());
	CR(DeallocateOGLPBOUnpack());
Error:
	return;
}

RESULT OGLTexture::Bind() {
	return m_pParentImp->BindTexture(m_glTextureTarget, m_glTextureIndex);
}

RESULT OGLTexture::BindPixelUnpackBuffer(int index) {
	return m_pParentImp->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_glPixelUnpackBufferIndex[index]);
}

RESULT OGLTexture::BindPixelPackBuffer(int index) {
	return m_pParentImp->glBindBuffer(GL_PIXEL_PACK_BUFFER, m_glPixelPackBufferIndex[index]);
}

RESULT OGLTexture::SetTextureParameter(GLenum paramName, GLint paramVal) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());

	CR(Bind());
	CR(m_pParentImp->TexParameteri(m_glTextureTarget, paramName, paramVal));

Error:
	return r;
}

// TODO: Border?
RESULT OGLTexture::AllocateGLTexture(unsigned char *pImageBuffer, GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType) {
	RESULT r = R_PASS;
	
	CR(Bind());

	m_glInternalFormat = internalGLFormat;
	m_glFormat = glFormat;
	m_glPixelDataType = pixelDataType;

	CR(m_pParentImp->TexImage2D(m_glTextureTarget,		// Texture Target
								0,						// Level
								m_glInternalFormat,		// Internal format
								m_width,				// width
								m_height,				// height
								0,						// border 
								m_glFormat,				// format
								m_glPixelDataType,		// pixel data type
								pImageBuffer));			// buffer data

Error:
	return r;
}

RESULT OGLTexture::AllocateGLTexture(size_t optOffset) {
	RESULT r = R_PASS;

	GLenum glFormat = GetOpenGLPixelFormat(m_pixelFormat, m_channels);
	GLint internalGLFormat = GetInternalOpenGLPixelFormat(m_pixelFormat, m_bitsPerPixel, m_channels);

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

OGLTexture* OGLTexture::MakeTextureWithFormat(OGLImp *pParentImp, texture::type type,
											  int width, int height, int channels, 
											  GLint internalGLFormat, GLenum glFormat, GLenum pixelDataType, 
											  int levels, int samples) 
{
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	// TODO: Get rid of other texture targets and do 2d, cube, rectangle (others?)
	if (type == texture::type::RECTANGLE) {
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
	CR(m_pParentImp->TexImage2D(m_glTextureTarget, 0, m_glInternalFormat, m_width, m_height, 0, m_glFormat, m_glPixelDataType, pTextureBuffer));

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

OGLTexture* OGLTexture::MakeTexture(OGLImp *pParentImp, texture::type type, int width, int height, int channels, int levels, int samples) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	// TODO: Get rid of other texture targets and do 2d, cube, rectangle (others?)
	if (type == texture::type::RECTANGLE) {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_RECTANGLE);
		CN(pTexture);
	}
	else {
		pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
		CN(pTexture);
	}
	
	CR(pTexture->SetFormat(PIXEL_FORMAT::BGRA));

	CR(pTexture->OGLInitialize(NULL));
	CR(pTexture->SetParams(width, height, channels, samples, levels));
	CR(pTexture->AllocateGLTexture());

	// TODO: Rename or remove this / specialize more
	CR(pTexture->SetDefaultTextureParams());

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromAllocatedTexture(OGLImp *pParentImp, texture::type type, GLenum textureTarget, GLuint textureID, int width, int height, int channels, int levels, int samples) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
	CN(pTexture);

	CR(pTexture->OGLInitialize(textureID));
	CR(pTexture->SetParams(width, height, channels, samples, levels));

Error:
	return pTexture;
}



OGLTexture* OGLTexture::MakeTextureFromPath(OGLImp *pParentImp, texture::type type, std::wstring wstrFilename) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	// TODO: Rectangle is only used rarely - should create special lane
	if (type == texture::type::RECTANGLE) {
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


	CR(pTexture->SetDefaultTextureParams());

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromBuffer(OGLImp *pParentImp, texture::type type, int width, int height, int channels, PIXEL_FORMAT pixelFormat, void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;
	
	pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
	CN(pTexture);

	CR(pTexture->OGLInitialize(NULL));
	CR(pTexture->SetParams(width, height, channels));
	CR(pTexture->SetFormat(pixelFormat));

	GLenum glFormat = GetOpenGLPixelFormat(pixelFormat, channels);
	GLint internalGLFormat = GetInternalOpenGLPixelFormat(pixelFormat, 8, channels);

	CR(pTexture->AllocateGLTexture((unsigned char*)(pBuffer), internalGLFormat, glFormat, GL_UNSIGNED_BYTE));
	
	CR(pTexture->SetDefaultTextureParams());

	// TODO: Temp
	CRM(pTexture->SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(pTexture->SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

Error:
	return pTexture;
}

OGLTexture* OGLTexture::MakeTextureFromFileBuffer(OGLImp *pParentImp, texture::type type, void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	pTexture = new OGLTexture(pParentImp, type, GL_TEXTURE_2D);
	CN(pTexture);

	CR(pTexture->OGLInitialize(NULL));

	CR(pTexture->LoadTextureFromFileBuffer((uint8_t*)pBuffer, pBuffer_n));
	CR(pTexture->AllocateGLTexture());


	CR(pTexture->SetDefaultTextureParams());

Error:
	return pTexture;
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

	CRM(SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MAG_FILTER");
	CRM(SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR), "Failed to set GL_TEXTURE_MIN_FILTER");

	CRM(SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");
	CRM(SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "Failed to set texture wrap");

	// EXP: Mip maps
	//CRM(SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST), "Failed to set GL_TEXTURE_MIN_FILTER");
	//CRM(m_pParentImp->glGenerateMipmap(m_glTextureTarget), "Failed to generate mip maps");

Error:
	return r;
}

RESULT OGLTexture::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLInitialize(NULL));
	CR(AllocateGLTexture());
	CR(SetDefaultTextureParams());

Error:
	return r;
}

RESULT OGLTexture::OGLInitialize(GLuint textureID) {
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

// border will be zero more often than the buffer is 
RESULT OGLTexture::OGLInitializeTexture(GLenum textureTarget, GLint level, GLint internalformat, GLenum format, GLenum type, const void *pBuffer, GLint border) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, &m_glTextureIndex));
	
	CR(m_pParentImp->BindTexture(textureTarget, m_glTextureIndex));

	CR(m_pParentImp->TexImage2D(textureTarget, level, internalformat, m_width, m_height, border, format, type, pBuffer));	

Error:
	return r;
}

// TODO: Move up 
RESULT OGLTexture::OGLInitializeMultisample(int multisample) {
	RESULT r = R_PASS;

	CR(m_pParentImp->MakeCurrentContext());
	CR(m_pParentImp->GenerateTextures(1, &m_glTextureIndex));

	CB((m_glTextureTarget == GL_TEXTURE_2D_MULTISAMPLE));

	CR(m_pParentImp->BindTexture(m_glTextureTarget, m_glTextureIndex));
	CR(m_pParentImp->glTexImage2DMultisample(m_glTextureTarget, multisample, GL_RGBA8, m_width, m_height, true));

Error:
	return r;
}

RESULT OGLTexture::OGLActivateTexture(int value) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glActiveTexture(GetGLTextureNumberDefine(value)));
	CR(m_pParentImp->BindTexture(m_glTextureTarget, m_glTextureIndex));

Error:
	return r;
}

GLenum OGLTexture::GetGLTextureNumberDefine(int value) {
	return (GLenum)((GL_TEXTURE0) + value);
}

GLuint OGLTexture::GetOGLTextureIndex() {
	return m_glTextureIndex;
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

	m_pParentImp->GetTextureImage(m_glTextureIndex, 0, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), (GLvoid*)(pBuffer));

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
	CR(m_pParentImp->TexImage2D(m_glTextureTarget, 0, m_glInternalFormat, m_width, m_height, 0, m_glFormat, m_glPixelDataType, nullptr));			 

	// Handle the pack PBO resizing
	if (IsOGLPBOPackEnabled()) {
		CR(DeallocateOGLPBOPack());
		CR(EnableOGLPBOPack());
	}

	// Handle the unpack PBO resizing
	if (IsOGLPBOUnpackEnabled()) {
		CR(DeallocateOGLPBOUnpack());
		CR(EnableOGLPBOUnpack());
	}

Error:
	return r;
}

RESULT OGLTexture::LoadBufferFromTexture(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	PIXEL_FORMAT pixelFormat = m_pixelFormat;

	m_pParentImp->MakeCurrentContext();

	if (IsOGLPBOPackEnabled()) {
		//// Set the target framebuffer to read
		// glReadBuffer(GL_FRONT);

		// read pixels from framebuffer to PBO
		// glReadPixels() should return immediately.
		
		// TODO: Needed?  Only if we want to do two PBOs for unpack?
		CR(BindPixelPackBuffer(m_packBufferIndex));

		//m_pParentImp->glReadPixels(0, 0, m_width, m_height, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, 0);
		CR(m_pParentImp->GetTextureImage(m_glTextureIndex, 0, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), NULL));	

		// Map the PBO to process its data by CPU (other PBO as to avoid waiting)
		//CR(BindPixelPackBuffer(m_packBufferIndex));
		void *pPackPBO = m_pParentImp->glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

		CN(pPackPBO);
		
		// Update data directly on the mapped buffer
		memcpy((void*)pBuffer, (void*)pPackPBO, pBuffer_n);

		m_pParentImp->glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

		// back to conventional pixel operation
		m_pParentImp->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		// increment index
		m_packBufferIndex = (m_packBufferIndex + 1) % NUM_PACK_BUFFERS;
		
	}
	else {
		//CR(m_pParentImp->GetTextureImage(m_glTextureIndex, 0, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), (GLvoid*)(pBuffer)));

		CR(m_pParentImp->GetTextureImage(m_glTextureIndex, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), (GLvoid*)(pBuffer)));
		
	}

	CN(pBuffer);

Error:
	return r;
}

RESULT OGLTexture::LoadFlippedBufferFromTexture(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	PIXEL_FORMAT pixelFormat = m_pixelFormat;

	m_pParentImp->MakeCurrentContext();

	{
		CR(m_pParentImp->CheckGLError());
		if (m_glFramebufferIndex == 0) {
			CR(m_pParentImp->glGenFramebuffers(1, &m_glFramebufferIndex));
			CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_glFramebufferIndex));
			CR(m_pParentImp->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glTextureIndex, 0));

			CR(m_pParentImp->GenerateTextures(1, &m_glFlippedTextureIndex));
			CR(m_pParentImp->BindTexture(GL_TEXTURE_2D, m_glFlippedTextureIndex));
			CR(m_pParentImp->TexImage2D(m_glTextureTarget, 0, m_glInternalFormat, m_width, m_height, 0, m_glFormat, m_glPixelDataType, 0));
			SetDefaultTextureParams();

			CR(m_pParentImp->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_glFlippedTextureIndex, 0));
		}

		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_glFramebufferIndex));
		CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_glFramebufferIndex));

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);

		CR(m_pParentImp->glBlitFramebuffer(0, 0, m_width, m_height,
			0, m_height, m_width, 0,
			GL_COLOR_BUFFER_BIT, GL_NEAREST));

		CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	}
	/*
	if (IsOGLPBOPackEnabled()) {
		// Set the target framebuffer to read
		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_glFramebufferIndex));
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		
		// read pixels from framebuffer to PBO
		// glReadPixels() should return immediately.

		// TODO: Needed?  Only if we want to do two PBOs for unpack?
		CR(BindPixelPackBuffer(m_packBufferIndex));

		glReadPixels(0, 0, m_width, m_height, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, 0);
		//CR(m_pParentImp->GetTextureImage(m_glTextureIndex, 0, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), NULL));

		// increment index
		m_packBufferIndex = (m_packBufferIndex + 1) % NUM_PACK_BUFFERS;

		// Map the PBO to process its data by CPU (other PBO as to avoid waiting)
		//CR(BindPixelPackBuffer(m_packBufferIndex));
		void *pPackPBO = m_pParentImp->glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

		CN(pPackPBO);

		// Update data directly on the mapped buffer
		memcpy((void*)pBuffer, (void*)pPackPBO, pBuffer_n);

		m_pParentImp->glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

		// back to conventional pixel operation
		m_pParentImp->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
		CR(m_pParentImp->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	}
	else {
	*/
		//CR(m_pParentImp->GetTextureImage(m_glTextureIndex, 0, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), (GLvoid*)(pBuffer)));
	CR(m_pParentImp->GetTextureImage(m_glFlippedTextureIndex, 0, GetOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, (GLsizei)(pBuffer_n), (GLvoid*)(pBuffer)));
	//}

	CN(pBuffer);

Error:
	return r;
}

RESULT OGLTexture::UpdateTextureFromBuffer(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	CB((GetTextureSize() == pBuffer_n));

	CR(Bind());

	if (IsOGLPBOUnpackEnabled()) {
		CR(BindPixelUnpackBuffer(m_unpackBufferIndex));

		CR(m_pParentImp->glBufferData(GL_PIXEL_UNPACK_BUFFER, pBuffer_n, 0, GL_STREAM_DRAW));

		void* pUnpackPBO = m_pParentImp->glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		CN(pUnpackPBO);

		// update the data here
		//updatePixels(ptr, DATA_SIZE);

		// Update data directly on the mapped buffer
		memcpy((void*)pUnpackPBO, (void*)pBuffer, pBuffer_n);

		// release pointer to mapping buffer
		m_pParentImp->glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	
		CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GetOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, NULL));

		// It is good idea to release PBOs with ID 0 after use.
		// Once bound with 0, all pixel operations behave normal ways.
		(m_pParentImp->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

		// increment index
		m_unpackBufferIndex = (m_unpackBufferIndex + 1) % NUM_PACK_BUFFERS;	
	}
	else {
		CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GetOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, pBuffer));
	}

	CRM(m_pParentImp->CheckGLError(), "UpdateTextureFromBuffer failed");

Error:
	return r;
}

RESULT OGLTexture::UpdateTextureRegionFromBuffer(void *pBuffer, int x, int y, int width, int height) {
	RESULT r = R_PASS;

	size_t pBuffer_n = width * height * m_channels;

	CBM((GetTextureSize() >= pBuffer_n), "texture region cannot be larger than the texture");

	CR(Bind());
	/*
	if (IsOGLPBOUnpackEnabled()) {
		CR(BindPixelUnpackBuffer());

		CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GetOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, NULL));

		// Needed?  Only if we want to do two PBOs for unpack?
		CR(BindPixelUnpackBuffer());

		CR(m_pParentImp->glBufferData(GL_PIXEL_UNPACK_BUFFER, pBuffer_n, 0, GL_STREAM_DRAW));

		void* pUnpackPBO = m_pParentImp->glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		CN(pUnpackPBO);

		// update the data here
		//updatePixels(ptr, DATA_SIZE);

		// Update data directly on the mapped buffer
		memcpy((void*)pUnpackPBO, (void*)pBuffer, pBuffer_n);

		// release pointer to mapping buffer
		m_pParentImp->glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);


		// It is good idea to release PBOs with ID 0 after use.
		// Once bound with 0, all pixel operations behave normal ways.
		(m_pParentImp->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

	}
	else {
	*/
	CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GetOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, pBuffer));
	//}

	CRM(m_pParentImp->CheckGLError(), "UpdateTextureRegionFromBuffer failed");

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

	CR(m_pParentImp->TextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pxWidth, pxHeight, GetOpenGLPixelFormat(pixelFormat), GL_UNSIGNED_BYTE, pBuffer));

Error:
	return r;
}

RESULT OGLTexture::EnableOGLPBOUnpack() {
	RESULT r = R_PASS;

	// Create pixel unpack buffer objects
	// glBufferData() with NULL pointer reserves only memory space

	CR(m_pParentImp->glGenBuffers(NUM_UNPACK_BUFFERS, m_glPixelUnpackBufferIndex));

	for (int i = 0; i < NUM_UNPACK_BUFFERS; i++) {
		CR(m_pParentImp->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_glPixelUnpackBufferIndex[i]));
		CR(m_pParentImp->glBufferData(GL_PIXEL_UNPACK_BUFFER, GetTextureSize(), 0, GL_STREAM_DRAW));
	}

	CR(m_pParentImp->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

Error:
	return r;
}

RESULT OGLTexture::EnableOGLPBOPack() {
	RESULT r = R_PASS;

	// Create pixel unpack buffer objects
	// glBufferData() with NULL pointer reserves only memory space

	CR(m_pParentImp->glGenBuffers(NUM_PACK_BUFFERS, m_glPixelPackBufferIndex));

	for (int i = 0; i < NUM_PACK_BUFFERS; i++) {
		CR(m_pParentImp->glBindBuffer(GL_PIXEL_PACK_BUFFER, m_glPixelPackBufferIndex[i]));
		CR(m_pParentImp->glBufferData(GL_PIXEL_PACK_BUFFER, GetTextureSize(), 0, GL_STREAM_READ));
	}

	CR(m_pParentImp->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

Error:
	return r;
}

RESULT OGLTexture::DeallocateOGLPBOPack() {
	for (int i = 0; i < NUM_PACK_BUFFERS; i++) {
		if (m_glPixelPackBufferIndex[i] != 0) {
			m_pParentImp->glDeleteBuffers(1, &(m_glPixelPackBufferIndex[i]));
			m_glPixelPackBufferIndex[i] = 0;
		}
	}

	return R_PASS;
}

RESULT OGLTexture::DeallocateOGLPBOUnpack() {
	for (int i = 0; i < NUM_UNPACK_BUFFERS; i++) {
		if (m_glPixelUnpackBufferIndex != 0) {
			m_pParentImp->glDeleteBuffers(1, &m_glPixelUnpackBufferIndex[i]);
			m_glPixelUnpackBufferIndex[i] = 0;
		}
	}

	return R_PASS;
}

bool OGLTexture::IsOGLPBOUnpackEnabled() {
	return (m_glPixelUnpackBufferIndex[0] != 0);
}

bool OGLTexture::IsOGLPBOPackEnabled() {
	return (m_glPixelPackBufferIndex[0] != 0);
}

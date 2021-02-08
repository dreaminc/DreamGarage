#include "texture.h"

#include <locale>
#include <codecvt>
#include <utility>
#include <regex>

#include "path_manager/PathManager.h"

#include "core/image/ImageFactory.h"

texture::texture() :
	m_params(texture::type::INVALID)
{
	Invalidate();
	return;
}

texture::texture(texture::type texType) :
	m_params(texType)
{
	Validate();
	return;
}

texture::texture(texture::params *pTextureParams) :
	m_params(*pTextureParams)
{
	Validate();
	return;
}

texture::texture(const texture& tex) :
	m_width(tex.m_width),
	m_height(tex.m_height),
	m_channels(tex.m_channels),
	m_samples(tex.m_samples),
	m_pixelFormat(tex.m_pixelFormat),
	m_params(tex.m_params.textureType)
{
	// NOTE: this will not copy buffers on either GPU or CPU side
	Validate();
	return;
}

texture::texture(texture::type texType, int width, int height, int channels, int samples) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_samples(samples),
	m_params(texType)
{
	Validate();
	return;
}

texture::texture(texture::type texType, int width, int height, int channels, void *pBuffer, int pBuffer_n, int samples) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_samples(samples),
	m_params(texType)
{
	RESULT r = R_PASS;

	CR(CopyTextureImageBuffer(width, height, channels, pBuffer, pBuffer_n))
	
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// Loads from a file buffer (file loaded into buffer)
texture::texture(texture::type texType, uint8_t *pBuffer, size_t pBuffer_n) :
	m_params(texType)
{
	RESULT r = R_PASS;

	CR(LoadTextureFromFileBuffer(pBuffer, pBuffer_n));	

	Validate();
	return;
Error:
	Invalidate();
	return;
}

texture::texture(texture::type texType, int width, int height, PIXEL_FORMAT format, int channels, void *pBuffer, int pBuffer_n, int samples) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_samples(samples),
	m_pixelFormat(format),
	m_params(texType)
{
	RESULT r = R_PASS;

	CR(CopyTextureImageBuffer(width, height, channels, pBuffer, pBuffer_n));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

texture::texture(texture::type texType, wchar_t *pszFilename) :
	m_params(texType)
{
	RESULT r = R_PASS;

	CR(LoadTextureFromFile(pszFilename));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

texture::~texture() {
	if (m_pImage != nullptr) {
		delete m_pImage;
		m_pImage = nullptr;
	}
}

RESULT texture::GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();

	// Move to key based file paths
	CRM(pPathManager->GetFilePath(PATH_TEXTURE, pszFilename, n_pszFilePath), "Failed to get path for %S texture", pszFilename);
	CN(n_pszFilePath);

	return r;

Error:
	if (n_pszFilePath != nullptr) {
		delete[] n_pszFilePath;
		n_pszFilePath = nullptr;
	}
	return r;
}

RESULT texture::ReleaseTextureData() {
	RESULT r = R_PASS;

	if (m_pImage != nullptr) {
		delete m_pImage;
		m_pImage = nullptr;
	}

Error:
	return r;
}

double texture::GetAverageValueAtUV(double uValue, double vValue) {
	if (m_pImage != nullptr) {
		return m_pImage->GetAverageValueAtUV(uValue, vValue);
	}
	else {
		return 0.0f;
	}
}

RESULT texture::SetParams(int pxWidth, int pxHeight, int channels, UNUSED int samples, int levels) {
	RESULT r = R_PASS;

	CR(SetWidth(pxWidth));
	CR(SetHeight(pxHeight));
	CR(SetChannels(channels));
	CR(SetLevels(levels));

Error:
	return r;
}

RESULT texture::LoadTextureFromPath(const wchar_t *pszFilepath) {
	RESULT r = R_PASS;

	if (m_pImage != nullptr) {
		delete m_pImage;
		m_pImage = nullptr;
	}

	m_pImage = ImageFactory::MakeImageFromPath(IMAGE_FREEIMAGE, std::wstring(pszFilepath));
	CN(m_pImage);

Error:
	return r;
}

RESULT texture::CopyTextureImageBuffer(int width, int height, int channels, void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	//CN(m_pImage);

	CR(m_pImage->CopyBuffer(width, height, channels, pBuffer, pBuffer_n));

Error:
	return r;
}

RESULT texture::LoadBufferFromTexture(UNUSED void *pBuffer, UNUSED size_t pBuffer_n) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT texture::LoadFlippedBufferFromTexture(UNUSED void *pBuffer, UNUSED size_t pBuffer_n) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT texture::LoadImageFromTexture(UNUSED int level, UNUSED PIXEL_FORMAT pixelFormat) {
	return R_NOT_HANDLED;
}

RESULT texture::LoadTextureFromFile(const wchar_t *pszFilename) {
	RESULT r = R_PASS;
	wchar_t *pszFilePath = nullptr;

	// Check if this is an absolute path
	PathManager *pPathManager = PathManager::instance();

	if (pPathManager->IsDreamPath(const_cast<wchar_t*>(pszFilename))) {
		// TODO: set to dream path
		CN(m_pImage);
	}
	else if (pPathManager->IsAbsolutePath(const_cast<wchar_t*>(pszFilename))) {
		CR(LoadTextureFromPath(const_cast<wchar_t*>(pszFilename)));
		CN(m_pImage);
	}
	else {
		CR(GetTextureFilePath(pszFilename, pszFilePath));
		CN(pszFilePath);

		CR(LoadTextureFromPath(pszFilePath));
		CN(m_pImage);
	}

	m_pixelFormat = m_pImage->GetPixelFormat();

	// Update sizing
	m_width = m_pImage->GetWidth();
	m_height = m_pImage->GetHeight();
	m_channels = m_pImage->GetChannels();

	// Flip image
	//CR(m_pImage->FlipVertical());

Error:
	if (pszFilePath != nullptr) {
		delete[] pszFilePath;
		pszFilePath = nullptr;
	}

	return r;
}

RESULT texture::LoadTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;
	
	m_pImage = ImageFactory::MakeImageFromMemory(IMAGE_FREEIMAGE, (unsigned char*)(pBuffer), pBuffer_n);
	CN(m_pImage);

	// Update sizing
	m_width = m_pImage->GetWidth();
	m_height = m_pImage->GetHeight();
	m_channels = m_pImage->GetChannels();

	m_pixelFormat = PIXEL_FORMAT::BGRA;	// TODO: move this into image
	//m_pixelFormat = PIXEL_FORMAT::RGBA;	// TODO: move this into image

	// Flip image
	//CR(m_pImage->FlipVertical());

Error:
	return r;
}

// TODO: Based on 8 bit per channel at the moment
// Note this returns the size of the texture, in the case of cube maps this refers to 
// one side not all six textures - for that use GetCubeMapSize
size_t texture::GetTextureSize() {
	const size_t sizeTexture = m_width * m_height * m_channels * sizeof(unsigned char);
	return sizeTexture;
}

RESULT texture::UpdateDimensions(UNUSED int width, UNUSED int height) {
	return R_NOT_HANDLED;
}

RESULT texture::Update(UNUSED unsigned char* pBuffer, UNUSED int width, UNUSED int height, UNUSED PIXEL_FORMAT pixelFormat) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT texture::UpdateTextureFromBuffer(UNUSED void *pBuffer, UNUSED size_t pBuffer_n) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT texture::UpdateTextureRegionFromBuffer(UNUSED void *pBuffer, UNUSED int x, UNUSED int y, UNUSED int width, UNUSED int height) {
	return R_NOT_IMPLEMENTED_WARNING;
}

bool texture::IsDistanceMapped() {
	return ((m_flags & texture::flags::DISTANCE_MAP) != texture::flags::NONE);
}

RESULT texture::SetDistanceMapped() {
	m_flags = m_flags | texture::flags::DISTANCE_MAP;
	return R_PASS;
}

bool texture::IsUVVerticalFlipped() {
	return ((m_flags & texture::flags::UV_VERTICAL_FLIPPED) != texture::flags::NONE);
}

RESULT texture::SetUVVerticalFlipped() {
	m_flags = m_flags | texture::flags::UV_VERTICAL_FLIPPED;
	return R_PASS;
}

uint8_t *texture::GetImageBuffer() {
	if (m_pImage != nullptr)
		return m_pImage->GetImageBuffer();
	else
		return nullptr;
}
#include "texture.h"

#include "Sandbox/PathManager.h"

//#include <string>
#include <locale>
#include <codecvt>
#include <utility>
#include <regex>

#include "Primitives/image/ImageFactory.h"

texture::texture() :
	m_type(texture::TEXTURE_TYPE::TEXTURE_INVALID)
{
	Validate();
}

texture::texture(const texture& tex) :
	m_width(tex.m_width),
	m_height(tex.m_height),
	m_channels(tex.m_channels),
	m_samples(tex.m_samples),
	m_format(tex.m_format),
	m_type(tex.m_type)
{
	// NOTE: this will not copy buffers on either GPU or CPU side
	Validate();
}

texture::texture(texture::TEXTURE_TYPE type) :
	m_type(type)
{
	Validate();
}

texture::texture(texture::TEXTURE_TYPE type, int width, int height, int channels, int samples) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_samples(samples),
	m_type(type)
{
	Validate();
}

texture::texture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n, int samples) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_samples(samples),
	m_type(type) 
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
texture::texture(texture::TEXTURE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n) :
	m_type(type)
{
	RESULT r = R_PASS;

	CR(LoadTextureFromFileBuffer(pBuffer, pBuffer_n));	

	Validate();
	return;
Error:
	Invalidate();
	return;
}

texture::texture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n, int samples) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_samples(samples),
	m_format(format),
	m_type(type)
{
	RESULT r = R_PASS;

	CR(CopyTextureImageBuffer(width, height, channels, pBuffer, pBuffer_n));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

texture::texture(wchar_t *pszFilename, texture::TEXTURE_TYPE type = texture::TEXTURE_TYPE::TEXTURE_INVALID) :
	m_type(type)
{
	RESULT r = R_PASS;

	if(type == texture::TEXTURE_TYPE::TEXTURE_CUBE) {
		CR(LoadCubeMapByName(pszFilename));
	}
	else {
		CR(LoadTextureFromFile(pszFilename));
	}

	Validate();
	return;

Error:
	Invalidate();
	return;
}

texture::texture(wchar_t *pszName, std::vector<std::wstring> cubeMapFiles) :
	m_type(texture::TEXTURE_TYPE::TEXTURE_CUBE)
{
	RESULT r = R_PASS;

	CR(LoadCubeMapFromFiles(pszName, cubeMapFiles));

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

// The texture type and channel
enum class TEXTURE_TYPE {
	TEXTURE_DIFFUSE = 0,
	TEXTURE_BUMP = 1,
	TEXTURE_INVALID = 32
};

RESULT texture::SetTextureType(texture::TEXTURE_TYPE textureType) {
	m_type = textureType;
	//return SetTextureNumber(static_cast<int>(textureType));
	return R_PASS;
}

/*
int texture::GetTextureNumber() {
	return m_textureNumber;
}
*/

texture::TEXTURE_TYPE texture::GetTextureType() {
	//return static_cast<texture::TEXTURE_TYPE>(m_textureNumber);
	return m_type;
}

/*
RESULT texture::SetTextureNumber(int texNum) {
	m_textureNumber = texNum;
	m_type = GetTextureType();
	return R_PASS;
}
*/

RESULT texture::GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	//wchar_t *pFilePath = nullptr;

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

RESULT texture::GetCubeMapFiles(const wchar_t *pszName, std::vector<std::wstring> &vstrFiles) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::vector<std::wstring> vstrPathFiles;
	
	CRM(pPathManager->GetFilesForNameInPath(PATH_TEXTURE_CUBE, pszName, vstrPathFiles), "Failed to get files for %S cube map", pszName);

	// Filter out only the names - note: this will fail with unicode
	for(auto &strFilename : vstrPathFiles) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> StringConverter;
		std::string strFilenameConverted = StringConverter.to_bytes(strFilename);

		std::regex strRegEx("((pos|neg)(x|y|z))\\.(([a-z]){3,3})");

		if(std::regex_match(strFilenameConverted, strRegEx))
			vstrFiles.push_back(strFilename);
	}

Error:
	return r;
}

RESULT texture::ReleaseTextureData() {
	RESULT r = R_PASS;

	if (m_pImage != nullptr) {
		delete m_pImage;
		m_pImage = nullptr;
	}

//Error:
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

RESULT texture::SetParams(int pxWidth, int pxHeight, int channels, int samples, int levels) {
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

	CN(m_pImage);

	CR(m_pImage->CopyBuffer(width, height, channels, pBuffer, pBuffer_n));

Error:
	return r;
}

RESULT texture::LoadTextureFromFile(const wchar_t *pszFilename) {
	RESULT r = R_PASS;
	wchar_t *pszFilePath = nullptr;

	// Check if this is an absolute path
	PathManager *pPathManager = PathManager::instance();

	if (pPathManager->IsDreamPath(const_cast<wchar_t*>(pszFilename))) {
		// TODO: set to dream path
	}
	else if (pPathManager->IsAbsolutePath(const_cast<wchar_t*>(pszFilename))) {
		CR(LoadTextureFromPath(const_cast<wchar_t*>(pszFilename)));
	}
	else {
		CR(GetTextureFilePath(pszFilename, pszFilePath));
		CN(pszFilePath);

		CR(LoadTextureFromPath(pszFilePath));
		CN(m_pImage);

		// Update sizing
		m_width = m_pImage->GetWidth();
		m_height = m_pImage->GetHeight();
		m_channels = m_pImage->GetChannels();

		// FreeImage uses a BGR[A] pixel layout under a Little Endian processor (Windows, Linux) 
		// and uses a RGB[A] pixel layout under a Big Endian processor (Mac OS X or any Big Endian Linux / Unix)
		m_format = PixelFormat::BGRA;	// TODO: move this into image
	}

	// Flip image
	CR(m_pImage->FlipVertical());

Error:
	if (pszFilePath != nullptr) {
		delete[] pszFilePath;
		pszFilePath = nullptr;
	}

	return r;
}

RESULT texture::LoadTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;
	
	//m_channels = 3;
	//m_pImageBuffer = SOIL_load_image_from_memory((unsigned char*)(pBuffer), (int)(pBuffer_n), &m_width, &m_height, NULL, SOIL_LOAD_RGB);
	//m_pImageBuffer = SOIL_load_image_from_memory((unsigned char*)(pBuffer), (int)(pBuffer_n), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
	m_pImage = ImageFactory::MakeImageFromMemory(IMAGE_FREEIMAGE, (unsigned char*)(pBuffer), pBuffer_n);
	CN(m_pImage);

	// Flip image
	CR(m_pImage->FlipVertical());

Error:
	return r;
}

RESULT texture::LoadCubeMapByName(const wchar_t * pszName) {
	RESULT r = R_PASS;

	std::vector<std::wstring> vstrCubeMapFiles;
	PathManager *pPathManager = PathManager::instance();

	CR(GetCubeMapFiles(pszName, vstrCubeMapFiles));

	CR(LoadCubeMapFromFiles(pszName, vstrCubeMapFiles));

	CN(m_pImage);

Error:
	return r;
}

texture::CUBE_MAP texture::GetCubeMapTypeFromFilename(std::wstring strFilename) {
	texture::CUBE_MAP retType = texture::CUBE_MAP::CUBE_MAP_INVALID;

	std::wstring strFace = strFilename.substr(0, 3);
	wchar_t wchAxis = tolower(strFilename[3]);

	switch(wchAxis) {
		case 'x': {
			if (strFace == L"pos")
				retType = texture::CUBE_MAP::CUBE_MAP_POS_X;
			else if (strFace == L"neg")
				retType = texture::CUBE_MAP::CUBE_MAP_NEG_X;
		} break;

		case 'y': {
			if (strFace == L"pos")
				retType = texture::CUBE_MAP::CUBE_MAP_POS_Y;
			else if (strFace == L"neg")
				retType = texture::CUBE_MAP::CUBE_MAP_NEG_Y;
		} break;

		case 'z': {
			if (strFace == L"pos")
				retType = texture::CUBE_MAP::CUBE_MAP_POS_Z;
			else if (strFace == L"neg")
				retType = texture::CUBE_MAP::CUBE_MAP_NEG_Z;
		} break;
	}

	return retType;
}

// TODO: Based on 8 bit per channel at the moment
// Note this returns the size of the texture, in the case of cube maps this refers to 
// one side not all six textures - for that use GetCubeMapSize
size_t texture::GetTextureSize() {
	const size_t sizeTexture = m_width * m_height * m_channels * sizeof(unsigned char);
	return sizeTexture;
}

size_t texture::GetCubeMapSize() {
	return GetTextureSize() * NUM_CUBE_MAP_TEXTURES;
}

//RESULT texture::LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight) {
RESULT texture::LoadCubeMapFromFiles(const wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles) {
	RESULT r = R_PASS;
	
	return R_FAIL;

	// TODO: Fix cube maps

	/*
	PathManager *pPathManager = PathManager::instance();
	uint8_t *pBuffers[NUM_CUBE_MAP_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	int widths[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };
	int heights[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };
	int channels[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };

	CBM((vstrCubeMapFiles.size() == 6), "LoadCubeMapFromFiles expects 6 files to be provided only %llu found", vstrCubeMapFiles.size());

	for (auto &strFilename : vstrCubeMapFiles) {
		std::wstring strFilePath;
		int CubeMapFace = static_cast<int>(GetCubeMapTypeFromFilename(strFilename));
		CRM(pPathManager->GetFilePathForName(PATH_TEXTURE_CUBE, pszName, strFilename, strFilePath), "Failed to get %S path for %S cube map", pszName, strFilename.c_str());

		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
		std::string strFilePathConverted = wstrConverter.to_bytes(strFilePath);

		pBuffers[CubeMapFace] = SOIL_load_image(strFilePathConverted.c_str(), &widths[CubeMapFace], &heights[CubeMapFace], &channels[CubeMapFace], SOIL_LOAD_AUTO);
		CN(pBuffers[CubeMapFace]);

		// Ensure all heights are the same
		if(m_width == 0) {
			m_width = widths[CubeMapFace];
			m_height = heights[CubeMapFace];
			m_channels = channels[CubeMapFace];
		}
		else {
			CBM((m_width == widths[CubeMapFace]), "Cube map width %d mismatches %d", widths[CubeMapFace], m_width);
			CBM((m_height == heights[CubeMapFace]), "Cube map height %d mismatches %d", heights[CubeMapFace], m_height);
			CBM((m_channels == channels[CubeMapFace]), "Cube map channels %d mismatches %d", channels[CubeMapFace], m_channels);
		}
	}

	// Stitch it together here
	size_t sizeSide = GetTextureSize();
	size_t sizeTexture = sizeSide * NUM_CUBE_MAP_TEXTURES;
	m_pImageBuffer = new unsigned char[sizeTexture];
	CNM(m_pImageBuffer, "Failed to allocate Image Buffer for cube map");

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		unsigned char *ptrOffset = m_pImageBuffer + i * (sizeSide);
		memcpy(ptrOffset, pBuffers[i], sizeSide);
	}

Error:
	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		if(pBuffers[i] != nullptr) {
			delete[] pBuffers[i];
			pBuffers[i] = nullptr;
		}
	}

	return r;
	*/
}

RESULT texture::Update(unsigned char* pBuffer, int width, int height, texture::PixelFormat pixelFormat) {
	return R_NOT_IMPLEMENTED;
}

bool texture::IsDistanceMapped() {
	return ((m_flags & texture::flags::DISTANCE_MAP) != texture::flags::NONE);
}

RESULT texture::SetDistanceMapped() {
	m_flags = m_flags | texture::flags::DISTANCE_MAP;
	return R_PASS;
}
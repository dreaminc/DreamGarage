#include "texture.h"
#include "External/SOIL/SOIL.h"

#include "Sandbox/PathManager.h"

//#include <string>
#include <locale>
#include <codecvt>
#include <utility>
#include <regex>

texture::texture() :
	m_pImageBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_channels(0),
	m_type(texture::TEXTURE_TYPE::TEXTURE_INVALID)
{
	Validate();
}

texture::texture(texture::TEXTURE_TYPE type, int width, int height, int channels) :
	m_pImageBuffer(nullptr),
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_type(type)
{
	Validate();
}

texture::texture(wchar_t *pszFilename, texture::TEXTURE_TYPE type = texture::TEXTURE_TYPE::TEXTURE_INVALID) :
	m_pImageBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_channels(0),
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
	m_pImageBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_channels(0),
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
	if (m_pImageBuffer != nullptr) {
		delete[] m_pImageBuffer;
		m_pImageBuffer = nullptr;
	}
}

// The texture type and channel
enum class TEXTURE_TYPE {
	TEXTURE_COLOR = 0,
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

RESULT texture::FlipTextureVertical() {
	RESULT r = R_PASS;

	CN(m_pImageBuffer);

	for (int i = 0; i * 2 < m_height; i++) {
		int index1 = i * m_width * m_channels;
		int index2 = (m_height - 1 - i) * m_width * m_channels;

		for (int j = m_width * m_channels; j > 0; j--) {
			std::swap(m_pImageBuffer[index1++], m_pImageBuffer[index2++]);
		}
	}

Error:
	return r;
}

RESULT texture::ReleaseTextureData() {
	RESULT r = R_PASS;

	SOIL_free_image_data(m_pImageBuffer);
	CB((m_pImageBuffer == nullptr));

Error:
	return r;
}

double texture::GetValueAtUV(double uValue, double vValue) {
	int pxValueX = uValue * m_width;
	int pxValueY = vValue * m_height;

	int lookUp = pxValueX * (sizeof(unsigned char) * m_channels) + (pxValueY * (sizeof(unsigned char) * m_channels * m_width));
	
	int accum = 0;
	for (int i = 0; i < m_channels; i++) {
		accum += m_pImageBuffer[lookUp + i];
	}

	double retVal = (double)((double)accum / (double)m_channels);
	retVal /= 255.0f;

	return retVal;
}

RESULT texture::LoadTextureFromPath(wchar_t *pszFilepath) {
	RESULT r = R_PASS;

	std::wstring wstrFilepath(pszFilepath);
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
	std::string strFilepath = wstrConverter.to_bytes(wstrFilepath);

	m_pImageBuffer = SOIL_load_image(strFilepath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
	CN(m_pImageBuffer);

Error:
	return r;
}

RESULT texture::LoadTextureFromFile(wchar_t *pszFilename) {
	RESULT r = R_PASS;
	wchar_t *pszFilePath = nullptr;

	CR(GetTextureFilePath(pszFilename, pszFilePath));
	CN(pszFilePath);

	CR(LoadTextureFromPath(pszFilePath));

	// Flip image
	CR(FlipTextureVertical());

Error:
	if (pszFilePath != nullptr) {
		delete[] pszFilePath;
		pszFilePath = nullptr;
	}

	return r;
}

RESULT texture::LoadCubeMapByName(wchar_t * pszName) {
	RESULT r = R_PASS;

	std::vector<std::wstring> vstrCubeMapFiles;
	PathManager *pPathManager = PathManager::instance();

	CR(GetCubeMapFiles(pszName, vstrCubeMapFiles));

	CR(LoadCubeMapFromFiles(pszName, vstrCubeMapFiles));
	CN(m_pImageBuffer);

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

// TODO: Based on 8 bit per channel atm
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
RESULT texture::LoadCubeMapFromFiles(wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles) {
	RESULT r = R_PASS;
	
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
}
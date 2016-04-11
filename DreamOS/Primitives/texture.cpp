#include "texture.h"
#include "External/SOIL/SOIL.h"

#include "Sandbox/PathManager.h"

//#include <string>
#include <locale>
#include <codecvt>
#include <utility>

texture::texture() :
	m_pImageBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_channels(0)
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

	CR(LoadTextureFromFile(pszFilename));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

texture::texture(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight) :
	m_pImageBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_channels(0),
	m_type(texture::TEXTURE_TYPE::TEXTURE_CUBE)
{
	RESULT r = R_PASS;

	CR(LoadCubeMapFromFiles(pszFilenameFront, pszFilenameBack, pszFilenameTop, pszFilenameBottom, pszFilenameLeft, pszFilenameRight));

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
	wchar_t *pFilePath = NULL;

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

RESULT texture::LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight) {
	RESULT r = R_PASS;
	wchar_t *pszFilenames[NUM_CUBE_MAP_TEXTURES] = { pszFilenameFront, pszFilenameBack, pszFilenameTop, pszFilenameBottom, pszFilenameLeft, pszFilenameRight };
	wchar_t *pszFilePaths[NUM_CUBE_MAP_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	uint8_t *pBuffers[NUM_CUBE_MAP_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	int widths[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };
	int heights[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };
	int channels[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		CR(GetTextureFilePath(pszFilenames[i], pszFilePaths[i]));
		CN(pszFilePaths[i]);

		std::wstring wstrFilepath(pszFilePaths[i]);
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
		std::string strFilepath = wstrConverter.to_bytes(wstrFilepath);

		pBuffers[i] = SOIL_load_image(strFilepath.c_str(), &widths[i], &heights[i], &channels[i], SOIL_LOAD_AUTO);
		CN(pBuffers[i]);

		// Ensure all heights are the same
		if(i == 0) {
			m_width = widths[i];
			m_height = heights[i];
		}
		else {
			CBM((m_width == widths[i]), "Cube map width %d mismatches %d", widths[i], m_width);
			CBM((m_height == heights[i]), "Cube map width %d mismatches %d", heights[i], m_height);
		}
	}

	// Stitch it together here
	size_t sizeSide = m_width * m_height * sizeof(unsigned char);
	size_t sizeTexture = sizeSide * NUM_CUBE_MAP_TEXTURES;
	m_pImageBuffer = new unsigned char[sizeTexture];
	CNM(m_pImageBuffer, "Failed to allocate Image Buffer for cube map");

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		unsigned char *ptrOffset = m_pImageBuffer + i * (sizeSide);
		memcpy(ptrOffset, pBuffers[i], sizeSide);
	}

Error:
	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
		if(pszFilePaths[i] != nullptr) {
			delete[] pszFilePaths[i];
			pszFilePaths[i] = nullptr;
		}

		if(pBuffers[i] != nullptr) {
			delete[] pBuffers[i];
			pBuffers[i] = nullptr;
		}
	}

	return r;
}
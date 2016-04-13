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
	m_channels(0),
	m_textureNumber(0)
{
	Validate();
}

texture::texture(wchar_t *pszFilename) :
	m_pImageBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_channels(0),
	m_textureNumber(0)
{
	RESULT r = R_PASS;

	CR(LoadTextureFromFile(pszFilename));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

texture::texture(texture *pTexture) :
	m_pImageBuffer(nullptr),
	m_width(pTexture->m_width),
	m_height(pTexture->m_height),
	m_channels(pTexture->m_channels),
	m_textureNumber(pTexture->m_textureNumber)
{
	if (pTexture->m_pImageBuffer != nullptr) {
		memcpy(m_pImageBuffer, pTexture->m_pImageBuffer, pTexture->m_channels * pTexture->m_height * pTexture->m_width * sizeof(unsigned char));
	}
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
	return SetTextureNumber(static_cast<int>(textureType));
}

int texture::GetTextureNumber() {
	return m_textureNumber;
}

texture::TEXTURE_TYPE texture::GetTextureType() {
	return static_cast<texture::TEXTURE_TYPE>(m_textureNumber);
}

RESULT texture::SetTextureNumber(int texNum) {
	m_textureNumber = texNum;
	return R_PASS;
}

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
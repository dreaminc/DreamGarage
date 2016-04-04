#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/texture.h
// Base type for texture

#include "valid.h"
#include "Types/UID.h"
#include "Sandbox/PathManager.h"

#include "External/SOIL/SOIL.h"

#include <string>
#include <locale>
#include <codecvt>
#include <utility>

class texture : public valid {
public:
	texture() :
		m_pImageBuffer(NULL),
		m_width(0),
		m_height(0),
		m_channels(0)
	{
		Validate();
	}

	texture(wchar_t *pszFilename) :
		m_pImageBuffer(NULL),
		m_width(0),
		m_height(0),
		m_channels(0)
	{
		RESULT r = R_PASS;

		CR(LoadTextureFromFile(pszFilename));

		Validate();
		return;
	Error:
		Invalidate();
		return;
	}

	~texture() {
		if (m_pImageBuffer != nullptr) {
			delete[] m_pImageBuffer;
			m_pImageBuffer = nullptr;
		}
	}

	RESULT GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath) {
		RESULT r = R_PASS;

		PathManager *pPathManager = PathManager::instance();
		wchar_t *pFilePath = NULL;

		// Move to key based file paths
		CRM(pPathManager->GetFilePath(PATH_TEXTURE, pszFilename, n_pszFilePath), "Failed to get path for %S texture", pszFilename);
		CN(n_pszFilePath);

		return r;

	Error:
		if (n_pszFilePath != nullptr) {
			delete [] n_pszFilePath;
			n_pszFilePath = nullptr;
		}
		return r;
	}

	RESULT FlipTextureVertical() {
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

	RESULT ReleaseTextureData() {
		RESULT r = R_PASS;

		SOIL_free_image_data(m_pImageBuffer);
		CB((m_pImageBuffer == nullptr));

	Error:
		return r;
	}

	RESULT LoadTextureFromPath(wchar_t *pszFilepath) {
		RESULT r = R_PASS;

		std::wstring wstrFilepath(pszFilepath);
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
		std::string strFilepath = wstrConverter.to_bytes(wstrFilepath);

		m_pImageBuffer = SOIL_load_image(strFilepath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		CN(m_pImageBuffer);

	Error:
		return r;
	}

	RESULT LoadTextureFromFile(wchar_t *pszFilename) {
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

protected:
	int m_width;
	int m_height;
	int m_channels;

	unsigned char *m_pImageBuffer;

private:
	UID m_uid;
};

#endif // ! TEXTURE_H_

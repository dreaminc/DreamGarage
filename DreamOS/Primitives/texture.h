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

class texture : public valid {
public:
	texture() {
		Validate();
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

	virtual RESULT InitializeFromFile(const wchar_t *pszFileName) = 0;

	// TODO: Add direct image loads
	RESULT LoadTextureFromFile(wchar_t *pszFilename) {
		return R_NOT_IMPLEMENTED;
	}

private:
	UID m_uid;
};

#endif // ! TEXTURE_H_

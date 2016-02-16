#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/texture.h
// Base type for texture

#include "valid.h"
#include "Types/UID.h"
#include "Sandbox/PathManager.h"

class texture : public valid {
public:
	texture(wchar_t *pszFilename) {
		RESULT r = R_PASS;
		
		// TODO: Load the texture
		CR(LoadTextureFromFile(pszFilename);

		Validate();
	Error:
		Invalidate();
	}

	RESULT LoadTextureFromFile(wchar_t *pszFilename) {
		RESULT r = R_PASS;

		// TODO: Solve this
		PathManager *pPathManager = pParentApp->GetPathManager();
		wchar_t *pFilePath = NULL;

	Error:
		return r;
	}

private:
	UID m_uid;
};

#endif // ! TEXTURE_H_

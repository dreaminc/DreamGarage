#ifndef OGL_MODEL_FACTORY_H_
#define OGL_MODEL_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLModelFactory.h
// This class will create and open a given file by name

#include "OGLObj.h"
#include "OGLModel.h"
//#include "OGLOBJModel.h"
#include "Sandbox\PathManager.h"
#include "Primitives\Types\UID.h"
#include <Primitives/OBJModel.h>

class OGLModelFactory {
private:
	OGLModelFactory()
	{
		// empty for now
	}

private:
	typedef enum {
		MODEL_OBJ = 0,
		MODEL_INVALID
	} MODEL_TYPE;

	const wchar_t *m_cszExtensions[MODEL_INVALID] = {
		L"OBJ",
	};

public:

	MODEL_TYPE GetModelTypeFromExtensionString(const wchar_t *pszExtension) {
		for (int i = 0; i < static_cast<int>(MODEL_INVALID); i++)
			if(wcscmp(pszExtension, m_cszExtensions[i]) == 0) 
				return static_cast<OGLModelFactory::MODEL_TYPE>(i);

		return MODEL_INVALID;
	}

	MODEL_TYPE GetModelTypeFromFilename(wchar_t *pszFilename) {
		MODEL_TYPE type = GetModelTypeFromExtensionString(PathManager::GetFilenameExtension(pszFilename).c_str());
		return type;
	}

	// TODO: Take implementation out of factory function and keep as member
	// TODO: Move to RESULT based arch
	VirtualObj *MakeOGLModel(OpenGLImp *pParentImp, wchar_t *pszFilename) {
		VirtualObj *pModel = nullptr;
		MODEL_TYPE type = GetModelTypeFromFilename(pszFilename);
		
		switch(type) {
			case OGLModelFactory::MODEL_OBJ: {
				OBJModel *pOBJModel = new OBJModel();

				RESULT r = pOBJModel->InitializeFromFile(pszFilename);
				if (r != R_PASS) {
					DEBUG_LINEOUT("OGLModelFactory Error: %S model failed to load", pszFilename);
					return nullptr;
				}

				// This is an assignment, no copy occurs but do not release pOBJModel since the memory is passed to pTempModel
				OGLModel *pTempModel = new OGLModel(pParentImp, pOBJModel);
				r = pTempModel->OGLInitialize();
				/*if (r != R_PASS) {
					DEBUG_LINEOUT("OGLModelFactory Error: %S model failed to load", pszFilename);
					return nullptr;
				}*/
				
				pModel = pTempModel;
				return pModel;
			} break;

			case OGLModelFactory::MODEL_INVALID: {
				DEBUG_LINEOUT("OGLModelFactory Error: %S is not a valid model extension", pszFilename);
				return nullptr;
			} break;
		}

		return pModel;
	}

	// Singleton Usage
protected:
	static OGLModelFactory *m_pInstance;

	/*
	static RESULT SetSingletonPathManager(OGLModelFactory *pInstance) {
		if (m_pInstance != nullptr) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}

		m_pInstance = pInstance;
		return R_PASS;
	}
	*/

public:
	static OGLModelFactory *instance() {
		if (m_pInstance == nullptr) {
			m_pInstance = new OGLModelFactory();
		}

		return m_pInstance;
	}

public:
	UID GetUID() {
		return m_uid;
	}

private:
	UID m_uid;
};

// Initialize and allocate the instance
OGLModelFactory* OGLModelFactory::m_pInstance = nullptr;

#endif	// ! OGL_MODEL_FACTORY_H

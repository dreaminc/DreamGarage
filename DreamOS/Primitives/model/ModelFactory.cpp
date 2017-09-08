#include "ModelFactory.h"

#include "Sandbox/PathManager.h"

// Add assimp

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "model.h"
#include "HAL/HALImp.h"

model* ModelFactory::MakeModel(HALImp *pParentImp, std::wstring wstrModelFilename) {
	RESULT r = R_PASS;

	model *pModel = nullptr;
	Assimp::Importer assetImporter;

	// Get file path
	// Root folder
	PathManager* pPathManager = PathManager::instance();
	std::wstring wstrModelFilePath;
	CRM(pPathManager->GetFilePath(PATH_MODEL, wstrModelFilename, wstrModelFilePath), "Failed to get model file path");
	CRM(pPathManager->DoesPathExist(wstrModelFilePath, true), "Model file path not found");

	// Load model from disk

	


// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}
#include "ModelFactory.h"

#include "Sandbox/PathManager.h"

// Add assimp

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "model.h"
#include "HAL/HALImp.h"

#include "Core/Utilities.h"

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
	const aiScene *pScene = assetImporter.ReadFile(util::WideStringToString(wstrModelFilePath), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

	CNM(pScene, "Asset Importer failed to allocate scene: %s", assetImporter.GetErrorString());
	CBM(((pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0), "Asset Importer Scene Incomplete: %s", assetImporter.GetErrorString());
	CNM(pScene->mRootNode, "Asset Importer scene root is null: %s", assetImporter.GetErrorString());


// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}
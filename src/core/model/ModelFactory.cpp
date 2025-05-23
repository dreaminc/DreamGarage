#include "ModelFactory.h"

#include "Sandbox/PathManager.h"

#include "os/DreamOS.h"

#include <vector>

// Add assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/color4.h"

#include "core/model/model.h"
#include "hal/HALImp.h"

#include "core/Utilities.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"
#include "core/primitives/Vertex.h"
#include "core/primitives/color.h"

std::vector<texture*> MakeTexturesFromAssetImporterMaterial(model *pModel, std::shared_ptr<mesh> pMesh, aiTextureType textureType, aiMaterial *pAIMaterial, const aiScene *pAIScene) {
	RESULT r = R_PASS;
	std::vector<texture*> retTextures;

	unsigned int nTextures = pAIMaterial->GetTextureCount(textureType);
	
	for (unsigned int i = 0; i < nTextures; i++) {
		
		aiString aistrTextureFilepath;
		pAIMaterial->GetTexture(textureType, i, &aistrTextureFilepath);

		// Make the texture

		// Automatically detect if absolute path, dream path, or local path
		std::wstring wstrFilename = pModel->GetModelDirectoryPath() + util::CStringToWideCString(aistrTextureFilepath.C_Str());
		texture* pTexture = pModel->MakeTextureRaw(texture::type::TEXTURE_2D, const_cast<wchar_t*>(wstrFilename.c_str()));
		CN(pTexture);

		retTextures.push_back(pTexture);
	}

	return retTextures;

Error:
	// Release memory on failure
	for (auto &pTexture : retTextures) {
		if (pTexture != nullptr) {
			delete pTexture;
			pTexture = nullptr;
		}
	}

	retTextures.clear();

	return std::vector<texture*>();
}

std::vector<std::wstring> GetTexturePaths(model *pModel, aiTextureType textureType, aiMaterial *pAIMaterial) {
	RESULT r = R_PASS;

	std::vector<std::wstring> texturePaths = std::vector<std::wstring>();

	unsigned int nTextures = pAIMaterial->GetTextureCount(textureType);

	for (unsigned int i = 0; i < nTextures; i++) {
		aiString aistrTextureFilepath;
		pAIMaterial->GetTexture(textureType, i, &aistrTextureFilepath);

		// Automatically detect if absolute path, dream path, or local path
		std::wstring wstrFilename = pModel->GetModelDirectoryPath() + util::CStringToWideCString(aistrTextureFilepath.C_Str());
		texturePaths.push_back(wstrFilename);
	}

Success:
	return texturePaths;

	// redundant
Error:
	return std::vector<std::wstring>();
}

RESULT ProcessAssetImporterMeshMaterial(model *pModel, aiMaterial *pAIMaterial, const aiScene *pAIScene, mesh::params *pMeshParams) {
	RESULT r = R_PASS;

	aiColor4D aic;

	float aiShininess;
	float aiBumpscale;

	unsigned int retArraySize = 0;

	//float aiStrength;
	//float aiMax;

	color c;

	// Diffuse Color
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_DIFFUSE, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMeshParams->meshMaterial.SetDiffuseColor(c);
	}

	// Specular Color
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_SPECULAR, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMeshParams->meshMaterial.SetSpecularColor(c);
	}

	// Ambient Color
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_AMBIENT, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMeshParams->meshMaterial.SetAmbientColor(c);
	}

	// TODO: Emission Color
	/*
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_EMISSIVE, &aic) == AI_SUCCESS) {
	c = color(aic.r, aic.g, aic.b, aic.a);
	pMesh->SetMaterialEmissiveColor(c, false);
	}
	*/

	// Shininess 
	if (aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_SHININESS, &aiShininess, &retArraySize) == AI_SUCCESS && retArraySize != 0) {
		pMeshParams->meshMaterial.SetShininess(aiShininess);
	}

	// Bumpiness
	if (aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_BUMPSCALING, &aiBumpscale, &retArraySize) == AI_SUCCESS && retArraySize != 0) {
		pMeshParams->meshMaterial.SetBumpiness(aiBumpscale);
	}

	// Textures
	// TODO: Support more than one texture
	// TODO: Bump maps
	if (pAIMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		pMeshParams->diffuseTexturePaths = GetTexturePaths(pModel, aiTextureType_DIFFUSE, pAIMaterial);
	}

	if (pAIMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		pMeshParams->specularTexturePaths = GetTexturePaths(pModel, aiTextureType_SPECULAR, pAIMaterial);
	}

	// TODO: This may or may not be a bug with assimp
	//if (pAIMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
	if (pAIMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0) {
		//pMeshParams->specularTexturePaths = GetTexturePaths(pModel, aiTextureType_NORMALS, pAIMaterial);
		pMeshParams->normalsTexturePaths = GetTexturePaths(pModel, aiTextureType_HEIGHT, pAIMaterial);
	}

	if (pAIMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0) {
		pMeshParams->ambientTexturePaths = GetTexturePaths(pModel, aiTextureType_AMBIENT, pAIMaterial);
	}

Error:
	return r;
}

RESULT ProcessAssetImporterMeshMaterial(model *pModel, std::shared_ptr<mesh> pMesh, aiMaterial *pAIMaterial, const aiScene *pAIScene) {
	RESULT r = R_PASS;

	aiColor4D aic;

	float aiShininess;
	float aiBumpscale;

	unsigned int retArraySize = 0;

	//float aiStrength;
	//float aiMax;

	color c;

	// Diffuse Color
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_DIFFUSE, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMesh->SetMaterialDiffuseColor(c, false);
	}

	// Specular Color
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_SPECULAR, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMesh->SetMaterialSpecularColor(c, false);
	}

	// Ambient Color
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_AMBIENT, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMesh->SetMaterialAmbientColor(c, false);
	}

	// TODO: Emission Color
	/*
	if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_EMISSIVE, &aic) == AI_SUCCESS) {
		c = color(aic.r, aic.g, aic.b, aic.a);
		pMesh->SetMaterialEmissiveColor(c, false);
	}
	*/

	// Shininess 
	if (aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_SHININESS, &aiShininess, &retArraySize) == AI_SUCCESS && retArraySize != 0) {
		pMesh->SetMaterialShininess(aiShininess, false);
	}

	// Bumpiness
	if (aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_BUMPSCALING, &aiBumpscale, &retArraySize) == AI_SUCCESS && retArraySize != 0) {
		pMesh->SetMaterialBumpiness(aiBumpscale, false);
	}

	// Textures
	// TODO: Support more than one texture
	// TODO: Bump maps
	if (pAIMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		auto diffuseTextures = MakeTexturesFromAssetImporterMaterial(pModel, pMesh, aiTextureType_DIFFUSE, pAIMaterial, pAIScene);

		if (diffuseTextures.size() > 0) {
			pMesh->SetDiffuseTexture(diffuseTextures[0]);
		}
	}

	if (pAIMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		auto specularTextures = MakeTexturesFromAssetImporterMaterial(pModel, pMesh, aiTextureType_SPECULAR, pAIMaterial, pAIScene);

		if (specularTextures.size() > 0) {
			pMesh->SetSpecularTexture(specularTextures[0]);
		}
	}

	// TODO: This may or may not be a bug with assimp
	//if (pAIMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
	if (pAIMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0) {
		//auto bumpTextures = MakeTexturesFromAssetImporterMaterial(pModel, pMesh, aiTextureType_NORMALS, pAIMaterial, pAIScene);
		auto bumpTextures = MakeTexturesFromAssetImporterMaterial(pModel, pMesh, aiTextureType_HEIGHT, pAIMaterial, pAIScene);

		if (bumpTextures.size() > 0) {
			pMesh->SetBumpTexture(bumpTextures[0]);
		}
	}

	if (pAIMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0) {
		auto ambientTextures = MakeTexturesFromAssetImporterMaterial(pModel, pMesh, aiTextureType_AMBIENT, pAIMaterial, pAIScene);

		if (ambientTextures.size() > 0) {
			pMesh->SetAmbientTexture(ambientTextures[0]);
		}
	}

Error:
	return r;
}

RESULT ProcessAssetImporterMesh(model *pModel, aiMesh *pAIMesh, const aiScene *pAIScene, bool fLoadAsync = false) {
	RESULT r = R_PASS;

	std::vector<vertex> vertices;
	std::vector<dimindex> indices;
	//std::vector<texture> textures;
	
	std::string strMeshName = "mesh";

	for (unsigned int i = 0; i < pAIMesh->mNumVertices; i++) {
		vertex vert;

		// temp
		vert.SetColor(color(COLOR_WHITE));

		// Point
		if (pAIMesh->mVertices != nullptr) {
			point ptVert;
			ptVert.x() = pAIMesh->mVertices[i].x;
			ptVert.y() = pAIMesh->mVertices[i].y;
			ptVert.z() = pAIMesh->mVertices[i].z;
			vert.SetPoint(ptVert);
		}

		// Color
		// TODO: Handle color sets (AI_MAX_NUMBER_OF_COLOR_SETS)

		int numColors = pAIMesh->GetNumColorChannels();

		for (int j = 0; j < numColors; j++) {
			if (pAIMesh->HasVertexColors(j)) {
				color colorVert;
				colorVert.r() = pAIMesh->mColors[j][i].r;
				colorVert.g() = pAIMesh->mColors[j][i].g;
				colorVert.b() = pAIMesh->mColors[j][i].b;
				colorVert.a() = pAIMesh->mColors[j][i].a;
				vert.SetColor(colorVert);
			}
		}
		
		//if (pAIMesh->mColors != nullptr) {
		//	color colorVert;
		//	colorVert.r() = pAIMesh->mColors[i][0].r;
		//	colorVert.g() = pAIMesh->mColors[i][0].g;
		//	colorVert.b() = pAIMesh->mColors[i][0].b;
		//	colorVert.a() = pAIMesh->mColors[i][0].a;
		//	vert.SetColor(colorVert);
		//}

		// Normal
		if (pAIMesh->mNormals != nullptr) {
			vector vNormal;
			vNormal.x() = pAIMesh->mNormals[i].x;
			vNormal.y() = pAIMesh->mNormals[i].y;
			vNormal.z() = pAIMesh->mNormals[i].z;
			vert.SetNormal(vNormal);
		}

		// Tangents
		if (pAIMesh->mTangents != nullptr) {
			vector vTangent;
			vTangent.x() = pAIMesh->mTangents[i].x;
			vTangent.y() = pAIMesh->mTangents[i].y;
			vTangent.z() = pAIMesh->mTangents[i].z;
			vert.SetTangent(vTangent);
		}

		// Bitangents
		if (pAIMesh->mBitangents != nullptr) {
			vector vBitangent;
			vBitangent.x() = pAIMesh->mBitangents[i].x;
			vBitangent.y() = pAIMesh->mBitangents[i].y;
			vBitangent.z() = pAIMesh->mBitangents[i].z;
			vert.SetBitangent(vBitangent);
		}

		// UV 
		// TODO: Support multi-textures (assimp supports up to 8 textures)
		if (pAIMesh->mTextureCoords[0] != nullptr) {
			float u = std::abs(pAIMesh->mTextureCoords[0][i].x);
			float v = std::abs(pAIMesh->mTextureCoords[0][i].y);

			if (u > 1.0f) {
				u = u - (int)(u);
			}

			if (v > 1.0f) {
				v = v - (int)(v);
			}

			vert.SetUV(u, v);
		}

		vertices.push_back(vert);
	}

	// Indices
	for (unsigned int i = 0; i < pAIMesh->mNumFaces; i++) {
		aiFace face = pAIMesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back((dimindex)(face.mIndices[j]));
		}
	}
	
	if (fLoadAsync == false) {
		// Create the mesh and add to model
		std::shared_ptr<mesh> pMesh = pModel->AddMesh(vertices, indices);
		CN(pMesh);

		// Materials 
		if (pAIScene->mNumMaterials > 0) {
			aiMaterial *pAIMaterial = pAIScene->mMaterials[pAIMesh->mMaterialIndex];
			CRM(ProcessAssetImporterMeshMaterial(pModel, pMesh, pAIMaterial, pAIScene), "Failed to process material for mesh");
		}
	}
	else {
		mesh::params meshParams(strMeshName, vertices, indices);

		// Materials 
		if (pAIScene->mNumMaterials > 0) {
			aiMaterial *pAIMaterial = pAIScene->mMaterials[pAIMesh->mMaterialIndex];
			CRM(ProcessAssetImporterMeshMaterial(pModel, pAIMaterial, pAIScene, &meshParams), 
				"Failed to process material for mesh");
		}

		// This will queue the mesh which will get added to the model
		CR(pModel->QueueMesh(meshParams));
	}


Error:
	return r;
}

RESULT ProcessAssetImporterNode(model *pModel, aiNode *pAINode, const aiScene *pAIScene, bool fLoadAsync = false) {
	RESULT r = R_PASS;

	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < pAINode->mNumMeshes; i++) {
		aiMesh *pAIMesh = pAIScene->mMeshes[pAINode->mMeshes[i]];
		
		//meshes.push_back(ProcessAssetImporterMesh(pModel, pAIMesh, pAIScene));

		CR(ProcessAssetImporterMesh(pModel, pAIMesh, pAIScene, fLoadAsync));
	}

	// Then do the same for each of its children (just gets added flat - no model heirarchy transferred)
	for (unsigned int i = 0; i < pAINode->mNumChildren; i++) {
		ProcessAssetImporterNode(pModel, pAINode->mChildren[i], pAIScene, fLoadAsync);
	}

Error:
	return r;
}

model *MakeAndInitializeModel(HALImp *pParentImp, std::wstring wstrModelFilename) {
	RESULT r = R_PASS;

	model *pModel = nullptr;

	// Get file path
	// Root folder
	PathManager* pPathManager = PathManager::instance();
	std::wstring wstrModelFilePath;

	if (!pPathManager->IsAbsolutePath(&wstrModelFilename[0])) {
		CRM(pPathManager->GetFilePath(PATH_MODEL, wstrModelFilename, wstrModelFilePath), "Failed to get model file path");
		CRM(pPathManager->DoesPathExist(wstrModelFilePath, true), "Model file path not found");
	}
	else if (pPathManager->IsDreamPath(&wstrModelFilename[0]) == true) {
		CRM(pPathManager->GetFilePath(wstrModelFilename, wstrModelFilePath), "Failed to get model file path");
		CRM(pPathManager->DoesPathExist(wstrModelFilePath, true), "Model file path not found");
	}
	else {
		wstrModelFilePath = wstrModelFilename;
	}

	pModel = pParentImp->MakeModel();
	CN(pModel);

	//CR(pModel->OGLInitialize());
	CR(pModel->InitializeOBB());
	//CR(pModel->InitializeBoundingSphere());

	// Set the path (used in texture loading)
	CR(pModel->SetModelFilePath(wstrModelFilePath));

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

unsigned int GetAssImpFlags(ModelFactory::flags modelFactoryFlags) {
	unsigned int assimpFlags = //aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenNormals |
		//aiProcess_FixInfacingNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		//aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate;

	if ((modelFactoryFlags & ModelFactory::flags::FLIP_WINDING) != 0) {
		assimpFlags |= aiProcess_FlipWindingOrder;
	}

	return assimpFlags;
}

model* ModelFactory::MakeModel(HALImp *pParentImp, std::wstring wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	RESULT r = R_PASS;

	model *pModel = nullptr;
	Assimp::Importer assetImporter;

	pModel = MakeAndInitializeModel(pParentImp, wstrModelFilename);
	CNM(pModel, "Failed to load model");

	// Load model from disk
	const aiScene *pAIScene = assetImporter.ReadFile(util::WideStringToString(pModel->GetModelFilePath()), GetAssImpFlags(modelFactoryFlags));
	CNM(pAIScene, "Asset Importer failed to allocate scene: %s", assetImporter.GetErrorString());
	CBM(((pAIScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0), "Asset Importer Scene Incomplete: %s", assetImporter.GetErrorString());
	CNM(pAIScene->mRootNode, "Asset Importer scene root is null: %s", assetImporter.GetErrorString());

	CRM(ProcessAssetImporterNode(pModel, pAIScene->mRootNode, pAIScene, false), "Failed to process Asset Importer root node");

	//pModel->UpdateBoundingVolume();

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

model *ModelFactory::MakeModel(DreamOS *pDOS, PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	model *pModel = nullptr;
	Assimp::Importer assetImporter;

	model::params *pModelParams = dynamic_cast<model::params*>(pPrimParams);
	CN(pModelParams);

	pModel = MakeAndInitializeModel(pDOS->GetHALImp(), pModelParams->wstrModelFilePath);
	CNM(pModel, "Failed to load model");
	CR(pModel->SetDreamOS(pDOS));

	// Load model from disk
	const aiScene *pAIScene = assetImporter.ReadFile(util::WideStringToString(pModel->GetModelFilePath()), GetAssImpFlags(pModelParams->modelFactoryFlags));
	CNM(pAIScene, "Asset Importer failed to allocate scene: %s", assetImporter.GetErrorString());
	CBM(((pAIScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0), "Asset Importer Scene Incomplete: %s", assetImporter.GetErrorString());
	CNM(pAIScene->mRootNode, "Asset Importer scene root is null: %s", assetImporter.GetErrorString());

	CRM(ProcessAssetImporterNode(pModel, pAIScene->mRootNode, pAIScene, true), "Failed to process Asset Importer root node");

	//pModel->UpdateBoundingVolume();

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}
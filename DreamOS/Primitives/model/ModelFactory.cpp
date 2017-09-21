#include "ModelFactory.h"

#include "Sandbox/PathManager.h"

// Add assimp

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/color4.h"

#include "model.h"
#include "HAL/HALImp.h"

#include "Core/Utilities.h"

#include <vector>

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/Vertex.h"

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
		texture* pTexture = pModel->MakeTextureRaw(const_cast<wchar_t*>(wstrFilename.c_str()), texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
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

//Error:
	return r;
}

RESULT ProcessAssetImporterMesh(model *pModel, aiMesh *pAIMesh, const aiScene *pAIScene) {
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
		point ptVert; 
		ptVert.x() = pAIMesh->mVertices[i].x;
		ptVert.y() = pAIMesh->mVertices[i].y;
		ptVert.z() = pAIMesh->mVertices[i].z;
		vert.SetPoint(ptVert);

		// Normal
		vector vNormal;
		vNormal.x() = pAIMesh->mNormals[i].x;
		vNormal.y() = pAIMesh->mNormals[i].y;
		vNormal.z() = pAIMesh->mNormals[i].z;
		vert.SetNormal(vNormal);

		// Tangents
		//vector vTangent;
		//vTangent.x() = pAIMesh->mTangents[i].x;
		//vTangent.y() = pAIMesh->mTangents[i].y;
		//vTangent.z() = pAIMesh->mTangents[i].z;
		//vert.SetTangent(vTangent);
		//
		//// Bitangents
		//vector vBitangent;
		//vBitangent.x() = pAIMesh->mBitangents[i].x;
		//vBitangent.y() = pAIMesh->mBitangents[i].y;
		//vBitangent.z() = pAIMesh->mBitangents[i].z;
		//vert.SetBitangent(vBitangent);

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
	
	// Create the mesh and add to model
	std::shared_ptr<mesh> pMesh = pModel->AddMesh(vertices, indices);
	CN(pMesh);

	// Materials 
	if (pAIMesh->mMaterialIndex != 0) {
		aiMaterial *pAIMaterial = pAIScene->mMaterials[pAIMesh->mMaterialIndex];
		CRM(ProcessAssetImporterMeshMaterial(pModel, pMesh, pAIMaterial, pAIScene), "Failed to process material for mesh");
	}


Error:
	return r;
}

RESULT ProcessAssetImporterNode(model *pModel, aiNode *pAINode, const aiScene *pAIScene) {
	RESULT r = R_PASS;

	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < pAINode->mNumMeshes; i++) {
		aiMesh *pAIMesh = pAIScene->mMeshes[pAINode->mMeshes[i]];
		
		//meshes.push_back(ProcessAssetImporterMesh(pModel, pAIMesh, pAIScene));

		CR(ProcessAssetImporterMesh(pModel, pAIMesh, pAIScene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < pAINode->mNumChildren; i++) {
		ProcessAssetImporterNode(pModel, pAINode->mChildren[i], pAIScene);
	}

Error:
	return r;
}

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
	
	//pModel = new model(pParentImp);
	pModel = pParentImp->MakeModel();
	CN(pModel);
	CR(pModel->InitializeOBB());
	//CR(pModel->InitializeBoundingSphere());

	// Set the path (used in texture loading)
	CR(pModel->SetModelFilePath(wstrModelFilePath));

	// Load model from disk
	const aiScene *pAIScene =
		assetImporter.ReadFile(util::WideStringToString(wstrModelFilePath),
			//aiProcess_FlipUVs |
			//aiProcess_CalcTangentSpace |
			aiProcess_GenNormals | 
			aiProcess_PreTransformVertices |
			aiProcess_Triangulate);

	CNM(pAIScene, "Asset Importer failed to allocate scene: %s", assetImporter.GetErrorString());
	CBM(((pAIScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0), "Asset Importer Scene Incomplete: %s", assetImporter.GetErrorString());
	CNM(pAIScene->mRootNode, "Asset Importer scene root is null: %s", assetImporter.GetErrorString());

	CRM(ProcessAssetImporterNode(pModel, pAIScene->mRootNode, pAIScene), "Failed to process Asset Importer root node");

	//pModel->UpdateBoundingVolume();

// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}
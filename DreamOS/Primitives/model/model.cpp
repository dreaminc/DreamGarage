#include "model.h"

#include "DreamOS.h"
#include "HAL/HALImp.h"

#include "Sandbox/PathManager.h"

model::model(HALImp *pParentImp) :
	composite(pParentImp),
	m_params(L"")
{
	RESULT r = R_PASS;

	// Bounding Box
	//CR(InitializeBoundingSphere());
	//CR(InitializeOBB());

Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

std::shared_ptr<mesh> model::AddMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh = MakeMesh(vertices);
	CR(AddObject(pMesh));

Success:
	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

std::shared_ptr<mesh> model::MakeMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh(m_pHALImp->MakeMesh(vertices));
	CN(pMesh);

Success:
	return pMesh;

Error:
	if (pMesh != nullptr) {
		pMesh = nullptr;
	}

	return nullptr;
}

std::shared_ptr<mesh> model::AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh = MakeMesh(vertices, indices);
	CN(pMesh);
	CR(AddObject(pMesh));

Success:
	return pMesh;

Error:
	if (pMesh != nullptr) {
		pMesh = nullptr;
	}

	return nullptr;
}

// Note:  I don't love the code duplication, but it's mostly sanity check code and a 
// wrap of the set X texture method for the mesh.  Better than messing with the async obj arch
RESULT model::HandleOnMeshDiffuseTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetDiffuseTexture(pTexture));
	
	// Remove from pending textures
	for (auto &wstrPendingTexturePath : m_pendingTextures) {
		if (wcscmp(wstrPendingTexturePath.c_str(), pTexture->m_params.pszFilename) == 0) {
			m_pendingTextures.erase(std::remove(m_pendingTextures.begin(), m_pendingTextures.end(), wstrPendingTexturePath), m_pendingTextures.end());
			break;
		}
	}

Error:
	return r;
}

RESULT model::HandleOnMeshSpecularTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetSpecularTexture(pTexture));
	
	// Remove from pending textures
	for (auto &wstrPendingTexturePath : m_pendingTextures) {
		if (wcscmp(wstrPendingTexturePath.c_str(), pTexture->m_params.pszFilename) == 0) {
			m_pendingTextures.erase(std::remove(m_pendingTextures.begin(), m_pendingTextures.end(), wstrPendingTexturePath), m_pendingTextures.end());
			break;
		}
	}

Error:
	return r;
}

RESULT model::HandleOnMeshNormalTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetBumpTexture(pTexture));

	// Remove from pending textures
	for (auto &wstrPendingTexturePath : m_pendingTextures) {
		if (wcscmp(wstrPendingTexturePath.c_str(), pTexture->m_params.pszFilename) == 0) {
			m_pendingTextures.erase(std::remove(m_pendingTextures.begin(), m_pendingTextures.end(), wstrPendingTexturePath), m_pendingTextures.end());
			break;
		}
	}

Error:
	return r;
}

RESULT model::HandleOnMeshAmbientTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetAmbientTexture(pTexture));

	// Remove from pending textures
	for (auto &wstrPendingTexturePath : m_pendingTextures) {
		if (wcscmp(wstrPendingTexturePath.c_str(), pTexture->m_params.pszFilename) == 0) {
			m_pendingTextures.erase(std::remove(m_pendingTextures.begin(), m_pendingTextures.end(), wstrPendingTexturePath), m_pendingTextures.end());
			break;
		}
	}

Error:
	return r;
}

RESULT model::HandleOnMeshReady(DimObj* pDimObj, void *pContext) {
	RESULT r = R_PASS;

	CNM(pDimObj, "Incoming DimObj invalid");

	mesh *pMesh = dynamic_cast<mesh*>(pDimObj);
	CNM(pMesh, "Incoming mesh invalid");

	// Mesh has been loaded and added to GPU 
	CRM(AddObject(std::shared_ptr<mesh>(pMesh)), "Failed to add mesh to model");

	CNM(m_pDreamOS, "DreamOS handle must be initialized for async object creation");
	
	// Textures

	// Diffuse 
	if (pMesh->m_params.diffuseTexturePaths.size() > 0) {
		std::function<RESULT(texture*, void*)> fnHandleOnMeshDiffuseTextureReady =
			std::bind(&model::HandleOnMeshDiffuseTextureReady, this, std::placeholders::_1, std::placeholders::_2);

		CRM(m_pDreamOS->LoadTexture(
			fnHandleOnMeshDiffuseTextureReady,
			(void*)(pMesh),
			texture::type::TEXTURE_2D,
			pMesh->m_params.diffuseTexturePaths[0].c_str()
		), "Failed to load mesh diffuse texture");

		// Push it, push it good
		m_pendingTextures.push_back(pMesh->m_params.diffuseTexturePaths[0]);
	}

	// Specular
	if (pMesh->m_params.specularTexturePaths.size() > 0) {
		std::function<RESULT(texture*, void*)> fnHandleOnMeshSpecularTextureReady =
			std::bind(&model::HandleOnMeshSpecularTextureReady, this, std::placeholders::_1, std::placeholders::_2);

		CRM(m_pDreamOS->LoadTexture(
			fnHandleOnMeshSpecularTextureReady,
			(void*)(pMesh),
			texture::type::TEXTURE_2D,
			pMesh->m_params.specularTexturePaths[0].c_str()
		), "Failed to load mesh specular texture");

		// Push it, push it good
		m_pendingTextures.push_back(pMesh->m_params.specularTexturePaths[0]);
	}

	// Normal Map 
	if (pMesh->m_params.normalsTexturePaths.size() > 0) {
		std::function<RESULT(texture*, void*)> fnHandleOnMeshNormalTextureReady =
			std::bind(&model::HandleOnMeshNormalTextureReady, this, std::placeholders::_1, std::placeholders::_2);

		CRM(m_pDreamOS->LoadTexture(
			fnHandleOnMeshNormalTextureReady,
			(void*)(pMesh),
			texture::type::TEXTURE_2D,
			pMesh->m_params.normalsTexturePaths[0].c_str()
		), "Failed to load mesh normal map texture");

		// Push it, push it good
		m_pendingTextures.push_back(pMesh->m_params.normalsTexturePaths[0]);
	}

	// Ambient 
	if (pMesh->m_params.ambientTexturePaths.size() > 0) {
		std::function<RESULT(texture*, void*)> fnHandleOnMeshAmbientTextureReady =
			std::bind(&model::HandleOnMeshAmbientTextureReady, this, std::placeholders::_1, std::placeholders::_2);

		CRM(m_pDreamOS->LoadTexture(
			fnHandleOnMeshAmbientTextureReady,
			(void*)(pMesh),
			texture::type::TEXTURE_2D,
			pMesh->m_params.ambientTexturePaths[0].c_str()
		), "Failed to load mesh ambient texture");

		// Push it, push it good
		m_pendingTextures.push_back(pMesh->m_params.ambientTexturePaths[0]);
	}

	if (pContext != nullptr) {
		unsigned long *pPendingMeshID = (unsigned long*)(pContext);
		CN(pPendingMeshID);

		CBM((std::find(m_pendingMeshIDs.begin(), m_pendingMeshIDs.end(), *pPendingMeshID) != m_pendingMeshIDs.end()), 
			"Couldn't find pending mesh ID %d", *pPendingMeshID);

		m_pendingMeshIDs.erase(std::remove(m_pendingMeshIDs.begin(), m_pendingMeshIDs.end(), *pPendingMeshID), m_pendingMeshIDs.end());
	}

Error:
	return r;
}



RESULT model::QueueMesh(const mesh::params &meshParams) {
	RESULT r = R_PASS;
	
	static unsigned long _meshID = 0;

	std::function<RESULT(DimObj*, void*)> fnHandleOnMeshReady =
		std::bind(&model::HandleOnMeshReady, this, std::placeholders::_1, std::placeholders::_2);

	unsigned long *pPendingMeshID = new unsigned long;
	CN(pPendingMeshID);

	*pPendingMeshID = ++_meshID;

	CNM(m_pDreamOS, "DreamOS handle must be initialized for async object creation");
	CRM(m_pDreamOS->MakeMesh(
		fnHandleOnMeshReady,			// fnHandler
		meshParams,						// mesh params
		(void*)pPendingMeshID					// context
	), "Failed to queue mesh %s", meshParams.strName.c_str());

	// Push it, push it good
	m_pendingMeshIDs.push_back(*pPendingMeshID);

Error:
	return r;
}

bool model::IsModelLoaded() {
	if (m_pendingMeshIDs.empty() == true && m_pendingTextures.empty() == true) {
		return true;
	}
	else {
		return false;
	}
}

std::shared_ptr<mesh> model::GetChildMesh(int index) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh = nullptr;

	auto childrenVector = GetChildren();

	CB(childrenVector.size() > index);

	{
		std::shared_ptr<VirtualObj> pVirtualObj = childrenVector[index];

		pMesh = std::dynamic_pointer_cast<mesh>(pVirtualObj);
		CN(pMesh);
	}

	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

std::shared_ptr<mesh> model::MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh(m_pHALImp->MakeMesh(vertices, indices));
	CN(pMesh);

Success:
	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

RESULT model::SetModelFilePath(std::wstring wstrFilepath) {
	m_params.wstrModelFilePath = wstrFilepath;

	// Set the directory path
	PathManager* pPathManager = PathManager::instance();
	m_params.wstrModelDirectoryPath = pPathManager->GetDirectoryPathFromFilePath(m_params.wstrModelFilePath);

	return R_PASS;
}
std::wstring model::GetModelFilePath() {
	return m_params.wstrModelFilePath;
}

std::wstring model::GetModelDirectoryPath() {
	return m_params.wstrModelDirectoryPath;
}

RESULT model::SetDreamOS(DreamOS *pDOS) {
	RESULT r = R_PASS;

	CN(pDOS);
	m_pDreamOS = pDOS;

Error:
	return r;
}
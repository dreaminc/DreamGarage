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

Error:
	return r;
}

RESULT model::HandleOnMeshSpecularTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetSpecularTexture(pTexture));

Error:
	return r;
}

RESULT model::HandleOnMeshNormalTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetBumpTexture(pTexture));

Error:
	return r;
}

RESULT model::HandleOnMeshAmbientTextureReady(texture *pTexture, void *pContext) {
	RESULT r = R_PASS;

	mesh *pMesh = (mesh*)(pContext);
	CN(pMesh);
	CN(pTexture);

	CR(pMesh->SetAmbientTexture(pTexture));

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
	}

Error:
	return r;
}

RESULT model::QueueMesh(const mesh::params &meshParams) {
	RESULT r = R_PASS;

	std::function<RESULT(DimObj*, void*)> fnHandleOnMeshReady =
		std::bind(&model::HandleOnMeshReady, this, std::placeholders::_1, std::placeholders::_2);

	CNM(m_pDreamOS, "DreamOS handle must be initialized for async object creation");
	CRM(m_pDreamOS->MakeMesh(
		fnHandleOnMeshReady,		// fnHandler
		meshParams,					// mesh params
		nullptr						// context
	), "Failed to queue mesh %s", meshParams.strName.c_str());

Error:
	return r;
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
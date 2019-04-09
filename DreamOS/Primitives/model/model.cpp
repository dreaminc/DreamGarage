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

RESULT model::HandleOnMeshReady(DimObj* pDimObj, void *pContext) {
	RESULT r = R_PASS;

	CNM(pDimObj, "Incoming DimObj invalid");

	mesh *pMesh = dynamic_cast<mesh*>(pDimObj);
	CNM(pMesh, "Incoming mesh invalid");

	// Mesh has been loaded and added to GPU 
	CRM(AddObject(std::shared_ptr<mesh>(pMesh)), "Failed to add mesh to model");

	// TODO: Textures

Error:
	return r;
}

RESULT model::QueueMesh(std::string strName, const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	std::function<RESULT(DimObj*, void*)> fnHandleOnMeshReady =
		std::bind(&model::HandleOnMeshReady, this, std::placeholders::_1, std::placeholders::_2);

	CNM(m_pDreamOS, "DreamObjectModule not initialized");
	CRM(m_pDreamOS->MakeMesh(
		fnHandleOnMeshReady,		// fnHandler
		strName,					// name
		vertices,					// verts
		indices,					// indices
		nullptr						// context
	), "Failed to queue mesh %s", strName.c_str());

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
#include "model.h"

#include "HAL/HALImp.h"

#include "Sandbox/PathManager.h"

model::model(HALImp *pParentImp) :
	composite(pParentImp)
{
	RESULT r = R_PASS;

	// Bounding Box
	//CR(InitializeBoundingSphere());
	//CR(InitializeOBB());

	Validate();
	return;

//Error:
//	Invalidate();
//	return;
}

std::shared_ptr<mesh> model::AddMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh = MakeMesh(vertices);
	CR(AddObject(pMesh));

	//Success:
	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

std::shared_ptr<mesh> model::MakeMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh(m_pHALImp->MakeMesh(vertices));
	CN(pMesh);

	//Success:
	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

std::shared_ptr<mesh> model::AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh = MakeMesh(vertices, indices);
	CR(AddObject(pMesh));

//Success:
	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

std::shared_ptr<mesh> model::MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	std::shared_ptr<mesh> pMesh(m_pHALImp->MakeMesh(vertices, indices));
	CN(pMesh);

//Success:
	return pMesh;

Error:
	pMesh = nullptr;
	return nullptr;
}

RESULT model::SetModelFilePath(std::wstring wstrFilepath) {
	m_wstrModelFilePath = wstrFilepath;

	// Set the directory path
	PathManager* pPathManager = PathManager::instance();
	m_wstModelDirectoryPath = pPathManager->GetDirectoryPathFromFilePath(m_wstrModelFilePath);

	return R_PASS;
}
std::wstring model::GetModelFilePath() {
	return m_wstrModelFilePath;
}

std::wstring model::GetModelDirectoryPath() {
	return m_wstModelDirectoryPath;
}
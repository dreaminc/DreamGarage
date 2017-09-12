#include "model.h"

#include "HAL/HALImp.h"

model::model(HALImp *pParentImp) :
	composite(pParentImp)
{
	// empty
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
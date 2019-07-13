#include "mesh.h"

#include "Sandbox/PathManager.h"

/*
mesh::mesh(wchar_t *pszModelName) {
	RESULT r = R_PASS;

	std::vector<vertex> vertices;
	wchar_t *pszFilePath = nullptr;
	std::wstring objFile;

	PathManager *pPathManager = PathManager::instance();
	CRM(pPathManager->GetFilePath(PATH_MODEL, pszModelName, pszFilePath), "Failed to get path for %S model", pszModelName);
	CN(pszFilePath);
	objFile = std::wstring(pszFilePath);
	delete[] pszFilePath;
	pszFilePath = nullptr;

	//FileLoaderHelper::LoadOBJFile(objFile, vertices);

	// TODO: This is a stop gap approach, this should move to manipulating the verts/indices of the DimObj directly
	// TODO: to avoid mem duplication
	CR(SetVertices(vertices));

	//	Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}
*/

mesh::mesh(mesh::params *pMeshParams) :
	m_params(*pMeshParams)
{
	RESULT r = R_PASS;

	m_nIndices = static_cast<unsigned int>(pMeshParams->indices.size());
	m_nVertices = static_cast<unsigned int>(pMeshParams->vertices.size());

	CR(Allocate());

	unsigned int indexCount = 0;

	for (auto& v : pMeshParams->vertices) {
		m_pVertices[indexCount] = vertex(v);
		m_pVertices[indexCount].SetTangentBitangentFromNormal();

		indexCount++;
	}

	indexCount = 0;

	for (auto& i : pMeshParams->indices) {
		m_pIndices[indexCount++] = i;
	}

	// Bounding Box
	//CR(InitializeBoundingSphere());
	CR(InitializeOBB());

	// Apply Materials
	CR(SetMaterial(pMeshParams->meshMaterial));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

mesh::mesh(const std::vector<vertex>& vertices) :
	m_params("mesh")
{
	RESULT r = R_PASS;

	// init a model with index for each vertex
	m_nIndices = m_nVertices;

	CR(SetVertices(vertices));

	// Bounding Box
	//CR(InitializeBoundingSphere());
	CR(InitializeOBB());

	Validate();
	return;

Error:
	Invalidate();
	return;
}

mesh::mesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) :
	m_params("mesh") 
{
	RESULT r = R_PASS;

	m_nIndices = static_cast<unsigned int>(indices.size());
	m_nVertices = static_cast<unsigned int>(vertices.size());

	CR(Allocate());

	unsigned int indexCount = 0;

	for (auto& v : vertices) {
		m_pVertices[indexCount] = vertex(v);
		m_pVertices[indexCount].SetTangentBitangentFromNormal();

		indexCount++;
	}

	indexCount = 0;

	for (auto& i : indices) {
		m_pIndices[indexCount++] = i;
	}

	// Bounding Box
	//CR(InitializeBoundingSphere());
	CR(InitializeOBB());

	Validate();
	return;

Error:
	Invalidate();
	return;
}

RESULT mesh::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(m_nVertices));
	CR(AllocateIndices(m_nIndices));

Error:
	return R_PASS;
}

RESULT mesh::SetVertices(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	m_nVertices = static_cast<unsigned int>(vertices.size());
	m_nIndices = m_nVertices;

	CR(Allocate());

	unsigned int verticesCnt = 0;

	for (auto& v : vertices) {
		m_pIndices[verticesCnt] = verticesCnt;
		m_pVertices[verticesCnt] = vertex(v);
		m_pVertices[verticesCnt].SetTangentBitangentFromNormal();

		verticesCnt++;
	}

Error:
	return r;
}

RESULT mesh::SetName(std::string strName) {
	m_params.strName = strName;
	return R_PASS;
}

std::string mesh::GetName() {
	return m_params.strName;
}
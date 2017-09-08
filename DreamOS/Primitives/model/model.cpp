#include "model.h"

model::model(wchar_t *pszModelName) {
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

model::model(const std::vector<vertex>& vertices) {
	// init a model with index for each vertex
	m_nIndices = m_nVertices;

	RESULT r = R_PASS;
	CR(SetVertices(vertices));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

model::model(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
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

	Validate();
	return;

Error:
	Invalidate();
	return;
}

RESULT model::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(m_nVertices));
	CR(AllocateIndices(m_nIndices));

Error:
	return R_PASS;
}

RESULT model::SetVertices(const std::vector<vertex>& vertices) {
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
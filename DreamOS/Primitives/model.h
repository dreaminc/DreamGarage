#ifndef MODEL_H_
#define MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/model.h

#include <vector>
#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"

#include "Sandbox/FileLoader.h"
#include "Sandbox/PathManager.h"

class model : public DimObj {
public:

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(m_nVertices));
		CR(AllocateIndices(m_nVertices));

		/*
		for (int i = 0; i < m_nVertices; i++)
			m_pIndices[i] = i;
		*/

	Error:
		return R_PASS;
	}

	inline dimindex NumberVertices() { return m_nVertices; }
	inline dimindex NumberIndices() { return m_nVertices; }

private:
	unsigned int m_nVertices;

private:
	RESULT SetVertices(const std::vector<vertex>& vertices) {
		RESULT r = R_PASS;

		m_nVertices = (unsigned int)vertices.size();
		CR(Allocate());

		unsigned int verticesCnt = 0;

		for (auto& v : vertices) {
			m_pIndices[verticesCnt] = verticesCnt;
			m_pVertices[verticesCnt++] = vertex(v);

			if (verticesCnt % 3 == 0) {
				SetTriangleTangentBitangent(verticesCnt - 3, verticesCnt - 2, verticesCnt - 1);
			}
		}

	Error:
		return r;
	}

public:
	model(wchar_t *pszModelName) {
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

		FileLoaderHelper::LoadOBJFile(objFile, vertices);

		// TODO: This is a stop gap approach, this should move to manipulating the verts/indices of the DimObj directly
		// TODO: to avoid mem duplication
		CR(SetVertices(vertices));
// TODO: use this label
//	Success:
		Validate();
		return;

	Error:
		Invalidate();
		return;
	}

	model(const std::vector<vertex>& vertices) {
		/*m_nVertices = vertices.size();

		RESULT r = R_PASS;
		CR(Allocate());

		unsigned int verticesCnt = 0;

		for (auto& v : vertices) {
			m_pVertices[verticesCnt++] = vertex(v);

			if (verticesCnt % 3 == 0) {
				SetTriangleTangentBitangent(verticesCnt - 3, verticesCnt - 2, verticesCnt - 1);
			}
		}
		*/

		RESULT r = R_PASS;
		CR(SetVertices(vertices));
	
		Validate();
		return;

	Error:
		Invalidate();
		return;
	}
};

#endif // ! MODEL_H_

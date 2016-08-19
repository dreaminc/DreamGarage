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
		CR(AllocateIndices(m_nIndices));

	Error:
		return R_PASS;
	}

	inline unsigned int NumberVertices() { return m_nVertices; }
	inline unsigned int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

private:
	RESULT SetVertices(const std::vector<vertex>& vertices) {
		RESULT r = R_PASS;

		m_nVertices = static_cast<unsigned int>(vertices.size());
		m_nIndices = m_nVertices;
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

	RESULT SetVerticesIndices(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
		RESULT r = R_PASS;

		m_nVertices = static_cast<unsigned int>(vertices.size());
		m_nIndices = static_cast<unsigned int>(indices.size());
		CR(Allocate());

		unsigned int verticesCnt = 0;
		unsigned int indexCnt = 0;

		for (auto& v : vertices) {
			m_pVertices[verticesCnt++] = vertex(v);

			if (verticesCnt % 3 == 0) {
				SetTriangleTangentBitangent(verticesCnt - 3, verticesCnt - 2, verticesCnt - 1);
			}
		}

		for (auto& ind : indices) {
			m_pIndices[indexCnt++] = (dimindex)(ind);
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
	/*
	model(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
		RESULT r = R_PASS;
		CR(SetVerticesIndices(vertices, indices));

		Validate();
		return;

	Error:
		Invalidate();
		return;
	}
	*/
	model(const std::vector<vertex>& vertices) {
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

	model(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
		RESULT r = R_PASS;

		m_nIndices = static_cast<unsigned int>(indices.size());		
		m_nVertices = static_cast<unsigned int>(vertices.size());
		CR(Allocate());

		unsigned int Cnt = 0;

		for (auto& v : vertices) {
			m_pVertices[Cnt++] = vertex(v);
		}

		Cnt = 0;

		for (auto& i : indices) {
			m_pIndices[Cnt++] = i;

			if (Cnt % 3 == 0) {
				SetTriangleTangentBitangent(m_pIndices[Cnt - 3], m_pIndices[Cnt - 2], m_pIndices[Cnt - 1]);
			}
		}

		Validate();
		return;

	Error:
		Invalidate();
		return;
	}
};

#endif // ! MODEL_H_

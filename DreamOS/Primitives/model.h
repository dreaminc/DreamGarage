#ifndef MODEL_H_
#define MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/model.h

#include <vector>
#include "DimObj.h"
#include "Vertex.h"
#include "Sandbox\PathManager.h"
#include "point.h"
#include "color.h"
#include <Sandbox/FileLoader.h>

class model : public DimObj {
public:

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(m_nVertices));
		CR(AllocateIndices(m_nVertices));

		for (int i = 0; i < m_nVertices; i++)
			m_pIndices[i] = i;

	Error:
		return R_PASS;
	}

	inline int NumberVertices() {
		return m_nVertices;
	}

	inline int NumberIndices() {
		return m_nIndices;
	}

	inline RESULT SetNumVertices(unsigned int numVerts) {
		m_nVertices = numVerts;
		return R_PASS;
	}

	inline RESULT SetNumIndices(unsigned int numIndices) {
		m_nIndices = numIndices;
		return R_PASS;
	}

public:
	//model(const std::vector<vertex>& vertices) {
	model() {
		// empty stub
	}

	model(model *pModel) :
		DimObj(pModel),
		m_nVertices(pModel->NumberVertices()),
		m_nIndices(pModel->NumberIndices())
	{
		// empty	
	}

	RESULT InitializeFromFile(wchar_t *pszFilename) {
		RESULT r = R_PASS;

		PathManager *pPathManager = PathManager::instance();
		wchar_t* pFilePath = nullptr;
		std::ifstream *pFile = nullptr;
		CRM(pPathManager->GetFilePath(PATH_MODELS, pszFilename, pFilePath), "Failed to get path for %S model", pszFilename);
		CRM(FileLoader::LoadFile(pFilePath, pFile), "Failed to open %S", pszFilename);

		CRM(LoadFile(pFile), "Failed to load file %S", pszFilename);

	Error:
		if (pFilePath != nullptr) {
			delete[] pFilePath;
			pFilePath = nullptr;
		}

		if (pFile != nullptr) {
			pFile->close();
			delete pFile;
			pFile = nullptr;
		}

		return r;
	}

	virtual RESULT LoadFile(std::ifstream *pFile) = 0;

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;
};

#endif // ! MODEL_H_

#ifndef MODEL_H_
#define MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/model.h

//#include <vector>

//#include "Primitives/DimObj.h"
//#include "Primitives/Vertex.h"
//#include "Primitives/point.h"
//#include "Primitives/color.h"

#include "Primitives/composite.h"

class HALImp;
class mesh;

class model : public composite {
public:
	model(HALImp *pParentImp);

	std::shared_ptr<mesh> MakeMesh(const std::vector<vertex>& vertices);
	std::shared_ptr<mesh> AddMesh(const std::vector<vertex>& vertices);

	std::shared_ptr<mesh> MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);
	std::shared_ptr<mesh> AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	/*
public:
	virtual RESULT Allocate() override;

	inline unsigned int NumberVertices() { return m_nVertices; }
	inline unsigned int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	model(wchar_t *pszModelName);
	model(const std::vector<vertex>& vertices);
	model(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

private:
	RESULT SetVertices(const std::vector<vertex>& vertices);
	*/

	RESULT SetModelFilePath(std::wstring wstrFilepath);
	std::wstring GetModelFilePath();
	std::wstring GetModelDirectoryPath();

private:
	std::wstring m_wstrModelFilePath;
	std::wstring m_wstModelDirectoryPath;
};

#endif // ! MODEL_H_

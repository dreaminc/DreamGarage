#ifndef MESH_H_
#define MESH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/mesh.h

#include <vector>

#include "Primitives/DimObj.h"
#include "Primitives/Vertex.h"
#include "Primitives/point.h"
#include "Primitives/color.h"

class mesh : public DimObj {
public:
	virtual RESULT Allocate() override;

	inline unsigned int NumberVertices() { return m_nVertices; }
	inline unsigned int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	//mesh(wchar_t *pszModelName);
	mesh(const std::vector<vertex>& vertices);
	mesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	RESULT SetName(std::string strName);
	std::string GetName();

private:
	RESULT SetVertices(const std::vector<vertex>& vertices);

	std::string m_strName;
};

#endif // ! MESH_H_
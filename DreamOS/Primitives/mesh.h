#ifndef MESH_H_
#define MESH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/mesh.h

#include <vector>
#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"

class mesh : public DimObj {
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

	inline int NumberVertices() { return m_nVertices; }
	inline int NumberIndices() { return m_nVertices; }

private:
	unsigned int m_nVertices;

public:
	mesh(const std::vector<vertex>& vertices)
	{
		m_nVertices = vertices.size();

		RESULT r = R_PASS;
		CR(Allocate());

		unsigned int verticesCnt = 0;

		for (auto& v : vertices)
		{
			m_pVertices[verticesCnt++] = vertex(v);
		}
	
		Validate();
	Error:
		Invalidate();
	}
};

#endif // ! MESH_H_

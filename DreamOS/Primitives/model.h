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

	inline int NumberVertices() { return m_nVertices; }
	inline int NumberIndices() { return m_nVertices; }

private:
	unsigned int m_nVertices;

public:
	model(const std::vector<vertex>& vertices)
	{
		m_nVertices = vertices.size();

		RESULT r = R_PASS;
		CR(Allocate());

		unsigned int verticesCnt = 0;

		for (auto& v : vertices)
		{
			m_pVertices[verticesCnt++] = vertex(v);

			if (verticesCnt % 3 == 0) {
				SetTriangleTangentBitangent(verticesCnt - 3, verticesCnt - 2, verticesCnt - 1);
			}
		}
	
		Validate();
	Error:
		Invalidate();
	}
};

#endif // ! MODEL_H_

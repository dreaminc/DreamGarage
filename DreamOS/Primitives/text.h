#ifndef TEXT_H_
#define TEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/text.h
// text is defined as a list of quads (per each character in the text). The quads as a whole define the "geometry" of a text.

#include <vector>
#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "quad.h"

class text : public DimObj {
public:

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(m_nVertices));
		CR(AllocateIndices(m_nIndices));

	Error:
		return R_PASS;
	}

	inline int NumberVertices() { return m_nVertices; }
	inline int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	text(std::vector<quad>& quads)
	{
		m_nVertices = 4 * quads.size();
		m_nIndices = 6 * quads.size();

		RESULT r = R_PASS;
		CR(Allocate());

		unsigned int verticesCnt = 0;
		unsigned int indicesCnt = 0;
		unsigned int quadCnt = 0;

		for (auto& q : quads)
		{
			vertex* pVertices = q.VertexData();

			m_pVertices[verticesCnt++] = pVertices[0];
			m_pVertices[verticesCnt++] = pVertices[1];
			m_pVertices[verticesCnt++] = pVertices[2];
			m_pVertices[verticesCnt++] = pVertices[3];

			dimindex* pIndices = q.IndexData();

			m_pIndices[indicesCnt++] = pIndices[0] + quadCnt;
			m_pIndices[indicesCnt++] = pIndices[1] + quadCnt;
			m_pIndices[indicesCnt++] = pIndices[2] + quadCnt;
			m_pIndices[indicesCnt++] = pIndices[3] + quadCnt;
			m_pIndices[indicesCnt++] = pIndices[4] + quadCnt;
			m_pIndices[indicesCnt++] = pIndices[5] + quadCnt;

			quadCnt += 4;
		}
	
		Validate();
	Error:
		Invalidate();
	}
};

#endif // ! TEXT_H_

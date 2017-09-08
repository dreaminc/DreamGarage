#ifndef MODEL_H_
#define MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/model.h

#include <vector>

#include "Primitives/DimObj.h"
#include "Primitives/Vertex.h"
#include "Primitives/point.h"
#include "Primitives/color.h"

#include "Sandbox/PathManager.h"

// TODO: Make mesh, model as composite or use scene?
class model : public DimObj {
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
};

#endif // ! MODEL_H_

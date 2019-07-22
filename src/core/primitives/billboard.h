#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/primitives/billboard.h

// Billboard Primitive

#include "core/dimension/DimObj.h"

class point;
class texture;

class billboard : public virtual DimObj {
public:
	billboard(point ptOrigin, float width, float height);
	~billboard();

	virtual inline unsigned int NumberIndices() override;
	virtual inline unsigned int NumberVertices() override;
	virtual RESULT Allocate() override;

	float GetWidth();
	float GetHeight();

	RESULT SetVertex(point ptOrigin);

private:
	float m_width;
	float m_height;
};

#endif // ! BILLBOARD_H_
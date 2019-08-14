#ifndef OGL_QUAD_H_
#define OGL_QUAD_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLQuad.h

// OpenGL Quadrilateral Object

#include "OGLObj.h"

#include "core/primitives/quad.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLQuad : public quad, public OGLObj {
public:
	OGLQuad(OGLImp *pParentImp);
	OGLQuad(OGLImp *pParentImp, quad&& q);
	OGLQuad(OGLImp *pParentImp, quad::params *pQuadParams);
	OGLQuad(OGLImp *pParentImp, float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	OGLQuad(OGLImp *pParentImp, float width, float height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	OGLQuad(OGLImp *pParentImp, float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal);
	OGLQuad(OGLImp *pParentImp, float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector());
	OGLQuad(OGLImp *pParentImp, BoundingQuad* pBoundingQuad, bool fTriangleBased);

	~OGLQuad() = default;

	RESULT UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad);
};
#pragma warning(pop)

#endif // ! OGL_QUAD_H_

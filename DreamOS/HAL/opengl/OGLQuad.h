#ifndef OGL_QUAD_H_
#define OGL_QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLQuad.h
// OpenGL Quadrilateral Object

#include "OGLObj.h"
#include "Primitives/quad.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLQuad : public quad, public OGLObj {
public:
	OGLQuad(OpenGLImp *pParentImp);
	OGLQuad(OpenGLImp *pParentImp, quad&& q);
	OGLQuad(OpenGLImp *pParentImp, quad::params *pQuadParams);
	OGLQuad(OpenGLImp *pParentImp, float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	OGLQuad(OpenGLImp *pParentImp, float width, float height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	OGLQuad(OpenGLImp *pParentImp, float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal);
	OGLQuad(OpenGLImp *pParentImp, float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector());
	OGLQuad(OpenGLImp *pParentImp, BoundingQuad* pBoundingQuad, bool fTriangleBased);

	~OGLQuad() = default;

	RESULT UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad);
};
#pragma warning(pop)

#endif // ! OGL_QUAD_H_

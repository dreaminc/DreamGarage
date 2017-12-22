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
	OGLQuad(OpenGLImp *pParentImp) :
		quad(1.0f),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, quad&& q) :
		quad(q),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) :
		quad(side, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, float width, float height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) :
		quad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) :
		quad(width, height, ptCenter, uvTopLeft, uvBottomRight, vNormal),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector()) :
		quad(width, height, numHorizontalDivisions, numVerticalDivisions, uvTopLeft, uvBottomRight, curveType, vNormal),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, BoundingQuad* pBoundingQuad, bool fTriangleBased) :
		quad(pBoundingQuad, fTriangleBased),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	RESULT UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad) {
		RESULT r = R_PASS;

		CR(quad::UpdateFromBoundingQuad(pBoundingQuad));
		CR(UpdateOGLBuffers());

	Error:
		return r;
	}
};
#pragma warning(pop)

#endif // ! OGL_QUAD_H_

#ifndef OGL_QUAD_H_
#define OGL_QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLQuad.h
// OpenGL Quadrilateral Object

#include "OGLObj.h"
#include "Primitives/quad.h"

class OGLQuad : public quad, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

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

	OGLQuad(OpenGLImp *pParentImp, BoundingQuad* pBoundingQuad, bool fTriangleBased) :
		quad(pBoundingQuad, fTriangleBased),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	RESULT UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad) {
		RESULT r = R_PASS;

		quad *pQuad = (quad*)(GetDimObj());
		CR(pQuad->UpdateFromBoundingQuad(pBoundingQuad));
		CR(UpdateOGLBuffers());

	Error:
		return r;
	}
};

#endif // ! OGL_QUAD_H_

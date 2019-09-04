#include "OGLQuad.h"

OGLQuad::OGLQuad(OpenGLImp *pParentImp) :
	quad(1.0f),
	OGLObj(pParentImp)
{
	// 
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, quad&& q) :
	quad(q),
	OGLObj(pParentImp)
{
	// 
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, quad::params *pQuadParams) :
	quad(pQuadParams),
	OGLObj(pParentImp)
{
	//
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, float side, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	quad(side, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal),
	OGLObj(pParentImp)
{
	//
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, float width, float height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	quad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal),
	OGLObj(pParentImp)
{
	//
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) :
	quad(width, height, ptCenter, uvTopLeft, uvBottomRight, vNormal),
	OGLObj(pParentImp)
{
	// 
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType, vector vNormal) :
	quad(width, height, numHorizontalDivisions, numVerticalDivisions, uvTopLeft, uvBottomRight, curveType, vNormal),
	OGLObj(pParentImp)
{
	// 
}

OGLQuad::OGLQuad(OpenGLImp *pParentImp, BoundingQuad* pBoundingQuad, bool fTriangleBased) :
	quad(pBoundingQuad, fTriangleBased),
	OGLObj(pParentImp)
{
	// 
}

RESULT OGLQuad::UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad) {
	RESULT r = R_PASS;

	CR(quad::UpdateFromBoundingQuad(pBoundingQuad));
	CR(UpdateOGLBuffers());

Error:
	return r;
}
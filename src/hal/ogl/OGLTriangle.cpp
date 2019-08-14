#include "OGLTriangle.h"

OGLTriangle::OGLTriangle(OGLImp *pParentImp) :
	triangle(1.0f),
	OGLObj(pParentImp)
{
	//m_pDimTriangle = new triangle(1.0f);

	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

OGLTriangle::OGLTriangle(OGLImp *pParentImp, float side) :
	triangle(side),
	OGLObj(pParentImp)
{
	//m_pDimTriangle = new triangle(side);

	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

OGLTriangle::OGLTriangle(OGLImp *pParentImp, float height, float width) :
	triangle(height, width),
	OGLObj(pParentImp)
{
	//m_pDimTriangle = new triangle(height, width);

	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}
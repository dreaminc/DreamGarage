#include "DimRay.h"
#include "quaternion.h"
#include "volume.h"

DimRay::DimRay(point ptOrigin, vector vDirection, float step) :
	ray(ptOrigin, vDirection)
{
	SetRayVertices(step);
}

unsigned int DimRay::NumberIndices() {
	return 2;
}

unsigned int DimRay::NumberVertices() {
	return 2;
}

RESULT DimRay::Allocate()
{
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return r;
}

RESULT DimRay::SetRayVertices(float step) {
	RESULT r = R_PASS;

	// line segment vertices
	vertex vBegin = vertex(m_ptOrigin);
	vertex vEnd = vertex(m_ptOrigin + point(step * m_vDirection));

	CR(Allocate());

	m_pVertices[0] = vBegin;
	m_pVertices[1] = vEnd;

	m_pIndices[0] = 0;
	m_pIndices[1] = 1;

Error:
	return r;
}

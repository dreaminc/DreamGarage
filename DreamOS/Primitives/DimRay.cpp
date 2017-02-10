#include "DimRay.h"
#include "quaternion.h"
#include "volume.h"

DimRay::DimRay(point ptOrigin, vector vDirection, float step) :
	ray(ptOrigin, vDirection)
{
	RESULT r = R_PASS;
	CR(Allocate());

	SetRayVertices(step);

	//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

unsigned int DimRay::NumberIndices() {
	return 10;
}

unsigned int DimRay::NumberVertices() {
	return 6;
}

RESULT DimRay::Allocate() {
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
	point ptEnd = m_ptOrigin + point(step * m_vDirection);
	vertex vEnd = vertex(ptEnd);

	int endIndex;
	int indexCounter = 0;
	int vertCounter = 0;
	float flare = 0.5f;

	// Line
	m_pVertices[vertCounter++] = vBegin;
	m_pVertices[vertCounter++] = vEnd;
	m_pIndices[indexCounter++] = 0;
	m_pIndices[indexCounter++] = endIndex = 1;

	// Arrow Line Left
	point ptArrowLineLeft = ptEnd - (vector)(m_vDirection + (m_vDirection.NormalizedCross(vector::iVector(1.0f))) * flare) * (step * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineLeft);
	m_pIndices[indexCounter++] = 2;
	m_pIndices[indexCounter++] = endIndex;

	// Arrow Line Right
	point ptArrowLineRight = ptEnd - (vector)(m_vDirection - (m_vDirection.NormalizedCross(vector::iVector(1.0f))) * flare) * (step * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineRight);
	m_pIndices[indexCounter++] = 3;
	m_pIndices[indexCounter++] = endIndex;

	// Arrow Line Front
	point ptArrowLineFront = ptEnd - (vector)(m_vDirection + (m_vDirection.NormalizedCross(vector::kVector(1.0f))) * flare) * (step * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineFront);
	m_pIndices[indexCounter++] = 4;
	m_pIndices[indexCounter++] = endIndex;

	// Arrow Line Back
	point ptArrowLineBack = ptEnd - (vector)(m_vDirection - (m_vDirection.NormalizedCross(vector::kVector(1.0f))) * flare) * (step * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineBack);
	m_pIndices[indexCounter++] = 5;
	m_pIndices[indexCounter++] = endIndex;

//Error:
	return r;
}

#include "billboard.h"
#include "point.h"
#include "texture.h"

billboard::billboard(point ptOrigin, float width, float height) :
	DimObj(),
	m_width(width),
	m_height(height)
{
	Allocate();
	SetVertex(ptOrigin);
}

billboard::~billboard() {
	// empty
}

float billboard::GetWidth() {
	return m_width;
}

float billboard::GetHeight() {
	return m_height;
}

RESULT billboard::SetVertex(point ptOrigin) {
	m_pVertices[0] = vertex(ptOrigin);
	return R_PASS;
}

unsigned int billboard::NumberIndices() {
	return 1;
}

unsigned int billboard::NumberVertices() {
	return 1;
}

RESULT billboard::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return r;
}
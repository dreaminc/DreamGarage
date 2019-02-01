#include "billboard.h"
#include "point.h"
#include "texture.h"

billboard::billboard(point ptOrigin, float width, float height, texture *pTexture) :
	DimObj(),
	m_pTexture(pTexture),
	m_width(width),
	m_height(height)
{
	SetVertex(ptOrigin);
}

billboard::~billboard() {
	// empty
}

RESULT billboard::SetTexture(texture *pTexture) {
	m_pTexture = pTexture;
	return R_PASS;
}

texture* billboard::GetTexture() {
	return m_pTexture;
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
#include "billboard.h"
#include "point.h"
#include "texture.h"

billboard::billboard(point ptOrigin, float width, float height, texture *pTexture) :
	VirtualObj(ptOrigin),
	m_pTexture(pTexture),
	m_width(width),
	m_height(height)
{
	// empty
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
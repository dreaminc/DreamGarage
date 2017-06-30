#include "text.h"
#include "quad.h"
#include <vector>
#include <algorithm>

#include "font.h"
#include "HAL/HALImp.h"
#include "Framebuffer.h"

text::text(HALImp *pHALImp, std::shared_ptr<font> font, const std::string& strText, double width, double height, bool fBillboard) :
	FlatContext(pHALImp),
	m_pFont(font),
	m_width(width),
	m_height(height)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something

	CR(SetText(strText));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

text::~text() {
	if (m_pQuad != nullptr) {
		m_pQuad = nullptr;
	}

	ClearChildren();
}

RESULT text::RenderToQuad() {
	RESULT r = R_PASS;

	CR(RenderToTexture());

	CR(ClearChildren());

	if (m_pQuad == nullptr) {
		m_pQuad = AddQuad(m_width, m_height, point(0.0f));
		CN(m_pQuad);
	}

	CR(m_pQuad->SetColorTexture(GetFramebuffer()->GetColorTexture()));

Error:
	return r;
}

VirtualObj* text::SetPosition(point pt, VerticalAlignment vAlign, HorizontalAlignment hAlign) {
	uv_precision xOffset = 0.0f;
	uv_precision yOffset = 0.0f;
	
	m_vAlign = vAlign;
	m_hAlign = hAlign;

	// TODO: Change this to use DimObj pivot
	switch (m_hAlign) {
		case HorizontalAlignment::LEFT: {
			xOffset = -m_width / 2;
		} break;

		case HorizontalAlignment::CENTER: {
			// nothing
		} break;

		case HorizontalAlignment::RIGHT: {
			xOffset = m_width / 2;
		} break;
	}

	switch (m_vAlign) {
		case VerticalAlignment::TOP: {
			yOffset = m_height / 2;
		} break;

		case VerticalAlignment::MIDDLE: {
			// nothing
		} break;

		case VerticalAlignment::BOTTOM: {
			yOffset = -m_height / 2;
		} break;
	}

	return this->MoveTo(pt.x() + xOffset, pt.y() + yOffset, pt.z());
}

std::string& text::GetText() {
	return m_strText;
}

std::shared_ptr<font> text::GetFont() { 
	return m_pFont; 
}

float text::GetWidth() {
	return m_width;
}

float text::GetHeight() {
	return m_height;
}

float text::GetDPMM(float mmVal) {
	return m_dpmm * mmVal;
}

float text::GetDPM(float mVal) {
	return m_dpmm * 100.0f * mVal;
}

// TODO: Update everything
RESULT text::SetWidth(float width) {
	m_width = width;
	return R_PASS;
}

RESULT text::SetHeight(float height) {
	m_height = height;
	return R_PASS;
}

RESULT text::SetDPMM(float dpmm) {
	m_dpmm = dpmm;
	return R_PASS;
}

RESULT text::SetText(const std::string& strText) {
	RESULT r = R_PASS;
	point ptCenter;

	CBR((m_strText.compare(strText) != 0), R_NO_EFFECT);

	// Clear out kids
	CR(ClearChildren());

	m_strText = strText;

	float fontImageWidth = static_cast<float>(m_pFont->GetFontTextureWidth());
	float fontImageHeight = static_cast<float>(m_pFont->GetFontTextureHeight());
	float fontBase = static_cast<float>(m_pFont->GetFontBase());

	// Apply DPMM to the width
	double effectiveDotsWidth = GetDPMM(m_width);
	double effectiveDotsHeight = GetDPMM(m_height);

	//float maxBelow = 0.0f;
	//float maxAbove = 0.0f;

	float posX = 0.0f;
	float posY = 0.0f;

	//float minLeft = std::numeric_limits<float>::max();
	//float maxRight = std::numeric_limits<float>::min();
	//float maxTop = std::numeric_limits<float>::min();
	//float minBottom = std::numeric_limits<float>::max();

	for(char &c : m_strText) {
		font::CharacterGlyph glyph;

		if (m_pFont->GetGlyphFromChar(c, glyph)) {
			
			// TODO: Do this through composite
			//minLeft = std::min(minLeft, posX + glyph.bearingX);
			//maxRight = std::max(maxRight, posX + glyph.bearingX);
			//maxTop = std::max(maxTop, ((fontBase - glyph.bearingY) + glyph.height) / 2.0f);
			//minBottom = std::min(minBottom, ((fontBase - glyph.bearingY) - glyph.height) / 2.0f);
			
			// UV
			float uvTop = (fontImageHeight - glyph.y) / fontImageHeight;
			float uvBottom = ((fontImageHeight - glyph.y) - glyph.height) / fontImageHeight;

			uvBottom = 1.0f - uvBottom;
			uvTop = 1.0f - uvTop;

			float uvLeft = glyph.x / fontImageWidth;
			float uvRight = (glyph.x + glyph.width) / fontImageWidth;

			uvcoord uvTopLeft = uvcoord(uvLeft, uvTop);
			uvcoord uvBottomRight = uvcoord(uvRight, uvBottom);

			// Position

			float glyphQuadXPosition = posX + ((float)(glyph.width) / 2.0f) + (float)(glyph.bearingX);
			float glyphQuadYPosition = posY - ((float)(fontBase) - (float)(glyph.bearingY) - ((float)(glyph.height) / 2.0f));

			//float glyphWidth = GetDPM(glyph.width);
			//float glyphHeight = GetDPM(glyph.height);

			float glyphWidth = (glyph.width);
			float glyphHeight = (glyph.height);

			// Apply DPMM
			glyphWidth /= (m_dpmm * 10.0f);
			glyphHeight /= (m_dpmm * 10.0f);

			glyphQuadXPosition /= (m_dpmm * 10.0f);
			glyphQuadYPosition /= (m_dpmm * 10.0f);

			//glyphQuadXPosition = GetDPM(glyphQuadXPosition);
			//glyphQuadYPosition = GetDPM(glyphQuadYPosition);

			point ptGlyph = point(glyphQuadXPosition, m_height, glyphQuadYPosition);
			auto pQuad = AddQuad(glyphWidth, glyphHeight, ptGlyph, uvTopLeft, uvBottomRight);
			pQuad->SetColorTexture(m_pFont->GetTexture().get());

			// TODO: Add in wrap / heights

			posX += (float)(glyph.advance);
		}
	}
	
	//m_width = maxRight - minLeft;
	//m_height = maxTop - minBottom;

	//ptCenter = point((minLeft + maxRight) / 2.0f, (maxTop + minBottom) / 2.0f, 0.0f);

Error:
	return r;
}
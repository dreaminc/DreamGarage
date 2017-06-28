#include "text.h"
#include "quad.h"
#include <vector>
#include <algorithm>

#include "font.h"

text::text(HALImp *pHALImp, std::shared_ptr<font> font, const std::string& strText, double size, bool fBillboard) :
	FlatContext(pHALImp),
	m_pFont(font)
{
	RESULT r = R_PASS;

	CR(SetText(strText, size));

	Validate();
	return;

Error:
	Invalidate();
	return;
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

RESULT text::SetText(const std::string& strText, double size, bool* fChanged) {
	RESULT r = R_PASS;


	if (m_strText.compare(strText) == 0) {
		// no need to update the text
		if (fChanged) {
			*fChanged = false;
		}

		return R_SUCCESS;
	}
	else  if (fChanged) {
		*fChanged = true;
	}

	// Clear out kids
	CR(ClearChildren());

	m_strText = strText;

	uv_precision glyphWidth = static_cast<float>(m_pFont->GetGlyphWidth());
	uv_precision glyphHeight = static_cast<float>(m_pFont->GetGlyphHeight());
	uv_precision glyphBase = static_cast<float>(m_pFont->GetGlyphBase());

	float maxBelow = 0.0f;
	float maxAbove = 0.0f;
	float posX = 0;
	point ptCenter;

	float minLeft = std::numeric_limits<float>::max();
	float maxRight = std::numeric_limits<float>::min();
	float maxTop = std::numeric_limits<float>::min();
	float minBottom = std::numeric_limits<float>::max();

	for_each(strText.begin(), strText.end(), [&](char c) {
		font::CharacterGlyph glyph;

		if (m_pFont->GetGlyphFromChar(c, glyph)) {
			uv_precision x = glyph.x / glyphWidth;
			uv_precision y = (glyphHeight - glyph.y) / glyphHeight;
			uv_precision w = glyph.width / glyphWidth;
			uv_precision h = glyph.height / glyphHeight;

			//uv_precision dx = XSCALE_TO_SCREEN(glyph.width);
			//uv_precision dy = YSCALE_TO_SCREEN(glyph.height);

			//vector_precision dxs = XSCALE_TO_SCREEN(glyph.bearingX);
			//vector_precision dys = YSCALE_TO_SCREEN(glyphBase - glyph.bearingY) - dy / 2.0f;

			minLeft = std::min(minLeft, posX + dxs);
			maxRight = std::max(maxRight, dx + posX + dxs);
			maxTop = std::max(maxTop, dys + dy / 2.0f);
			minBottom = std::min(minBottom, dys - dy / 2.0f);
			

			auto pQuad = AddQuad(x, y, vector(dx / 2.0f + posX + dxs, dys, 0.0f));
			//pQuad->SetUV()

			//quads.push_back(quad(dy, dx, , uvcoord(x, y - h), uvcoord(x + w, y)));

			//posX += XSCALE_TO_SCREEN(glyph.advance);
			posX += glyph.advance;
		}
	});

	m_width = maxRight - minLeft;
	m_height = maxTop - minBottom;

	ptCenter = point((minLeft + maxRight) / 2.0f, (maxTop + minBottom) / 2.0f, 0.0f);

	/*
	unsigned int verticesCnt = 0;
	unsigned int indicesCnt = 0;
	unsigned int quadCnt = 0;

	for (auto& q : quads) {
		vertex* pVertices = q.VertexData();

		pVertices[0].m_point -= ptCenter;
		pVertices[1].m_point -= ptCenter;
		pVertices[2].m_point -= ptCenter;
		pVertices[3].m_point -= ptCenter;

		m_pVertices[verticesCnt++] = pVertices[0];
		m_pVertices[verticesCnt++] = pVertices[1];
		m_pVertices[verticesCnt++] = pVertices[2];
		m_pVertices[verticesCnt++] = pVertices[3];

		dimindex* pIndices = q.IndexData();

		m_pIndices[indicesCnt++] = pIndices[0] + quadCnt;
		m_pIndices[indicesCnt++] = pIndices[1] + quadCnt;
		m_pIndices[indicesCnt++] = pIndices[2] + quadCnt;
		m_pIndices[indicesCnt++] = pIndices[3] + quadCnt;
		m_pIndices[indicesCnt++] = pIndices[4] + quadCnt;
		m_pIndices[indicesCnt++] = pIndices[5] + quadCnt;

		quadCnt += 4;
	}
	*/

Error:
	return r;
}
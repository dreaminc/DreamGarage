#include "text.h"
#include "quad.h"
#include <vector>
#include <algorithm>

RESULT text::SetText(const std::string& text, double size)
{
	std::vector<quad> quads;
	point center_vector;

	if (m_text.compare(text) == 0)
	{
		// no need to update the text
		return R_SUCCESS;
	}

	if (m_text.length() != text.length())
	{
		// text length was changed, we need to re-allocate buffers
		Destroy();

		m_nVertices = 4 * static_cast<unsigned int>(text.length());
		m_nIndices = 6 * static_cast<unsigned int>(text.length());

		RESULT r = R_PASS;
		CR(Allocate());

		SetDirty();
	}

	m_text = text;

	float posx = 0;

	// For now the font scale is based on 1080p
	const int screen_width = static_cast<int>(1920 * size);
	const int screen_height = static_cast<int>(1080 * size);

	uv_precision	glyphWidth = static_cast<float>(m_font->GetGlyphWidth());
	uv_precision	glyphHeight = static_cast<float>(m_font->GetGlyphHeight());
	uv_precision	glyphBase = static_cast<float>(m_font->GetGlyphBase());

	#define XSCALE_TO_SCREEN(x)	2.0f * (x) / screen_width
	#define YSCALE_TO_SCREEN(y)	2.0f * (y) / screen_height

	m_width = 0.0f;
	float max_below = 0.0f;
	float max_above = 0.0f;

	bool  first_char = true;

	float min_left = 0.0f;
	float max_right = 0.0f;
	float max_top = 0.0f;
	float min_bottom = 0.0f;

	for_each(text.begin(), text.end(), [&](char c) {
		Font::CharacterGlyph glyph;
		if (m_font->GetGlyphFromChr(c, glyph))
		{
			uv_precision x = glyph.x / glyphWidth;
			uv_precision y = (glyphHeight - glyph.y) / glyphHeight;
			uv_precision w = glyph.width / glyphWidth;
			uv_precision h = glyph.height / glyphHeight;

			uv_precision dx = XSCALE_TO_SCREEN(glyph.width);
			uv_precision dy = YSCALE_TO_SCREEN(glyph.height);

			vector_precision dxs = XSCALE_TO_SCREEN(glyph.xoffset);
			vector_precision dys = YSCALE_TO_SCREEN(glyphBase - glyph.yoffset) - dy / 2.0f;

			if (first_char)
			{
				first_char = false;

				min_left = posx + dxs;
				max_right = dx + posx + dxs;
				max_top = dys + dy / 2.0f;
				min_bottom = dys - dy / 2.0f;
			}
			else
			{
				min_left = std::min(min_left, posx + dxs);
				max_right = std::max(max_right, dx + posx + dxs);
				max_top = std::max(max_top, dys + dy / 2.0f);
				min_bottom = std::min(min_bottom, dys - dy / 2.0f);
			}

			quads.push_back(quad(dy, dx, vector(dx / 2.0f + posx + dxs, dys, 0), uvcoord(x, y - h), uvcoord(x + w, y)));
			posx += XSCALE_TO_SCREEN(glyph.xadvance);
		}
	});

	m_width = max_right - min_left;
	m_height = max_top - min_bottom;

	center_vector = point((min_left + max_right) / 2.0f, (max_top + min_bottom) / 2.0f, 0.0f);

	unsigned int verticesCnt = 0;
	unsigned int indicesCnt = 0;
	unsigned int quadCnt = 0;

	for (auto& q : quads)
	{
		vertex* pVertices = q.VertexData();

		pVertices[0].m_point -= center_vector;
		pVertices[1].m_point -= center_vector;
		pVertices[2].m_point -= center_vector;
		pVertices[3].m_point -= center_vector;

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

	return R_SUCCESS;

Error:
	return R_FAIL;
}
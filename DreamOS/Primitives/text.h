#ifndef TEXT_H_
#define TEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/text.h
// text is defined as a list of quads (per each character in the text). The quads as a whole define the "geometry" of a text.

#include <algorithm>
#include <vector>
#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "quad.h"

class text : public DimObj {
public:

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(m_nVertices));
		CR(AllocateIndices(m_nIndices));

	Error:
		return R_PASS;
	}

	inline unsigned int NumberVertices() { return m_nVertices; }
	inline unsigned int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	text(std::shared_ptr<Font> font, const std::string& text, double size = 1.0) :
		m_font(font)
	{
		SetText(text, size);

		Validate();
//	Error:
//		Invalidate();
	}

	RESULT SetText(const std::string& text, double size)
	{
		std::vector<quad> quads;

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
				vector_precision dys = YSCALE_TO_SCREEN(glyphBase - glyph.yoffset);

				quads.push_back(quad(dy, dx, vector(dx / 2.0f + posx + dxs, dys, 0), uvcoord(x, y - h), uvcoord(x + w, y)));
				posx += XSCALE_TO_SCREEN(glyph.xadvance);
			}
		});

		unsigned int verticesCnt = 0;
		unsigned int indicesCnt = 0;
		unsigned int quadCnt = 0;

		for (auto& q : quads)
		{
			vertex* pVertices = q.VertexData();

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

private:

	// Font to be used for the text
	std::shared_ptr<Font> m_font = nullptr;

	// String of the text
	std::string	m_text = "";
};

#endif // ! TEXT_H_

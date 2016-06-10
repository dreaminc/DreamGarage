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

	inline int NumberVertices() { return m_nVertices; }
	inline int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	text(std::shared_ptr<Font> font, const std::string& text) :
		m_font(font)
	{
		SetText(text);

		Validate();
	Error:
		Invalidate();
	}

	RESULT SetText(const std::string& text)
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

			m_nVertices = 4 * text.length();
			m_nIndices = 6 * text.length();

			RESULT r = R_PASS;
			CR(Allocate());
		}
		
		m_text = text;

		double posx = 0;

		// For now this is hard-coded. Need to fix incorrect size
		const int screen_width = 1180;// 1920 / 2;
		const int screen_height = 626;// 1080 / 2;

		for_each(text.begin(), text.end(), [&](char c) {
			Font::CharacterGlyph glyph;
			if (m_font->GetGlyphFromChr(c, glyph))
			{
				uv_precision x = glyph.x / 512.0f;
				uv_precision y = (512 - glyph.y) / 512.0f;
				uv_precision w = glyph.width / 512.0f;
				uv_precision h = glyph.height / 512.0f;

				double dx = 2.0f * glyph.width / screen_width;
				double dy = 2.0f * glyph.height / screen_height;

				quads.push_back(quad(dy, dx, vector(dx / 2.0f + posx + 2.0f * glyph.xoffset / screen_width, 2.0f * 75 / screen_height - dy / 2.0f - 2.0f * glyph.yoffset / screen_height, 0), uvcoord(x, y - h), uvcoord(x + w, y)));
				posx += 2.0f * glyph.xadvance / screen_width;
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

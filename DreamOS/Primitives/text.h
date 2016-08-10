#ifndef TEXT_H_
#define TEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/text.h
// text is defined as a list of quads (per each character in the text). The quads as a whole define the "geometry" of a text.

#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "font.h"
#include <memory>

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

	enum AlignmentType {
		TOP_LEFT,
		TOP_RIGHT,
		CENTER,
		BOTTOM_LEFT,
		BOTTOM_RIGHT
	};

	uv_precision m_width = 0.0f;
	uv_precision m_height = 0.0f;

	text(std::shared_ptr<Font> font, const std::string& text = "", double size = 1.0, bool isBillboard = false) :
		m_font(font)
	{
		SetText(text, size);

		Validate();
//	Error:
//		Invalidate();
	}

	RESULT SetText(const std::string& text, double size);
	
	VirtualObj* SetPosition(point p, AlignmentType align = CENTER)
	{
		uv_precision dx = (align == BOTTOM_LEFT || align == TOP_LEFT) ? 0.0f : ((align == CENTER) ? m_width / 2 : m_width);
		uv_precision dy = (align == BOTTOM_LEFT || align == BOTTOM_RIGHT) ? 0.0f : ((align == CENTER) ? m_height / 2 : m_height);
		return this->MoveTo(p.x() - dx, p.y() - dy, p.z());
	}


private:

	// Font to be used for the text
	std::shared_ptr<Font> m_font = nullptr;

	// String of the text
	std::string	m_text = "";
};

#endif // ! TEXT_H_

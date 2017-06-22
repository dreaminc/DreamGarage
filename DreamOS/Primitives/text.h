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
#include <memory>

class Font;

class text : public DimObj {
public:

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(m_nVertices));
		CR(AllocateIndices(m_nIndices));

	Error:
		return R_PASS;
	}

	inline unsigned int NumberVertices() { 
		return m_nVertices; 
	}

	inline unsigned int NumberIndices() { 
		return m_nIndices; 
	}

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:

	// TODO: Make into two (valign vs halign)
	enum AlignmentType {
		TOP_LEFT,
		TOP_RIGHT,
		CENTER,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		RIGHT,
		LEFT,
	};


	text(std::shared_ptr<Font> font, const std::string& strText = "", double size = 1.0, bool fBillboard = false);

	RESULT SetText(const std::string& strText, double size, bool* fChanged = nullptr);
	
	VirtualObj* SetPosition(point pt, AlignmentType alignType = CENTER);

	std::shared_ptr<Font> GetFont() { return m_font; }
	std::string& GetText();

	float GetWidth() {
		return m_width;
	}

	float GetHeight() {
		return m_height;
	}

private:
	float m_width = 0.0f;
	float m_height = 0.0f;

	// Font to be used for the text
	std::shared_ptr<Font> m_font;

	// String of the text
	std::string	m_strText = "";
};

#endif // ! TEXT_H_

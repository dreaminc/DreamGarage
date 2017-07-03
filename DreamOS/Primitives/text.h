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

#include "FlatContext.h"
#include "quad.h"

class font;
//class quad;

class HALImp;

//class text : public DimObj {
class text : public FlatContext {
public:
	enum class VerticalAlignment {
		TOP,
		MIDDLE,
		BOTTOM,
		INVALID
	};

	enum class HorizontalAlignment {
		LEFT,
		CENTER,
		RIGHT,
		INVALID
	};

	/*
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
	*/

public:
	text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText = "", double width = 1.0f, double height = 0.25f, bool fBillboard = false);
	~text();

	virtual RESULT SetText(const std::string& strText);
	
	VirtualObj* SetPosition(point pt, VerticalAlignment vAlign = VerticalAlignment::MIDDLE, HorizontalAlignment hAlign = HorizontalAlignment::CENTER);

	// TODO: This clobbers the rest of thing, might want to move text to quad and 
	// have a flat context internally or something like that
	RESULT RenderToQuad();

	std::string& GetText();
	std::shared_ptr<font> GetFont();

	float GetWidth();
	float GetHeight();
	float GetDPMM(float mmVal = 1.0f);
	float GetDPM(float mVal = 1.0f);

	RESULT SetWidth(float width);
	RESULT SetHeight(float height);
	RESULT SetDPMM(float dpmm);

	RESULT SetScaleToFit(bool fScaleToFit = true);
	RESULT SetOffset(float xOffset, float yOffset);

public:
	//static text& MakeText()

private:
	bool m_fScaleToFit = false;

	float m_xOffset = 0.0f;
	float m_yOffset = 0.0f;

	float m_width = 0.0f;
	float m_height = 0.0f;
	float m_dpmm = 11.0f;	// Dots per mm - global units in meters so this should be taken into consideration

	VerticalAlignment m_vAlign = VerticalAlignment::TOP;
	HorizontalAlignment m_hAlign = HorizontalAlignment::CENTER;

	// Font to be used for the text
	std::shared_ptr<font> m_pFont = nullptr;

	// String of the text
	std::string	m_strText = "";

	std::shared_ptr<quad> m_pQuad = nullptr;
};

#endif // ! TEXT_H_

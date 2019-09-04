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
struct CharacterGlyph;

class UIKeyboardLayout;

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

	enum class flags : uint16_t {
		NONE					= 0,
		WRAP					= 1 << 0,
		SCALE_TO_FIT			= 1 << 1,
		FIT_TO_SIZE				= 1 << 2,
		BILLBOARD				= 1 << 3,
		TRAIL_ELLIPSIS			= 1 << 4,
		RENDER_QUAD 			= 1 << 5,
		CURVE_QUAD_CIRCLE		= 1 << 6, 
		CURVE_QUAD_PARABOLIC	= 1 << 7,
		LEAD_ELLIPSIS			= 1 << 8,
		PASSWORD				= 1 << 9,
		USE_CURSOR				= 1 << 10,
		INVALID					= 0xFFFF
	};

public:
	text(HALImp *pHALImp, std::shared_ptr<font> pFont, text::flags textFlags = text::flags::NONE);
	text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText = "", double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE);
	text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText = "", double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);
	text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText = "", double width = 1.0f, double height = 0.25f, bool fBillboard = false);
	~text();

	virtual RESULT SetText(const std::string& strText);
	RESULT CreateLayout(UIKeyboardLayout *pLayout, double marginRatio = 0.25f);

	// SetText wrappers for cursor use
	RESULT AddCharacter(const std::string& strChar); // add character at cursor index
	RESULT RemoveCharacter(); // remove character at cursor index
	
	VirtualObj* SetPosition(point pt, VerticalAlignment vAlign = VerticalAlignment::MIDDLE, HorizontalAlignment hAlign = HorizontalAlignment::CENTER);

	std::string& GetText();
	std::shared_ptr<font> GetFont();

	RESULT RenderToQuad();

	float GetWidth();
	float GetHeight();

	float GetDPMM(float mmVal = 1.0f);
	float GetDPM(float mVal = 1.0f);

	float GetMMSizeFromDots(float val);
	float GetMSizeFromDots(float val);

	RESULT SetWidth(float width);
	RESULT SetHeight(float height);
	RESULT SetDPMM(float dpmm);

	RESULT SetFontHeightM(float mVal);
	RESULT SetFontHeightMM(float mmVal);

	RESULT SetOffset(float xOffset, float yOffset);
	RESULT SetRows(int rows);

	virtual RESULT SetScaleToFit(bool fScaleToFit = true) override;
	RESULT SetWrap(bool fWrap = true);
	RESULT SetFitToSize(bool fFitToSize = true);
	RESULT SetBillboard(bool fBillboard = true);
	RESULT SetCursorIndex(int index);

	virtual bool IsScaleToFit() override;
	bool IsWrap();
	bool IsFitToSize();
	bool IsBillboard();
	bool IsTrailingEllipsis();
	bool IsLeadingEllipsis();
	bool IsPassword();
	bool IsUsingCursor();
	bool IsRenderToQuad();

	bool CheckFlag(text::flags checkFlag);
	bool CheckFlagAgainstFlags(text::flags checkFlag, text::flags allFlags);
	RESULT AddFlags(text::flags newFlags);
	RESULT RemoveFlags(text::flags newFlags);
	
	RESULT SetBackgroundColor(color backgroundColor);
	RESULT SetBackgroundColorTexture(texture *pColorTexture);

public:
	//static text& MakeText()

private:
	std::shared_ptr<quad> AddGlyphQuad(CharacterGlyph glyph, float posX, float posY);
	RESULT SetBackgroundQuad();
	RESULT AddTrailingEllipsisQuads(float posX, float posY, float posXM, float posYM, std::vector<std::shared_ptr<quad>> curLineQuads);

private:
	//bool m_fScaleToFit = false;
	flags m_flags = text::flags::NONE;

	float m_scaleFactor = 1.0f;
	
	int m_rows = 1;

	float m_dpmm = 11.0f;	// Dots per mm - global units in meters so this should be taken into consideration

	float m_width = 0.0f;
	float m_height = 0.0f;

	float m_xOffset = 0.0f;
	float m_yOffset = 0.0f;

	VerticalAlignment m_vAlign = VerticalAlignment::TOP;
	HorizontalAlignment m_hAlign = HorizontalAlignment::CENTER;

	// Font to be used for the text
	std::shared_ptr<font> m_pFont = nullptr;

	// String of the text
	std::string	m_strText = "";

	std::shared_ptr<quad> m_pBackgroundQuad = nullptr;
	color m_backgroundColor = COLOR_WHITE;
	texture *m_pBackgroundColorTexture = nullptr;

	unsigned int m_cursorIndex = 0;
};


inline constexpr text::flags operator | (const text::flags &lhs, const text::flags &rhs) {
	return static_cast<text::flags>(
		static_cast<std::underlying_type<text::flags>::type>(lhs) | static_cast<std::underlying_type<text::flags>::type>(rhs)
		);
}

inline constexpr text::flags operator & (const text::flags &lhs, const text::flags &rhs) {
	return static_cast<text::flags>(
		static_cast<std::underlying_type<text::flags>::type>(lhs) & static_cast<std::underlying_type<text::flags>::type>(rhs)
		);
}

inline constexpr text::flags operator ~ (const text::flags &lhs) {
	return static_cast<text::flags>(
		~(static_cast<std::underlying_type<text::flags>::type>(lhs)));
}

#endif // ! TEXT_H_

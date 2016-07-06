#ifndef FONT_H_
#define FONT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/font.h

#include <string>
#include <map>
#include <vector>

#include "Primitives/quad.h"

class Font {
public:

	struct CharacterGlyph {
		uint8_t		ascii_id;
		uint32_t	x;
		uint32_t	y;
		uint32_t	width;
		uint32_t	height;
		int32_t		xoffset;
		int32_t		yoffset;
		uint32_t	xadvance;
		uint32_t	page;

		CharacterGlyph(uint8_t		_ascii_id,
			uint32_t	_x,
			uint32_t	_y,
			uint32_t	_width,
			uint32_t	_height,
			int32_t		_xoffset,
			int32_t		_yoffset,
			uint32_t	_xadvance,
			uint32_t	_page) :
			ascii_id(_ascii_id),
			x(_x),
			y(_y),
			width(_width),
			height(_height),
			xoffset(_xoffset),
			yoffset(_yoffset),
			xadvance(_xadvance),
			page(_page)
		{}

		CharacterGlyph() { }
	};

	Font(const std::wstring& fnt_file);
	~Font();

	// Get a glyph structure from an ascii.
	// returns false when the ascii does not exist for the font.
	bool GetGlyphFromChr(uint8_t ascii_id, CharacterGlyph& ret);

	const std::wstring& GetGlyphImageFile() const;
	uint32_t GetGlyphWidth() const;
	uint32_t GetGlyphHeight() const;
	uint32_t GetGlyphBase() const;

private:
	bool LoadFontFromFile(const std::wstring& fnt_file);

	template <typename T>
	T GetValue(const std::wstring& line, const std::wstring& valueName, const char breaker = ' ');

	template <typename T>
	bool GetValue(T& value, const std::wstring& line, const std::wstring& valueName);

private:
	std::map<uint32_t, const std::string>	m_glyphTexturesMap;
	std::map<uint8_t, CharacterGlyph>	m_charMap;

	std::wstring	m_glyphImageFileName = L"";

	uint32_t m_glyphWidth = 0;
	uint32_t m_glyphHeight = 0;

	// A glyph base defines the number of pixels in the y-axis above the virtual line of drawing a text
	uint32_t m_glyphBase = 0;
};

#endif // ! FONT_H_

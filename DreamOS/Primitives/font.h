#ifndef FONT_H_
#define FONT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/font.h
// Converting this to FreeType

#include <string>
#include <map>
#include <vector>

#include "Primitives/quad.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

class composite;

class Font {
public:

	struct CharacterGlyph {
		uint8_t		asciiValue = 0;
		uint32_t	x = 0;
		uint32_t	y = 0;
		uint32_t	width = 0;
		uint32_t	height = 0;
		int32_t		xoffset = 0;
		int32_t		yoffset = 0;
		uint32_t	xadvance = 0;
		uint32_t	page = 0;
		bool fValid = false;

		CharacterGlyph() {
			// empty
		}

		CharacterGlyph(std::wstring wstrFontFileLine) {
			asciiValue = GetValue<uint32_t>(wstrFontFileLine, L"char id=");

			if (asciiValue) {
				x = GetValue<uint32_t>(wstrFontFileLine, L"x=");
				y = GetValue<uint32_t>(wstrFontFileLine, L"y=");
				width = GetValue<uint32_t>(wstrFontFileLine, L"width=");
				height = GetValue<uint32_t>(wstrFontFileLine, L"height=");
				xoffset = GetValue<uint32_t>(wstrFontFileLine, L"xoffset=");
				yoffset = GetValue<uint32_t>(wstrFontFileLine, L"yoffset=");
				xadvance = GetValue<uint32_t>(wstrFontFileLine, L"xadvance=");
				page = GetValue<uint32_t>(wstrFontFileLine, L"page=");
				fValid = true;
			}
		}
	};

	Font(bool fDistanceMap = false);
	Font(const std::wstring& wstrFontFile, composite *pContext, bool fDistanceMap = false);
	
	//TODO: not removing this in order to avoid changing DreamConsole
	Font(const std::wstring& wstrFontFile, bool fDistanceMap = false);
	
	~Font();

	// Get a glyph structure from an ASCII.
	// returns false when the ASCII does not exist for the font.
	bool GetGlyphFromChr(uint8_t ascii_id, CharacterGlyph& ret);

	const std::wstring& GetGlyphImageFile() const;
	uint32_t GetGlyphWidth() const;
	uint32_t GetGlyphHeight() const;
	uint32_t GetGlyphBase() const;

private:
	RESULT LoadFontFromFile(const std::wstring& wstrFontFile);

	template <typename T>
	static T GetValue(const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter = ' ');

	template <typename T>
	static RESULT GetValue(T& value, const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter = ' ');

private:
	std::map<uint32_t, const std::string> m_glyphTexturesMap;
	std::map<uint8_t, CharacterGlyph> m_charMap;

	std::wstring m_wstrGlyphImageFilename = L"";

	uint32_t m_glyphWidth = 0;
	uint32_t m_glyphHeight = 0;

	// A glyph base defines the number of pixels in the y-axis above the virtual line of drawing a text
	uint32_t m_glyphBase = 0;

// Distance Mapping
public:
	bool HasDistanceMap();
	float GetBuffer();
	float GetGamma();

	std::shared_ptr<texture> GetTexture();
	RESULT SetTexture(std::shared_ptr<texture> pTexture);
	
private:
	bool m_fDistanceMap;
	float m_buffer = 0.5f;
	float m_gamma = 0.02f;

	std::shared_ptr<texture> m_pTexture;

	// internal freetype stuff (remove above when done)
private:
	RESULT SetFreetypeFace(FT_Face pFTFace);
	FT_Face m_pFTFace = nullptr;

	// Static Freetype Stuff
private:
	static FT_Library m_pFT;
	static bool IsFreetypeInitialized();
	static RESULT InitializeFreetypeLibrary();

public:
	static std::shared_ptr<Font> MakeFreetypeFont(std::wstring wstrFontFilename, bool fDistanceMapped = true);
};

#endif // ! FONT_H_

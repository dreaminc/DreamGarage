#ifndef FONT_H_
#define FONT_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/text/font.h

// Converting this to FreeType

#include <string>
#include <map>
#include <vector>

#include "core/primitives/quad.h"

// Freetype
#include "third_party/Freetype/freetype-2.8/include/ft2build.h"
#include FT_FREETYPE_H  

#define DEFAULT_FONT_SIZE 32
#define FONT_PT_PER_INCH 72.0f
#define FONT_PT_PER_MM (FONT_PT_PER_INCH / 25.4)
#define FONT_PT_PER_M (FONT_PT_PER_MM * 1000.0f)

class composite;
class quad;

class font {
	friend struct CharacterGlyph;

public:
	font(bool fDistanceMap = false);
	font(const std::wstring& wstrFontFile, composite *pContext, bool fDistanceMap = false);
	
	//TODO: not removing this in order to avoid changing DreamConsole
	font(const std::wstring& wstrFontFile, bool fDistanceMap = false);
	
	~font();

	// Get a glyph structure from an ASCII.
	// returns false when the ASCII does not exist for the font.
	RESULT GetGlyphFromChar(uint8_t ascii_id, CharacterGlyph& r_glyph);

	const std::wstring& GetFontImageFile() const;
	uint32_t GetFontTextureWidth() const;
	uint32_t GetFontTextureHeight() const;
	uint32_t GetFontBase() const;
	uint32_t GetFontLineHeight() const;
	
	float GetLineHeight();
	RESULT SetLineHeight(float lineHeight);

private:
	RESULT LoadFontFromFile(const std::wstring& wstrFontFile);

protected:
	template <typename T>
	static T GetValue(const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter = ' ');

	template <typename T>
	static RESULT GetValue(T& value, const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter = ' ');

private:
	std::map<uint32_t, const std::string> m_fontTexturesMap;
	std::map<uint32_t, CharacterGlyph> m_characters;

	std::wstring m_wstrFontImageFilename = L"";

	uint32_t m_fontImageWidth = 0;
	uint32_t m_fontImageHeight = 0;

	// A glyph base defines the number of pixels in the y-axis above the virtual line of drawing a text
	uint32_t m_fontBase = 0;
	uint32_t m_fontLineHeight = 0;
	float m_lineHeight = 0.25f;

	uint32_t m_fontPixelSize = DEFAULT_FONT_SIZE;

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

	std::shared_ptr<texture> m_pTexture = nullptr;

	// internal freetype stuff (remove above when done)
	// TODO: Refactor to remove freetype / or remove all other functions and pathways
private:
	RESULT SetFreetypeFace(FT_Face pFTFace);
	FT_Face m_pFTFace = nullptr;

	// Public Freetype Refactor stuff
public:
	RESULT SetFontSize(uint32_t size);
	uint32_t GetFontSize();
	RESULT LoadFreetypeGlyphs();

	// Static Freetype Stuff
private:
	static FT_Library m_pFT;
	static bool IsFreetypeInitialized();
	static RESULT InitializeFreetypeLibrary();
	static RESULT UninitializeFreetypeLibrary();

public:
	static std::shared_ptr<font> MakeFreetypeFont(std::wstring wstrFontFilename, bool fDistanceMapped = true);
};


// TODO: Move to own file
struct CharacterGlyph {
	uint32_t	value = 0;
	uint32_t	x = 0;
	uint32_t	y = 0;
	uint32_t	width = 0;
	uint32_t	height = 0;
	int32_t		bearingX = 0;
	int32_t		bearingY = 0;
	uint32_t	advance = 0;
	uint32_t	page = 0;
	bool fValid = false;

	CharacterGlyph() {
		// empty
	}

	// TODO: Get rid of this when we move to Freetype
	CharacterGlyph(std::wstring wstrFontFileLine) {
		value = font::GetValue<uint32_t>(wstrFontFileLine, L"char id=");

		if (value) {
			x = font::GetValue<uint32_t>(wstrFontFileLine, L"x=");
			y = font::GetValue<uint32_t>(wstrFontFileLine, L"y=");
			width = font::GetValue<uint32_t>(wstrFontFileLine, L"width=");
			height = font::GetValue<uint32_t>(wstrFontFileLine, L"height=");
			bearingX = font::GetValue<uint32_t>(wstrFontFileLine, L"xoffset=");
			bearingY = font::GetValue<uint32_t>(wstrFontFileLine, L"yoffset=");
			advance = font::GetValue<uint32_t>(wstrFontFileLine, L"xadvance=");
			page = font::GetValue<uint32_t>(wstrFontFileLine, L"page=");
			fValid = true;
		}
	}

	// TODO: Add flag / switch approach this is getting verbose
	// TODO: Split into a different file as well
	float GetWidthMM() {
		return (width / FONT_PT_PER_MM);
	}

	float GetWidthM() {
		return (width / FONT_PT_PER_M);
	}

	float GetHeightMM() {
		return (height / FONT_PT_PER_MM);
	}

	float GetHeightM() {
		return (height / FONT_PT_PER_M);
	}
};

#endif // ! FONT_H_

#include "font.h"

#include <fstream>
#include <sstream>
#include <memory>

#include "Sandbox/PathManager.h"

#include "Primitives/composite.h"

// Freetype Library Stuff
FT_Library font::m_pFT = nullptr;


bool font::IsFreetypeInitialized() {
	if (m_pFT != nullptr)
		return true;

	return false;
}

RESULT font::InitializeFreetypeLibrary() {
	RESULT r = R_PASS;
	FT_Error fte = 0;

	CBM((m_pFT == nullptr), "Freetype already initialized");

	fte = FT_Init_FreeType(&m_pFT);
	CBM((fte == 0), "ERROR::FREETYPE: Could not init FreeType Library err:0x%x", fte);

Error:
	return r;
}

RESULT font::UninitializeFreetypeLibrary() {
	RESULT r = R_PASS;
	FT_Error fte = 0;

	CNM(m_pFT, "Freetype not initialized");

	fte = FT_Done_FreeType(m_pFT);
	CBM((fte == 0), "ERROR::FREETYPE: Could not uninit FreeType Library err:0x%x", fte);

Error:
	return r;
}

// TODO: Cache font faces
// TODO: Check windows folder
// TODO: Check DreamOS font folder
std::shared_ptr<font> font::MakeFreetypeFont(std::wstring wstrFontFilename, bool fDistanceMapped) {
	RESULT r = R_PASS;
	std::shared_ptr<font> pFont = nullptr;
	FT_Error fte = 0;
	char *pszFilepath = nullptr;
	FT_Face pFTFace = nullptr;

	if (IsFreetypeInitialized() == false) {
		CR(InitializeFreetypeLibrary());
	}

	// Create the font object
	pFont = std::make_shared<font>(fDistanceMapped);
	CN(pFont);

	PathManager *pPathManager = PathManager::instance();
	CN(pPathManager);

	pPathManager->GetFilePath(PATH_FONT, wstrFontFilename, pszFilepath);
	CN(pszFilepath);

	// Load the face from FT
	fte = FT_New_Face(m_pFT, pszFilepath, 0, &pFTFace);
	CBM((fte != FT_Err_Unknown_File_Format), "Font %s was read but not supported", pszFilepath);
	CBM((fte == 0), "Font failed to load with error 0x%x", fte);
	CN(pFTFace);
	CR(pFont->SetFreetypeFace(pFTFace));

	// Set up the characters
	CR(pFont->SetFontSize(pFont->GetFontSize()));
	CR(pFont->LoadFreetypeGlyphs());

Error:
	if (RFAILED() && pFont != nullptr) {
		pFont = nullptr;
	}

	if (pszFilepath != nullptr) {
		delete[] pszFilepath;
		pszFilepath = nullptr;
	}

	return pFont;
}


// FONT
// TODO: A lot of this will not be needed when this effort is complete
font::font(bool fDistanceMap) :
	m_fDistanceMap(fDistanceMap)
{
	// empty
}

RESULT font::LoadFreetypeGlyphs() {
	RESULT r = R_PASS;
	FT_Error fte = 0;

	for (FT_Long i = 0; i < m_pFTFace->num_glyphs; i++) {
		FT_Load_Char(m_pFTFace, i, FT_LOAD_RENDER);
		CBM((fte == 0), "Font failed to load char %d with error 0x%x", i, fte);

		CharacterGlyph glyph;
		
		glyph.value = i;
		glyph.width = m_pFTFace->glyph->bitmap.width;
		glyph.height = m_pFTFace->glyph->bitmap.rows;
		glyph.bearingX = m_pFTFace->glyph->bitmap_left;
		glyph.bearingY = m_pFTFace->glyph->bitmap_top;
		glyph.advance = m_pFTFace->glyph->advance.x;

		// TODO: Copy the bitmap buffer?

		// Push into map
		m_characters[i] = glyph;
	}

Error:
	return r;
}

uint32_t font::GetFontSize() {
	return m_fontPixelSize;
}

RESULT font::SetFontSize(uint32_t size) {
	RESULT r = R_PASS;
	FT_Error fte = 0;

	m_fontPixelSize = size;
	fte = FT_Set_Pixel_Sizes(m_pFTFace, 0, m_fontPixelSize);
	CBM((fte == 0), "Set font sizes with error 0x%x", fte);

Error:
	return r;
}

RESULT font::SetFreetypeFace(FT_Face pFTFace) {
	RESULT r = R_PASS;

	CN(pFTFace);
	CBM((m_pFTFace == nullptr), "FT Face already set");

	m_pFTFace = pFTFace;

Error:
	return r;
}

font::font(const std::wstring& strFontFilename, composite *pContext, bool fDistanceMap) :
	m_fDistanceMap(fDistanceMap)
{
	LoadFontFromFile(strFontFilename);

	std::wstring strFile = L"Fonts/" + GetGlyphImageFile();
	const wchar_t* pszFile = strFile.c_str();
	
	m_pTexture = pContext->MakeTexture(const_cast<wchar_t*>(pszFile), texture::TEXTURE_TYPE::TEXTURE_COLOR);
}

font::font(const std::wstring& strFontFilename, bool fDistanceMap) :
	m_fDistanceMap(fDistanceMap)
{
	LoadFontFromFile(strFontFilename);
}

font::~font() {
	if (m_pFTFace != nullptr) {
		FT_Done_Face(m_pFTFace);
		m_pFTFace = nullptr;
	}
}

// TODO: Merge two
template <typename T>
T font::GetValue(const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter) {
	RESULT r = R_PASS;
	T value;

	CR(GetValue(value, wstrLine, wstrValueName, delimiter));

	return value;

Error:
	return T();
}

template <typename T>
RESULT font::GetValue(T& value, const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter) {
	RESULT r = R_PASS;

	auto pos = wstrLine.find(wstrValueName);

	CB((pos < wstrLine.size()));

	{
		pos += wstrValueName.size();
		std::wstring valueString = wstrLine.substr(pos, wstrLine.find(delimiter, pos) - pos);
		std::wistringstream iss(valueString);

		//return !(iss >> std::dec >> value).fail();

		CB(((iss >> std::dec >> value).fail() == false));
	}

Error:
	return r;
}

RESULT font::LoadFontFromFile(const std::wstring& wstrFontFile) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrLine;
	wchar_t* wpszPath = nullptr;

	pPathManager->GetFilePath(PATH_FONT, wstrFontFile.c_str(), wpszPath);

	std::wstring wstrFilePath(wpszPath);
	std::wifstream file(wstrFilePath, std::ios::binary);

	CB((file.is_open()));

	while (std::getline(file, wstrLine)) {

		if (m_glyphWidth == 0) {
			GetValue<uint32_t>(m_glyphWidth, wstrLine, L"scaleW=");
		}

		if (m_glyphHeight == 0) {
			GetValue<uint32_t>(m_glyphHeight, wstrLine, L"scaleH=");
		}

		if (m_glyphBase == 0) {
			GetValue<uint32_t>(m_glyphBase, wstrLine, L"base=");
		}

		if (m_wstrGlyphImageFilename.length() == 0) {
			// GetValue<std::wstring>(m_wstrGlyphImageFilename, wstrLine, L"file=\"", '\"');
			m_wstrGlyphImageFilename = GetValue<std::wstring>(wstrLine, L"file=\"", '\"');
		}

		CharacterGlyph charGlyph(wstrLine);

		if (charGlyph.fValid == true) {
			m_characters[charGlyph.value] = charGlyph;
		}
	}

Error:
	if (wpszPath) {
		delete[] wpszPath;
		wpszPath = nullptr;
	}

	return r;
}

const std::wstring& font::GetGlyphImageFile() const {
	return m_wstrGlyphImageFilename;
}

uint32_t font::GetGlyphWidth() const {
	return m_glyphWidth;
}

uint32_t font::GetGlyphHeight() const {
	return m_glyphHeight;
}

uint32_t font::GetGlyphBase() const {
	return m_glyphBase;
}

RESULT font::GetGlyphFromChar(uint8_t ascii_id, CharacterGlyph& r_glyph) {
	RESULT r = R_PASS;

	auto it = m_characters.find(ascii_id); 
	CB((it != m_characters.end()));				// Ascii does not exist in the glyph
		
	// Retrieve the glyph from the font
	r_glyph = (*it).second;

Error:
	return r;
}

bool font::HasDistanceMap() {
	return m_fDistanceMap;
}

float font::GetBuffer() {
	return m_buffer;
}

float font::GetGamma() {
	return m_gamma;
}

std::shared_ptr<texture> font::GetTexture() {
	return m_pTexture;
}

RESULT font::SetTexture(std::shared_ptr<texture> pTexture) {
	RESULT r = R_PASS;

	CN(pTexture);
	m_pTexture = pTexture;

Error:
	return r;
}

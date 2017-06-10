#include "font.h"

#include <fstream>
#include <sstream>
#include <memory>

#include "Sandbox/PathManager.h"

#include "Primitives/composite.h"

Font::Font(const std::wstring& fnt_file, composite *pContext, bool fDistanceMap) {
	LoadFontFromFile(fnt_file);
	std::wstring strFile = L"Fonts/" + GetGlyphImageFile();
	const wchar_t* pszFile = strFile.c_str();
	m_pTexture = pContext->MakeTexture(const_cast<wchar_t*>(pszFile), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	m_fDistanceMap = fDistanceMap;
}

Font::Font(const std::wstring& fnt_file, bool distanceMap) {
	LoadFontFromFile(fnt_file);
	m_fDistanceMap = distanceMap;
}

Font::~Font() {
	// empty
}

// TODO: Merge two
template <typename T>
T Font::GetValue(const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter) {
	T value;

	auto pos = wstrLine.find(wstrValueName);

	if (pos < wstrLine.size()) {
		pos += wstrValueName.size();
		std::wstring valueString = wstrLine.substr(pos, wstrLine.find(delimiter, pos) - pos);
		std::wistringstream iss(valueString);
		iss >> value;
	}

	return value;
}

template <typename T>
RESULT Font::GetValue(T& value, const std::wstring& wstrLine, const std::wstring& wstrValueName, const char delimiter) {
	RESULT r = R_PASS;

	auto pos = wstrLine.find(wstrValueName);

	CB((pos < wstrLine.size()));

	{
		pos += wstrValueName.size();
		std::wstring valueString = wstrLine.substr(pos, wstrLine.find(' ', pos) - pos);
		std::wistringstream iss(valueString);

		//return !(iss >> std::dec >> value).fail();

		CB(((iss >> std::dec >> value).fail() == false));
	}

Error:
	return r;
}

RESULT Font::LoadFontFromFile(const std::wstring& wstrFontFile) {
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
			m_wstrGlyphImageFilename = GetValue<std::wstring>(wstrLine, L"file=\"", '\"');
		}

		CharacterGlyph charGlyph(wstrLine);

		if (charGlyph.fValid == true) {
			m_charMap[charGlyph.asciiValue] = charGlyph;
		}
	}

Error:
	if (wpszPath) {
		delete[] wpszPath;
		wpszPath = nullptr;
	}

	return r;
}

const std::wstring& Font::GetGlyphImageFile() const {
	return m_wstrGlyphImageFilename;
}

uint32_t Font::GetGlyphWidth() const {
	return m_glyphWidth;
}

uint32_t Font::GetGlyphHeight() const {
	return m_glyphHeight;
}

uint32_t Font::GetGlyphBase() const {
	return m_glyphBase;
}

bool Font::GetGlyphFromChr(uint8_t ascii_id, CharacterGlyph& ret) {
	if (m_charMap.find(ascii_id) == m_charMap.end()) {
		// ascii does not exist in the glyph
		return false;
	}

	ret = m_charMap[ascii_id];

	return true;
}

bool Font::HasDistanceMap() {
	return m_fDistanceMap;
}

float Font::GetBuffer() {
	return m_buffer;
}

float Font::GetGamma() {
	return m_gamma;
}

std::shared_ptr<texture> Font::GetTexture() {
	return m_pTexture;
}

RESULT Font::SetTexture(std::shared_ptr<texture> pTexture) {
	RESULT r = R_PASS;

	CN(pTexture);
	m_pTexture = pTexture;

Error:
	return r;
}

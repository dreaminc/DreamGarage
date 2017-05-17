#include "font.h"

#include <fstream>
#include <sstream>
#include <memory>

#include "Sandbox/PathManager.h"

#include "Primitives/composite.h"

Font::Font(const std::wstring& fnt_file, composite *pContext, bool distanceMap) {
	LoadFontFromFile(fnt_file);
	wchar_t* strFile = (wchar_t*)(L"Fonts/" + GetGlyphImageFile()).c_str();
	m_pTexture = pContext->MakeTexture(strFile, texture::TEXTURE_TYPE::TEXTURE_COLOR);
	m_fDistanceMap = distanceMap;
}

Font::Font(const std::wstring& fnt_file, bool distanceMap)
{
	LoadFontFromFile(fnt_file);
	m_fDistanceMap = distanceMap;
	//m_pTexture = 
	// Error:
	// return false;
}

Font::~Font()
{

}

template <typename T>
T Font::GetValue(const std::wstring& line, const std::wstring& valueName, const char breaker)
{
	T value;

	auto pos = line.find(valueName);

	if (pos < line.size())
	{
		pos += valueName.size();
		std::wstring valueString = line.substr(pos, line.find(breaker, pos) - pos);
		std::wistringstream iss(valueString);
		iss >> value;
	}

	return value;
}

template <typename T>
bool Font::GetValue(T& value, const std::wstring& line, const std::wstring& valueName)
{
	auto pos = line.find(valueName);

	if (pos < line.size())
	{
		pos += valueName.size();
		std::wstring valueString = line.substr(pos, line.find(' ', pos) - pos);
		std::wistringstream iss(valueString);
		return !(iss >> std::dec >> value).fail();
	}

	return false;
}

bool Font::LoadFontFromFile(const std::wstring& fnt_file)
{
	PathManager *pPathManager = PathManager::instance();

	wchar_t* path = nullptr;

	pPathManager->GetFilePath(PATH_FONT, fnt_file.c_str(), path);

	std::wstring	file_path(path);

	if (path)
	{
		delete[] path;
		path = nullptr;
	}

	std::wifstream file(file_path, std::ios::binary);

	if (!file.is_open())
	{
		return false;
	}

	std::wstring line;

	while (std::getline(file, line)) {

		if (m_glyphWidth == 0)
			GetValue<uint32_t>(m_glyphWidth, line, L"scaleW=");

		if (m_glyphHeight == 0)
			GetValue<uint32_t>(m_glyphHeight, line, L"scaleH=");

		if (m_glyphBase == 0)
			GetValue<uint32_t>(m_glyphBase, line, L"base=");

		if (m_glyphImageFileName.length() == 0)
			m_glyphImageFileName = GetValue<std::wstring>(line, L"file=\"", '\"');

		uint32_t ascii_id = 0;

		if (GetValue<uint32_t>(ascii_id, line, L"char id="))
		{
			m_charMap.emplace(ascii_id, CharacterGlyph(ascii_id,
				GetValue<uint32_t>(line, L"x="),
				GetValue<uint32_t>(line, L"y="),
				GetValue<uint32_t>(line, L"width="),
				GetValue<uint32_t>(line, L"height="),
				GetValue<int32_t>(line, L"xoffset="),
				GetValue<int32_t>(line, L"yoffset="),
				GetValue<uint32_t>(line, L"xadvance="),
				GetValue<uint32_t>(line, L"page=")));
		}
	}

	return true;

//Error:
//	return false;
}

const std::wstring& Font::GetGlyphImageFile() const
{
	return m_glyphImageFileName;
}

uint32_t Font::GetGlyphWidth() const
{
	return m_glyphWidth;
}

uint32_t Font::GetGlyphHeight() const
{
	return m_glyphHeight;
}

uint32_t Font::GetGlyphBase() const
{
	return m_glyphBase;
}

bool Font::GetGlyphFromChr(uint8_t ascii_id, CharacterGlyph& ret)
{
	if (m_charMap.find(ascii_id) == m_charMap.end())
	{
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

#include "Utilities.h"

#include <algorithm>
#include <locale>
#include <codecvt>

std::vector<std::string> util::TokenizeString(std::string str, char cDelim) {
	std::istringstream strStream(str);
	std::vector<std::string> strTokens;
	std::string strToken;

	while (std::getline(strStream, strToken, cDelim)) {
		if (!strToken.empty()) {
			strTokens.push_back(strToken);
		}
	}

	return strTokens;
}

void util::tolowerstring(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::string util::WideStringToString(const std::wstring& wstrStr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
	std::string strStr = wstrConverter.to_bytes(wstrStr);
	return strStr;
}

std::wstring util::StringToWideString(const std::string& strStr) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstrConverter;
	std::wstring wstrStr = wstrConverter.from_bytes(strStr);
	return wstrStr;
}
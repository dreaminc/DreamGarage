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

std::vector<std::string> util::TokenizeString(std::string str, std::string strDelim) {
	size_t str_pos = 0;
	std::string strToken;
	std::vector<std::string> strTokens;

	do {
		str_pos = str.find(strDelim); 
		
		strToken = str.substr(0, str_pos);
		strTokens.push_back(strToken);

		str.erase(0, str_pos + strDelim.length());
	} while (str_pos != std::string::npos);

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

char* util::WideCStringToCString(const wchar_t *pwszStr) {
	RESULT r = R_PASS;

	char *pszString = nullptr;

	size_t pwszStr_n = wcslen(pwszStr) + 1;

	pszString = (char*)calloc(pwszStr_n * sizeof(char), 1);
	CN(pszString);

	size_t bytesWritten = wcstombs(pszString, pwszStr, pwszStr_n);
	CB((bytesWritten == (pwszStr_n - 1)));

	return pszString;

Error:
	if (pszString != nullptr) {
		free(pszString);
		pszString = nullptr;
	}

	return nullptr;
}

wchar_t* util::CStringToWideCString(const char *pszStr) {
	RESULT r = R_PASS;

	wchar_t *pwszString = nullptr;

	size_t pszStr_n = strlen(pszStr) + 1;

	pwszString = (wchar_t*)calloc(pszStr_n * sizeof(wchar_t), 1);
	CN(pwszString);

	size_t charsWritten = mbstowcs(pwszString, pszStr, pszStr_n);
	CB((charsWritten == (pszStr_n - 1)));

	return pwszString;

Error:
	if (pwszString != nullptr) {
		free(pwszString);
		pwszString = nullptr;
	}

	return nullptr;
}
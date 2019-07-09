#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Core/Utilities.h
// This is intended as a place to place general purpose utility functions (classes/structs if needed)
// Once enough similar functionality is in here that justifies an object, it should be forked out into 
// an object but there are a variety of random functions that are useful engine wide that can be put here.

#include <sstream>
#include <string>
#include <vector>

namespace util {
	std::vector<std::string> TokenizeString(std::string str, char cDelim);
	std::vector<std::string> TokenizeString(std::string str, std::string strDelim);

	void tolowerstring(std::string& str);
	void toupperstring(std::string& str);

	void tolowerstring(std::wstring& wstr);
	void toupperstring(std::wstring& wstr);

	std::string WideStringToString(const std::wstring& wstrStr);
	std::wstring StringToWideString(const std::string& strStr);

	char* WideCStringToCString(const wchar_t *pwszStr);
	wchar_t* CStringToWideCString(const char *pszStr);

	std::wstring CStringToWideString(const char *pszStr);
	//std::string WideCStringToString(const wchar_t *wpszStr);

	GUID StringToGuid(const std::string& strGUID);
	std::string GuidToString(GUID guid);
	std::wstring GuidToWideString(GUID guid);

	template <class T>
	RESULT Clamp(T &val, const T& low, const T& high) {
		if (val < low)
			val = low;
		else if (val > high)
			val = high;

		return R_PASS;
	}
}

#endif // ! UTILITIES_H_

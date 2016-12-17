#pragma once

#include <string>

namespace RegistryHelper
{
	HKEY OpenKey(HKEY hRootKey, wchar_t* strKey, DWORD access = KEY_ALL_ACCESS);

	bool SetVal(HKEY hKey, LPCTSTR lpValue, const std::wstring& data);

	DWORD GetVal(HKEY hKey, LPCTSTR lpValue);
	std::wstring GetStrVal(HKEY hKey, LPCTSTR lpValue);
	bool ReadRegValue(HKEY root, std::wstring key, std::wstring name, std::wstring& result);

	void SetintVal(HKEY hKey, LPCTSTR lpValue, DWORD data);

	DWORD GetintVal(HKEY hKey, LPCTSTR lpValue);
};

bool InstallRegistryVars();
bool CheckRegistryVars();
bool CheckDev();

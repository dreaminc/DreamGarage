#include "Logger.h"

#include "registry.h"
#include "ProcessExecutor.h"

#include "tchar.h"

// TODO: remove all this hard coded vars and make it more generic

std::multimap<std::wstring, std::pair<std::wstring, std::wstring>> registry
{
	{ L"dreamos",							{ L"", L"\"URL:Dream OS Protocol\"" } },
	{ L"dreamos",							{ L"URL Protocol", L"\"\"" } },
	{ L"dreamos\\DefaultIcon",				{ L"", L"\"dreamos.exe,1\"" } },
	{ L"dreamos\\shell\\open\\command",		{ L"", L"[UPDATEPATH]Update.exe --processStart \"DreamLauncher.exe\" --process-start-args \"%1\"'" } },
};

bool InitRegistry()
{
	std::wstring squirrelUpdateProcess(ProcessExecutor::GetProcessExecutor()->GetParentDir());
	//std::replace(squirrelUpdateProcess.begin(), squirrelUpdateProcess.end(), '\\', '/');

	for (auto& reg : registry)
	{
		size_t pos = reg.second.second.find(L"[UPDATEPATH]");

		if (pos != std::wstring::npos)
		{
			reg.second.second.replace(pos, std::wstring(L"[UPDATEPATH]").length(), squirrelUpdateProcess);
		}
	}

	return true;
}

bool InstallRegistryVars()
{
	if (!InitRegistry())
	{
		LOG(ERROR) << "Registry failed to init";
		return false;
	}

	for (const auto& reg : registry)
	{
		HKEY hKey = RegistryHelper::OpenKey(HKEY_CLASSES_ROOT, (wchar_t*)reg.first.c_str());

		if (hKey == NULL)
		{
			LOG(ERROR) << "registry failed on key - " << reg.first;
			return false;
		}

		if (!RegistryHelper::SetVal(hKey, (wchar_t*)reg.second.first.c_str()/*L""*/, reg.second.second))
		{
			LOG(ERROR) << "registry failed setting - " << reg.first << " value " << reg.second.first << " " << reg.second.second;
			return false;
		}

		LOG(INFO) << "registry set - " << reg.first << " to " << reg.second.first << " " << reg.second.second;;
	}

	return true;
}

bool CheckRegistryVars()
{
	if (!InitRegistry())
	{
		LOG(ERROR) << "registry failed to init";
		return false;
	}

	// checks all registry vars are ok

	std::wstring res;

	for (const auto& reg : registry)
	{
		if (!RegistryHelper::ReadRegValue(HKEY_CLASSES_ROOT, reg.first, reg.second.first/*L""*/, res))
		{
			LOG(ERROR) << "registry failed on key - " << reg.first;
			return false;
		}

		if (res.compare(reg.second.second) != 0)
		{
			LOG(ERROR) << "registry failed - value of " << reg.second.second << " for key " << reg.first;
			return false;
		}
	}

	LOG(ERROR) << "registry ok";

	return true;
}

HKEY RegistryHelper::OpenKey(HKEY hRootKey, wchar_t* strKey, DWORD access)
{
	HKEY hKey;
	LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, access, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND)
	{
		std::cout << "Creating registry key: " << strKey << std::endl;
		nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (nError)
		std::cout << "Error: " << nError << " Could not find or create " << strKey << std::endl;

	return hKey;
}

bool RegistryHelper::SetVal(HKEY hKey, LPCTSTR lpValue, const std::wstring& data)
{
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_SZ, (const BYTE*)data.c_str(), (data.length() + 1) * sizeof(wchar_t));

	if (nError)
		std::cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << std::endl;

	return (nError == 0);
}

DWORD RegistryHelper::GetVal(HKEY hKey, LPCTSTR lpValue)
{
	DWORD data;		
	DWORD size = sizeof(data);	
	DWORD type = REG_DWORD;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		std::cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << std::endl;

	return data;
}

std::wstring RegistryHelper::GetStrVal(HKEY hKey, LPCTSTR lpValue)
{
	std::wstring data;
	DWORD size = 0;
	DWORD type = REG_SZ;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
	{ }
		//data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		std::cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << std::endl;

	return data;
}

bool RegistryHelper::ReadRegValue(HKEY root, std::wstring key, std::wstring name, std::wstring& result)
{
	HKEY hKey;
	if (RegOpenKeyEx(root, key.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD type;
	DWORD cbData;
	if (RegQueryValueEx(hKey, name.c_str(), NULL, &type, NULL, &cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}

	if (type != REG_SZ)
	{
		RegCloseKey(hKey);
		return false;
	}

	std::wstring value(cbData / sizeof(wchar_t), L'\0');
	if (RegQueryValueEx(hKey, name.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	size_t firstNull = value.find_first_of(L'\0');
	if (firstNull != std::string::npos)
		value.resize(firstNull);

	result = value;

	return true;
}

void RegistryHelper::SetintVal(HKEY hKey, LPCTSTR lpValue, DWORD data)
{
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));

	if (nError)
		std::cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << std::endl;
}

DWORD RegistryHelper::GetintVal(HKEY hKey, LPCTSTR lpValue)
{
	DWORD data;
	DWORD size = sizeof(data);
	DWORD type = REG_DWORD;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0;
	//SetVal() is called.
	else if (nError)
		std::cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << std::endl;

	return data;
}

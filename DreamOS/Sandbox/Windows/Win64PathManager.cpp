#include "Win64PathManager.h"
#include <stdlib.h>
#include <cstring>

Win64PathManager::Win64PathManager() :
	PathManager()	// Call super
{
	memset(m_pszDreamRootPath, 0, (sizeof(wchar_t) * MAX_PATH));
	m_pszDreamRootPath_n = 0;

	memset(m_pszCurDirectiory, 0, (sizeof(wchar_t) * MAX_PATH));
	m_pszCurDirectiory_n = 0;

	memset(m_pszShadersPath, 0, (sizeof(wchar_t) * MAX_PATH));
	m_pszShadersPath_n = 0;

	// Set up the path values
	
}


RESULT Win64PathManager::Dealloc() {
	RESULT r = R_PASS;

	// Empty for now

Error:
	return r;
}

/*
RESULT Win64PathManager::RegisterPath(wchar_t *pszName, wchar_t *pszValue) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Found Name:%S Value:%S", pszName, pszValue);

Error:
	return r;
}
*/

// This file runs in current directory
RESULT Win64PathManager::OpenDreamPathsFile() {
	RESULT r = R_PASS;

	FILE *pFile = NULL;
	wchar_t szLine[MAX_PATH];

	errno_t err = fopen_s(&pFile, DREAM_OS_PATHS_FILE, "r");
	CNM(pFile, "OpenDreamPathsFile failed to open file %S%s", m_pszCurDirectiory, DREAM_OS_PATHS_FILE);
	rewind(pFile);

	while(fgetws(szLine, MAX_PATH, pFile) != NULL) {
		wchar_t *pszContext = NULL;
		wchar_t *pszName = wcstok_s(szLine, _T(" \t\n\v\f\r"), &pszContext);
		
		if (pszName == NULL || pszName[0] == '#')
			continue;
		
		wchar_t *pszValue = wcstok_s(NULL, _T(" \t\n\v\f\r"), &pszContext);
		CRM(RegisterPath(pszName, pszValue), "Failed to register value %S", pszName);
	}
		
Error:
	if (pFile != NULL) {
		fclose(pFile);
		pFile = NULL;
	}
	return r;
}

RESULT Win64PathManager::GetCurrentPath(wchar_t *& pszCurrentPath) {
	RESULT r = R_PASS;

	pszCurrentPath = NULL;

	CNM(m_pszCurDirectiory, "CurDirectory NULL");
	pszCurrentPath = m_pszCurDirectiory;
	
Error:
	return r;
}

RESULT Win64PathManager::GetDreamPath(wchar_t*&pszDreamPath) {
	RESULT r = R_PASS;

	pszDreamPath = NULL;

	CNM(m_pszDreamRootPath, "DreamRootPath NULL");
	pszDreamPath = m_pszDreamRootPath;

Error:
	return r;
}

RESULT Win64PathManager::SetCurrentPath(wchar_t *pszPath) {
	RESULT r = R_PASS;

	CBM(SetCurrentDirectory(pszPath), "Failed to set path to %S", pszPath);
	//wcscpy(m_pszCurDirectiory, pszPath);
	errno_t err = wcscpy_s(m_pszCurDirectiory, pszPath);
	DEBUG_LINEOUT("Switched current directory to %S", m_pszCurDirectiory);

Error:
	return r;
}

RESULT Win64PathManager::UpdateCurrentPath() {
	RESULT r = R_PASS;

	// Initialize Current Path
	DWORD dwReturn = GetCurrentDirectory(MAX_PATH, m_pszCurDirectiory);
	DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);

Error:
	return r;
}

RESULT Win64PathManager::InitializePaths() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Win64PathManager Initialize Paths");

	// Dream Root Path - Try to find environment variable to set paths
	char *pszDreamPath = NULL;
	size_t pszDreamPath_n = 0;
	errno_t err = _dupenv_s(&pszDreamPath, &pszDreamPath_n, DREAM_OS_PATH_ENV);
	if (pszDreamPath != NULL) {
		DEBUG_LINEOUT("Found %s env variable: %s", DREAM_OS_PATH_ENV, pszDreamPath);

		//mbstowcs(m_pszDreamRootPath, pszDreamPath, pszDreamPath_n);
		mbstowcs_s(&m_pszDreamRootPath_n, m_pszDreamRootPath, pszDreamPath, pszDreamPath_n);
		CRM(SetCurrentPath(m_pszDreamRootPath), "Failed to set current path to dream root");

		CRM(OpenDreamPathsFile(), "Failed to find Dream Paths file");
	}
	else {
		// Try to back pedal to find dreampaths.txt
		DEBUG_LINEOUT("%s env variable not found", DREAM_OS_PATH_ENV);
	}

	

Error:
	return r;
}

RESULT Win64PathManager::DoesPathExist(const wchar_t *pszPath) {
	RESULT r = R_PATH_NOT_FOUND;
	DWORD fileAttributes = GetFileAttributesW(pszPath);
	
	if (fileAttributes != INVALID_FILE_ATTRIBUTES) 
		if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return R_DIRECTORY_FOUND;
		else
			return R_FILE_FOUND;

	return r;
}

RESULT Win64PathManager::PrintPaths() {
	RESULT r = R_PASS;

	// Some debug / path management
	DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);
	DEBUG_LINEOUT("Dream Root Path %S", m_pszDreamRootPath);

Error:
	return r;
}

// Make note that this will allocate new strings and push them into the directory list
RESULT Win64PathManager::GetListOfDirectoriesInPath(PATH_VALUE_TYPE type, std::list<wchar_t*>* pListDirs) {
	RESULT r = R_PASS;
	wchar_t *pszValuePath = NULL;
	long pszValuePath_n = 0;

	wchar_t *pszValueFindPath = NULL;
	long pszValueFindPath_n = 0;

	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fileFindData;
	int i = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = wcslen(pszValuePath);
	CRM(DoesPathExist(pszValuePath), "Path %S does not exist", pszValuePath);
	CBM((r == R_DIRECTORY_FOUND), "Invalid return from DoesPathExist");

	// FindFirstFile call requires some characters at the end
	pszValueFindPath_n = pszValuePath_n + 2;
	pszValueFindPath = new wchar_t[pszValueFindPath_n];
	memset(pszValueFindPath, 0, sizeof(wchar_t) * pszValueFindPath_n);
	wcscat(pszValueFindPath, pszValuePath);
	wcscat(pszValueFindPath, L"*");

	// Find all directories in path
	hFind = FindFirstFile(pszValueFindPath, &fileFindData);

	//hFind = FindFirstFile(L"D:\\dev\\DreamGarage\\DreamOS./HAL/opengl/GLSL/*", &fileFindData);
	CBM((hFind != INVALID_HANDLE_VALUE), "Could not find anything");

	do {
		if (fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (wcscmp(L".", fileFindData.cFileName) != 0 && wcscmp(L"..", fileFindData.cFileName) != 0) {
				long pszFoundDir_n = wcslen(fileFindData.cFileName) + 1;
				wchar_t *pszFoundDir = new wchar_t[pszFoundDir_n];
				
				memset(pszFoundDir, 0, sizeof(wchar_t) * pszFoundDir_n);
				memcpy(pszFoundDir, fileFindData.cFileName, sizeof(wchar_t) * pszFoundDir_n);
				
				pListDirs->push_back(pszFoundDir);
			}
		}
	} while (FindNextFile(hFind, &fileFindData) != 0);

Error:
	if (pszValuePath != NULL) {
		delete[] pszValuePath;
		pszValuePath = NULL;
	}

	if (pszValueFindPath != NULL) {
		delete[] pszValueFindPath;
		pszValueFindPath = NULL;
	}

	FindClose(hFind);

	return r;
}
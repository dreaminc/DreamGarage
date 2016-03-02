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


RESULT Win64PathManager::PrintPaths() {
	RESULT r = R_PASS;

	// Some debug / path management
	DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);
	DEBUG_LINEOUT("Dream Root Path %S", m_pszDreamRootPath);
	

Error:
	return r;
}
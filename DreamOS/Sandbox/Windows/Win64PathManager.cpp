#include "Win64PathManager.h"
#include <stdlib.h>

Win64PathManager::Win64PathManager() :
	PathManager()	// Call super
{
	memset(m_pszDreamRootPath, 0, (sizeof(wchar_t) * MAX_PATH));
	m_pszDreamRootPath_n = 0;

	memset(m_pszCurDirectiory, 0, (sizeof(wchar_t) * MAX_PATH));
	m_pszCurDirectiory_n = 0;

	memset(m_pszShadersPath, 0, (sizeof(wchar_t) * MAX_PATH));
	m_pszShadersPath_n = 0;
}


RESULT Win64PathManager::Dealloc() {
	RESULT r = R_PASS;

	// Empty for now

Error:
	return r;
}

// This file runs in current directory
RESULT Win64PathManager::OpenDreamPathsFile() {
	RESULT r = R_PASS;

	FILE *pFile = NULL;
	errno_t err = fopen_s(&pFile, DREAM_OS_PATHS_FILE, "r");
	CNM(pFile, "OpenDreamPathsFile failed to open file %S\\%s", m_pszCurDirectiory, DREAM_OS_PATHS_FILE);

Error:
	return r;
}

RESULT Win64PathManager::InitializePaths() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Win64PathManager Initialize Paths");

	// Dream Root Path
	// Try to find environment variable to set paths
	//const char *cpszDreamPath = getenv(DREAM_OS_PATH_ENV);
	char *pszDreamPath = NULL;
	size_t pszDreamPath_n = 0;
	errno_t err = _dupenv_s(&pszDreamPath, &pszDreamPath_n, DREAM_OS_PATH_ENV);
	if (pszDreamPath != NULL) {
		DEBUG_LINEOUT("Found %s env variable: %s", DREAM_OS_PATH_ENV, pszDreamPath);

		//mbstowcs(m_pszDreamRootPath, pszDreamPath, pszDreamPath_n);
		mbstowcs_s(&m_pszDreamRootPath_n, m_pszDreamRootPath, pszDreamPath, pszDreamPath_n);
		SetCurrentDirectory(m_pszDreamRootPath);
		DEBUG_LINEOUT("Set current directory Dream Root %S", m_pszDreamRootPath);

		CRM(OpenDreamPathsFile(), "Failed to find Dream Paths file");
	}
	else {
		// Try to back pedal to find dreampaths.txt
		DEBUG_LINEOUT("%s env variable not found", DREAM_OS_PATH_ENV);
	}

	// Initialize Current Path
	DWORD dwReturn = GetCurrentDirectory(MAX_PATH, m_pszCurDirectiory);
	DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);

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
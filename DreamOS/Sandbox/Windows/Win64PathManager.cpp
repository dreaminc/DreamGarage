#include "Win64PathManager.h"
#include <stdlib.h>

Win64PathManager::Win64PathManager() :
	PathManager()	// Call super
{
	// Empty for now
}


RESULT Win64PathManager::Dealloc() {
	RESULT r = R_PASS;

	// Empty for now

Error:
	return r;
}

RESULT Win64PathManager::InitializePaths() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Win64PathManager Initialize Paths");

	// Initialize Current Path
	DWORD dwReturn = GetCurrentDirectory(MAX_PATH, m_pszCurDirectiory);

	// Dream Root Path


Error:
	return r;
}


RESULT Win64PathManager::PrintPaths() {
	RESULT r = R_PASS;

	// Some debug / path management
	DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);

	// Try to find environment variable to set paths
	//const char *cpszDreamPath = getenv(DREAM_OS_PATH_ENV);
	char *pszDreamPath = NULL;
	size_t pszDreamPath_n = 0;
	errno_t err = _dupenv_s(&pszDreamPath, &pszDreamPath_n, DREAM_OS_PATH_ENV);
	if (pszDreamPath != NULL) {
		DEBUG_LINEOUT("Found %s env variable: %s", DREAM_OS_PATH_ENV, pszDreamPath);
	}
	else {
		// Try to back pedal to find dreampaths.txt
		DEBUG_LINEOUT("%s env variable not found", DREAM_OS_PATH_ENV);
	}

Error:
	return r;
}
#ifndef WIN_64_PATH_MANAGER_H_
#define WIN_64_PATH_MANAGER_H_

#include "RESULT/EHM.h"
#include "Sandbox/PathManager.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64PathManager.h
// Dream OS Win64 sandbox path manager

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

class Win64PathManager : public PathManager {
public:
	Win64PathManager();

protected:
	RESULT Dealloc();
	RESULT InitializePaths();
	RESULT OpenDreamPathsFile();
	RESULT UpdateCurrentPath();
	RESULT SetCurrentPath(wchar_t *pszPath);

public:
	RESULT PrintPaths();

private:
	wchar_t m_pszCurDirectiory[MAX_PATH];
	size_t m_pszCurDirectiory_n;

	wchar_t m_pszDreamRootPath[MAX_PATH];
	size_t m_pszDreamRootPath_n;

	wchar_t m_pszShadersPath[MAX_PATH];
	size_t m_pszShadersPath_n;
};

#endif // ! WIN_64_PATH_MANAGER_H_

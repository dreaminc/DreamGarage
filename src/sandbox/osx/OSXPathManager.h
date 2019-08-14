#ifndef OSX_PATH_MANAGER_H_
#define OSX_PATH_MANAGER_H_

#include "core/ehm/EHM.h"

// Dream Sandbox OSX
// dos/src/sandbox/osx/OSXPathManager.h

// Dream OS OSX sandbox path manager

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "Sandbox/PathManager.h"

#ifndef MAX_PATH_LENGTH
	#define MAX_PATH_LENGTH_PM PATH_MAX
#endif

class OSXPathManager : public PathManager {
public:
    OSXPathManager();
    
protected:
    RESULT Dealloc();
    RESULT InitializePaths();
    RESULT OpenDreamPathsFile();
    RESULT UpdateCurrentPath();
    RESULT SetCurrentPath(wchar_t *pszPath);
    //RESULT RegisterPath(wchar_t *pszName, wchar_t *pszValue);
    
public:
    RESULT PrintPaths();
    
    RESULT GetCurrentPath(wchar_t*&pszCurrentPath);
    RESULT GetDreamPath(wchar_t*&pszDreamPath);
    //RESULT GetValuePath(PATH_VALUE_TYPE, wchar_t* &pszPath);
    
private:
    wchar_t m_pszCurDirectiory[MAX_PATH_LENGTH_PM];
    size_t m_pszCurDirectiory_n;
    
    wchar_t m_pszDreamRootPath[MAX_PATH_LENGTH_PM];
    size_t m_pszDreamRootPath_n;
    
    wchar_t m_pszShadersPath[MAX_PATH_LENGTH_PM];
    size_t m_pszShadersPath_n;
};

#endif // ! OSX_PATH_MANAGER_H_

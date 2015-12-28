#ifndef PATH_MANAGER_H_
#define PATH_MANAGER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"

// DREAM OS
// DreamOS/Sandbox/PathManager.h
// Dream OS Sandbox path manager - this class effectively manages the given paths of the Sandbox
// and ultimately may or may not be used for the native implementation

// This should be used to scrape Dream OS path
#define DREAM_OS_PATH_ENV "DREAMOSPATH"
#define DREAM_OS_PATHS_FILE "dreampaths.txt"	// TODO: Rename?

#include <map>

class PathManagerFactory;

typedef enum {
	PATH_ROOT = 0,
	PATH_HAL,		
	PATH_SHADERS,
	PATH_SANDBOX,
	PATH_RESULT,
	PATH_INVALID	// Also acts as a found
} PATH_VALUE_TYPE;

class PathManager {
	friend class PathManagerFactory;

	const wchar_t *m_cszPathValues[PATH_INVALID] = {
		L"ROOT",
		L"HAL",
		L"SHADERS",
		L"SANDBOX",
		L"RESULT"
	};

	int m_cszPathValues_n;

public:
	PathManager();
	~PathManager();

protected:
	virtual RESULT Dealloc();					
	virtual RESULT OpenDreamPathsFile() = 0;
	virtual RESULT UpdateCurrentPath() = 0;
	virtual RESULT SetCurrentPath(wchar_t *pszPath) = 0;

	RESULT RegisterPath(wchar_t *pszName, wchar_t *pszValue);
	
	virtual RESULT InitializePaths();
	PATH_VALUE_TYPE GetPathValueType(wchar_t *pszValue);

public:
	// Print Paths
	virtual RESULT PrintPaths() = 0;

	virtual RESULT GetCurrentPath(wchar_t*&pszCurrentPath) = 0;
	virtual RESULT GetDreamPath(wchar_t*&pszDreamPath) = 0;
	RESULT GetValuePath(PATH_VALUE_TYPE type, wchar_t* &n_pszPath);

private:
	UID m_uid;
	std::map<PATH_VALUE_TYPE, wchar_t*> *m_pmapNVPPaths;
};

#endif // ! PATH_MANAGER_H_

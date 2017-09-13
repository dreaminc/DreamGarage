#ifndef PATH_MANAGER_H_
#define PATH_MANAGER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Sandbox/PathManager.h
// Dream OS Sandbox path manager - this class effectively manages the given paths of the Sandbox
// and ultimately may or may not be used for the native implementation

// This should be used to scrape Dream OS path
#define DREAM_OS_PATH_ENV "DREAMOSPATH"
#define DREAM_OS_PATHS_FILE "dreampaths.txt"	// TODO: Rename?
#define DREAM_OS_PATH_ROOT "dreamos"
#define DREAM_OS_PATH_WROOT L"dreamos"

#include <list>
#include <map>
#include <vector>
#include "Primitives/version.h"

class PathManagerFactory;

typedef enum {
	PATH_ROOT = 0,
	PATH_HAL,		
	PATH_SHADERS,
	PATH_SANDBOX,
	PATH_RESULT,
	PATH_TEXTURE,
	PATH_TEXTURE_CUBE,
	PATH_FONT,
	PATH_MODEL,
	PATH_DATA,
	PATH_INVALID	// Also acts as a found
} PATH_VALUE_TYPE;

// This sets the configuration that version paths will be preceded by
// the letter v as in "v123\" vs "123\" for example
#define PATH_VERSION_PATH_WITH_V true

class PathManager : public valid {
	friend class PathManagerFactory;

	const wchar_t *m_cszPathValues[PATH_INVALID] = {
		L"ROOT",
		L"HAL",
		L"SHADERS",
		L"SANDBOX",
		L"RESULT",
		L"TEXTURE",
		L"TEXTURE_CUBE",
		L"FONT",
		L"MODEL",
		L"DATA"
	};

	int m_cszPathValues_n;

public:
	PathManager();
	~PathManager();

	const wchar_t *GetPathValueString(PATH_VALUE_TYPE type) {
		return m_cszPathValues[type];
	}

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
	RESULT IsPathRegistered(PATH_VALUE_TYPE type);

	virtual RESULT GetCurrentPath(wchar_t*&pszCurrentPath) = 0;
	virtual RESULT GetDreamPath(wchar_t*&pszDreamPath) = 0;
	virtual RESULT GetDreamPath(char* &n_pszDreamPath) = 0;
	virtual RESULT GetDreamPath(std::wstring &r_wstrDreamPath) = 0;
	
	RESULT GetVersionFolder(version ver, wchar_t* &n_pszVersionFolder);

	RESULT GetValuePath(PATH_VALUE_TYPE type, wchar_t* &n_pszPath);
	RESULT GetValuePath(PATH_VALUE_TYPE type, char* &n_pszPath);
	RESULT GetValuePath(PATH_VALUE_TYPE type, std::wstring &r_wstrPath);

	RESULT GetValuePathVersion(PATH_VALUE_TYPE type, version ver, wchar_t* &n_pszVersionPath);

	RESULT GetFilePath(PATH_VALUE_TYPE type, const wchar_t *pszFileName, wchar_t* &n_pszFilePath);
	RESULT GetFilePath(PATH_VALUE_TYPE type, std::wstring wstrFilename, char* &n_pszFilePath);

	RESULT GetFilePath(PATH_VALUE_TYPE type, std::wstring wstrFilename, wchar_t* &n_pszFilePath);
	RESULT GetFilePath(PATH_VALUE_TYPE type, std::wstring wstrFilename, std::wstring &r_wstrFilePath);

	RESULT GetFilePathVersion(PATH_VALUE_TYPE type, version ver, const wchar_t *pszFileName, wchar_t * &n_pszVersionFilePath);
	RESULT GetFilePathWithFolder(PATH_VALUE_TYPE type, const wchar_t *pszFolderName, std::wstring &strPathWithFolder);

	// std::wstring version
	std::wstring GetFilePath(PATH_VALUE_TYPE type, const wchar_t *pszFileName);

	std::wstring GetDirectoryPathFromFilePath(std::wstring wstrFilePath);

	bool IsRootPath(wchar_t *pwszRoot, wchar_t *pwszFilename);
	bool IsAbsolutePath(wchar_t *pwszFilename);
	bool IsDreamPath(wchar_t *pwszFilename);

	virtual RESULT DoesPathExist(const wchar_t *pszPath) = 0;
	virtual RESULT GetListOfDirectoriesInPath(PATH_VALUE_TYPE type, std::list<wchar_t*>* pListDirs) = 0;
	virtual RESULT GetListOfFilesInPath(std::wstring strNameDir, std::vector<std::wstring> &vstrFiles, const wchar_t *pszOptExtension = nullptr) = 0;

	RESULT DoesPathExist(PATH_VALUE_TYPE type);
	RESULT DoesPathExist(std::wstring wstrPath, bool fCheckFile = false);
	RESULT DoesFileExist(PATH_VALUE_TYPE type, const wchar_t *pszFileName);
	RESULT GetFileVersionThatExists(PATH_VALUE_TYPE type, version versionFile, const wchar_t *pszFileName, version *versionFileExists);

	RESULT GetFilesForNameInPath(PATH_VALUE_TYPE type, const wchar_t *pszName, std::vector<std::wstring> &vstrFiles, const wchar_t *pszOptExtension = nullptr);
	RESULT GetFilePathForName(PATH_VALUE_TYPE type, const wchar_t *pszName, std::wstring strFilename, std::wstring &strFilePath);

private:
	UID m_uid;
	std::map<PATH_VALUE_TYPE, wchar_t*> *m_pmapNVPPaths;
	
	// TODO: Replace with Singleton pattern / manager
	// Singleton Usage
protected:
	static PathManager *m_pInstance;

	static RESULT SetSingletonPathManager(PathManager *pInstance) {
		if (m_pInstance != NULL) {
			delete m_pInstance;
			m_pInstance = NULL;
		}

		m_pInstance = pInstance;
		return R_PASS;
	}

public:
	static PathManager *instance() {
		if (m_pInstance)
			return m_pInstance;
		else
			return NULL;
	}
};

#endif // ! PATH_MANAGER_H_

#ifndef ANDROID_PATH_MANAGER_H_
#define ANDROID_PATH_MANAGER_H_

#include "core/ehm/EHM.h"

// Dream Sandbox Android
// dos/src/sandbox/android/AndroidPathManager.h

// Dream OS Android sandbox path manager

#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "Sandbox/PathManager.h"

class DreamOS;

class AndroidPathManager : public PathManager {
public:
	AndroidPathManager();

	RESULT DoesPathExist(const wchar_t *pszPath);
	RESULT GetListOfDirectoriesInPath(PATH_VALUE_TYPE type, std::list<wchar_t*>* pListDirs);
	RESULT GetListOfFilesInPath(std::wstring strNameDir, std::vector<std::wstring> &vstrFiles, const wchar_t *pszOptExtension = nullptr);

protected:
	virtual RESULT Dealloc() override;
	virtual RESULT InitializePaths(DreamOS *pDOSHandle) override;
	virtual RESULT OpenDreamPathsFile() override;
	virtual RESULT UpdateCurrentPath() override;
	virtual RESULT SetCurrentPath(wchar_t *pszPath) override;

	virtual RESULT CreateDirectory(wchar_t *pszDirectoryName) override;

public:
	RESULT PrintPaths();

	RESULT GetCurrentPath(wchar_t*&pszCurrentPath);

	virtual RESULT GetDreamPath(std::wstring &wstrAppDataPath, DREAM_PATH_TYPE pathValueType) override;
	virtual RESULT GetDreamPath(wchar_t*&pszDreamPath) override;
	virtual RESULT GetDreamPath(char* &n_pszDreamPath) override;
	virtual RESULT GetDreamPath(std::wstring &r_wstrDreamPath) override;

private:
	wchar_t m_pszCurDirectiory[MAX_PATH];
	size_t m_pszCurDirectiory_n;

	wchar_t m_pszDreamRootPath[MAX_PATH];
	size_t m_pszDreamRootPath_n;

	wchar_t m_pszShadersPath[MAX_PATH];
	size_t m_pszShadersPath_n;
};

#endif // ! ANDROID_PATH_MANAGER_H_

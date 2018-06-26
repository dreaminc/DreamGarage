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

	RESULT DoesPathExist(const wchar_t *pszPath);
	RESULT GetListOfDirectoriesInPath(PATH_VALUE_TYPE type, std::list<wchar_t*>* pListDirs);
	RESULT GetListOfFilesInPath(std::wstring strNameDir, std::vector<std::wstring> &vstrFiles, const wchar_t *pszOptExtension = nullptr);

protected:
	virtual RESULT Dealloc() override;
	virtual RESULT InitializePaths() override;
	virtual RESULT OpenDreamPathsFile() override;
	virtual RESULT UpdateCurrentPath() override;
	virtual RESULT SetCurrentPath(wchar_t *pszPath) override;
	//RESULT RegisterPath(wchar_t *pszName, wchar_t *pszValue);

	virtual RESULT CreateDirectory(wchar_t *pszDirectoryName) override;

public:
	RESULT PrintPaths();

	RESULT GetCurrentPath(wchar_t*&pszCurrentPath);
	virtual RESULT GetAppDataPath(std::wstring &wstrAppDataPath, PATH_VALUE_TYPE pathValueType) override;
	virtual RESULT GetDreamPath(wchar_t*&pszDreamPath) override;
	virtual RESULT GetDreamPath(char* &n_pszDreamPath) override;
	virtual RESULT GetDreamPath(std::wstring &r_wstrDreamPath) override;
	//RESULT GetValuePath(PATH_VALUE_TYPE, wchar_t* &pszPath);

private:
	wchar_t m_pszCurDirectiory[MAX_PATH];
	size_t m_pszCurDirectiory_n;

	wchar_t m_pszDreamRootPath[MAX_PATH];
	size_t m_pszDreamRootPath_n;

	wchar_t m_pszShadersPath[MAX_PATH];
	size_t m_pszShadersPath_n;
};

#endif // ! WIN_64_PATH_MANAGER_H_

#include "PathManager.h"
#include "Primitives/Types/Number.h"

#include "Core/Utilities.h"

// Initialize and allocate the instance
PathManager* PathManager::m_pInstance = nullptr;

PathManager::PathManager() :
	m_pmapNVPPaths(nullptr)
{
	RESULT r = R_PASS;

	m_cszPathValues_n = sizeof(m_cszPathValues) / sizeof(m_cszPathValues[0]);

	m_pmapNVPPaths = new std::map<PATH_VALUE_TYPE, wchar_t*>();
	ACNM(m_pmapNVPPaths, "Failed to allocate paths map");

	Validate();
	return;
}

PathManager::~PathManager() {
	ACRM(Dealloc(), "PathManager failed to deallocate paths");

	// Dealloc the map
	if (m_pmapNVPPaths != nullptr) {
		std::map<PATH_VALUE_TYPE, wchar_t*>::iterator it = m_pmapNVPPaths->begin();
		while (it != m_pmapNVPPaths->end()) {
			delete[] it->second;
			it++;
		}

		delete m_pmapNVPPaths;
		m_pmapNVPPaths = nullptr;
	}
}

RESULT PathManager::Dealloc() {
	return R_NOT_IMPLEMENTED;
}

RESULT PathManager::RegisterPath(wchar_t *pszName, wchar_t *pszValue) {
	RESULT r = R_PASS;
	std::pair<std::map<PATH_VALUE_TYPE, wchar_t*>::iterator, bool>retVal;
	wchar_t *pszPath = nullptr;

    long pszValueCopy_n = 0;
    wchar_t *pszValueCopy = nullptr;
    //errno_t err;

	DEBUG_LINEOUT("Registering Name:%S Value:%S", pszName, pszValue);
	PATH_VALUE_TYPE pathValueType = GetPathValueType(pszName);
	CBM((pathValueType != PATH_INVALID), "Not a valid path value type");

	// Copy the value - save the memory
	pszValueCopy_n = static_cast<long>(wcslen(pszValue) + 1);
	pszValueCopy = new wchar_t[pszValueCopy_n];
	memset(pszValueCopy, 0, sizeof(wchar_t) * pszValueCopy_n);
    
    CNM(wcscpy(pszValueCopy, pszValue), "Failed to copy over value");

	retVal = m_pmapNVPPaths->insert(std::pair<PATH_VALUE_TYPE, wchar_t*>(pathValueType, pszValueCopy));
	CBM((retVal.second != false), "Failed to insert NVP into Paths map");

	CRM(GetValuePath(pathValueType, pszPath), "Failed to get path value");
	DEBUG_LINEOUT("Registered type:%d Value:%S", pathValueType, pszPath);

Error:
	if (pszPath != nullptr) {
		delete[] pszPath;
		pszPath = nullptr;
	}

	return r;
}

// Set up path manager
RESULT PathManager::InitializePaths(DreamOS *pDOSHandle) {
	DEBUG_LINEOUT("PathManager Initialize Paths");
	return R_VIRTUAL;
}

PATH_VALUE_TYPE PathManager::GetPathValueType(wchar_t *pszValue) {
	for (int i = 0; i < m_cszPathValues_n; i++)
		if (wcscmp(m_cszPathValues[i], pszValue) == 0)
			return (PATH_VALUE_TYPE)(i);

	return PATH_INVALID;
}

RESULT PathManager::IsPathRegistered(PATH_VALUE_TYPE type) {
	RESULT r = R_PASS;

	// Check to see we have the type registered 
	std::map<PATH_VALUE_TYPE, wchar_t*>::iterator it = m_pmapNVPPaths->find(type);
	CBM((it != m_pmapNVPPaths->end()), "Path Value %d not found in map", type);

Error:
	return r;
}

// TODO: Move this into the version object?
RESULT PathManager::GetVersionFolder(version ver, wchar_t* &n_pszVersionFolder) {
	RESULT r = R_PASS;

	// Length of digits of major, minor, double minor and the forward slash as well as null terminator
	long n_pszVersionFolder_n = 0; 
	// DEBUG:
	n_pszVersionFolder_n += Number::DigitCount(ver.major());
	n_pszVersionFolder_n += Number::DigitCount(ver.minor()); 
	n_pszVersionFolder_n += Number::DigitCount(ver.doubleminor());
	n_pszVersionFolder_n += 2;

	bool fPathVersionWithV = PATH_VERSION_PATH_WITH_V;

	// Add one for the 'v' if applicable
	if (fPathVersionWithV)
		n_pszVersionFolder_n++;

	n_pszVersionFolder = new wchar_t[n_pszVersionFolder_n];
	CN(n_pszVersionFolder);

	memset(n_pszVersionFolder, 0, sizeof(wchar_t) * n_pszVersionFolder_n);

	if (fPathVersionWithV) {
		swprintf(n_pszVersionFolder, n_pszVersionFolder_n, L"v%d%d%d/", ver.major(), ver.minor(), ver.doubleminor());
	}
	else {
		swprintf(n_pszVersionFolder, n_pszVersionFolder_n, L"%d%d%d/", ver.major(), ver.minor(), ver.doubleminor());
	}

Error:
	return r;
}

RESULT PathManager::GetValuePath(PATH_VALUE_TYPE type, std::wstring &r_wstrPath) {
	RESULT r = R_PASS;

	CRM(IsPathRegistered(type), "Value not registered");

	r_wstrPath.clear();

	CRM(GetDreamPath(r_wstrPath), "Failed to acquire dream path");

	r_wstrPath += m_pmapNVPPaths->at(type);

Error:
	return r;
}

RESULT PathManager::GetValuePath(PATH_VALUE_TYPE type, char* &n_pszPath) {
	RESULT r = R_PASS;

	size_t n_pszPath_n = 0;

	char *pszDreamPath = nullptr;
	size_t pszDreamPath_n = 0;

	char *pszValue = nullptr;
	size_t pszValue_n = 0;

	CRM(IsPathRegistered(type), "Value not registered");

	// Convert to char*
	// TODO: add to util?
	pszValue_n = wcslen(m_pmapNVPPaths->at(type));
	pszValue = new char[pszValue_n];
	memset(pszValue, 0, pszValue_n);
	std::wcstombs(pszValue, m_pmapNVPPaths->at(type), pszValue_n);

	CRM(GetDreamPath(pszDreamPath), "Failed to acquire dream path");
	pszDreamPath_n = strlen(pszDreamPath);

	n_pszPath_n = pszDreamPath_n + 1 + pszValue_n + 1;
	n_pszPath = new char[n_pszPath_n];
	memset(n_pszPath, 0, sizeof(char) * n_pszPath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	// TODO: This breaks cross-platform-ness
	strncat(n_pszPath, pszDreamPath, pszDreamPath_n);
	strncat(n_pszPath, pszValue, pszValue_n);

Error:
	if (pszDreamPath != nullptr) {
		delete[] pszDreamPath;
		pszDreamPath = nullptr;
	}

	return r;
}

RESULT PathManager::GetValuePath(PATH_VALUE_TYPE type, wchar_t* &n_pszPath) {
	RESULT r = R_PASS;

	long n_pszPath_n = 0;
	wchar_t *pszDreamPath = nullptr;
	long pszDreamPath_n = 0;
	wchar_t *pszValue = nullptr;
	long pszValue_n = 0;

	CRM(IsPathRegistered(type), "Value not registered");
	
	pszValue = m_pmapNVPPaths->at(type);
	pszValue_n = static_cast<long>(wcslen(pszValue));
	
	CRM(GetDreamPath(pszDreamPath), "Failed to acquire dream path");
	pszDreamPath_n = static_cast<long>(wcslen(pszDreamPath));

	n_pszPath_n = pszDreamPath_n + 1 + pszValue_n + 1;
	n_pszPath = new wchar_t[n_pszPath_n];
	memset(n_pszPath, 0, sizeof(wchar_t) * n_pszPath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
    // TODO: This breaks cross-platform-ness
    wcsncat(n_pszPath, pszDreamPath, pszDreamPath_n);
    wcsncat(n_pszPath, pszValue, pszValue_n);

Error:
	return r;
}

RESULT PathManager::GetValuePathVersion(PATH_VALUE_TYPE type, version ver, wchar_t* &n_pszVersionPath) {
	RESULT r = R_PASS;

	wchar_t *pszValuePath = nullptr;
	long pszValuePath_n = 0;

	wchar_t *pszVersionFolder = nullptr;
	long pszVersionFolder_n = 0;

	long n_pszVersionPath_n = 0;

	// Get Path
	CRM(GetValuePath(type, pszValuePath), "Failed to get value %S path", GetPathValueString(type));
	pszValuePath_n = static_cast<long>(wcslen(pszValuePath));

	CRM(GetVersionFolder(ver, pszVersionFolder), "Failed to get version folder");
	pszVersionFolder_n = static_cast<long>(wcslen(pszVersionFolder));

	n_pszVersionPath_n = (pszValuePath_n + 1) + (pszVersionFolder_n + 1);
	n_pszVersionPath = new wchar_t[n_pszVersionPath_n];
	memset(n_pszVersionPath, 0, sizeof(wchar_t) * n_pszVersionPath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	// TODO: This breaks cross-platform-ness
	wcsncat(n_pszVersionPath, pszValuePath, pszValuePath_n);
	wcsncat(n_pszVersionPath, pszVersionFolder, pszVersionFolder_n);

Error:
	if (pszValuePath != nullptr) {
		delete [] pszValuePath;
		pszValuePath = nullptr;
	}

	if (pszVersionFolder != nullptr) {
		delete [] pszVersionFolder;
		pszVersionFolder = nullptr;
	}
	return r;
}

// TODO: Move this to get file / FileManager
RESULT PathManager::GetFilePath(PATH_VALUE_TYPE type, std::wstring wstrFilename, char* &n_pszFilePath) {
	RESULT r = R_PASS;

	size_t wstrFilename_n = wstrFilename.size();
	size_t n_pszFilePath_n = 0;

	char *pszValuePath = nullptr;
	size_t pszValuePath_n = 0;

	std::string strFilename = util::WideStringToString(wstrFilename);

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = strlen(pszValuePath);

	n_pszFilePath_n = strlen(pszValuePath) + 1 + strFilename.size() + 1;
	n_pszFilePath = (char *)(new char[n_pszFilePath_n]);
	CN(n_pszFilePath);

	memset(n_pszFilePath, 0, n_pszFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	strncat(n_pszFilePath, pszValuePath, pszValuePath_n);
	strncat(n_pszFilePath, strFilename.c_str(), strFilename.size());

Error:
	// Release memory from GetValuePath
	if (pszValuePath != nullptr) {
		delete[] pszValuePath;
		pszValuePath = nullptr;
	}

	return r;
}

RESULT PathManager::GetFilePath(PATH_VALUE_TYPE type, std::wstring wstrFilename, wchar_t* &n_pszFilePath) {
	RESULT r = R_PASS;

	size_t wstrFilename_n = wstrFilename.size();
	size_t n_pszFilePath_n = 0;

	wchar_t *pszValuePath = nullptr;
	size_t pszValuePath_n = 0;

	std::string strFilename = util::WideStringToString(wstrFilename);

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = wcslen(pszValuePath);

	n_pszFilePath_n = wcslen(pszValuePath) + 1 + wstrFilename.size() + 1;
	n_pszFilePath = (wchar_t*)(new wchar_t[n_pszFilePath_n]);
	CN(n_pszFilePath);

	memset(n_pszFilePath, 0, n_pszFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	wcsncat(n_pszFilePath, pszValuePath, pszValuePath_n);
	wcsncat(n_pszFilePath, wstrFilename.c_str(), wstrFilename.size());

Error:
	// Release memory from GetValuePath
	if (pszValuePath != nullptr) {
		delete[] pszValuePath;
		pszValuePath = nullptr;
	}

	return r;
}

RESULT PathManager::GetFilePath(PATH_VALUE_TYPE type, std::wstring wstrFilename, std::wstring &r_wstrFilePath) {
	RESULT r = R_PASS;

	r_wstrFilePath.clear();
	
	CRM(GetValuePath(type, r_wstrFilePath), "Failed to get value path");

	r_wstrFilePath += wstrFilename;

Error:
	return r;
}

RESULT PathManager::GetFilePath(PATH_VALUE_TYPE type, const wchar_t *pszFileName, wchar_t * &n_pszFilePath) {
	RESULT r = R_PASS;
	long pszFileName_n = static_cast<long>(wcslen(pszFileName));
	long n_pszFilePath_n = 0;

	wchar_t *pszValuePath = nullptr;
	long pszValuePath_n = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = static_cast<long>(wcslen(pszValuePath));

	n_pszFilePath_n = pszValuePath_n + 1 + pszFileName_n + 1;
	n_pszFilePath = new wchar_t[n_pszFilePath_n];
	memset(n_pszFilePath, 0, sizeof(wchar_t) * n_pszFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
    wcsncat(n_pszFilePath, pszValuePath, pszValuePath_n);
    wcsncat(n_pszFilePath, pszFileName, pszFileName_n);

Error:
	// Release memory from GetValuePath
	if (pszValuePath != nullptr) {
		delete[] pszValuePath;
		pszValuePath = nullptr;
	}

	return r;
}

// TODO: There is a lot of redundancy between this and the GetFilePath call
// intent is different so better to keep it this way for now
RESULT PathManager::GetFilePathVersion(PATH_VALUE_TYPE type, version ver, const wchar_t *pszFileName, wchar_t * &n_pszVersionFilePath) {
	RESULT r = R_PASS;

	long pszFileName_n = static_cast<long>(wcslen(pszFileName));
	long n_pszVersionFilePath_n = 0;

	wchar_t *pszValueVersionPath = nullptr;
	long pszValueVersionPath_n = 0;

	CRM(GetValuePathVersion(type, ver, pszValueVersionPath), "Failed to get value version path");
	pszValueVersionPath_n = static_cast<long>(wcslen(pszValueVersionPath));

	n_pszVersionFilePath_n = pszValueVersionPath_n + 1 + pszFileName_n + 1;
	n_pszVersionFilePath = new wchar_t[n_pszVersionFilePath_n];
	memset(n_pszVersionFilePath, 0, sizeof(wchar_t) * n_pszVersionFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	wcsncat(n_pszVersionFilePath, pszValueVersionPath, pszValueVersionPath_n);
	wcsncat(n_pszVersionFilePath, pszFileName, pszFileName_n);

Error:
	if (pszValueVersionPath != nullptr) {
		delete[] pszValueVersionPath;
		pszValueVersionPath = nullptr;
	}

	return r;
}

RESULT PathManager::GetFilePathWithFolder(PATH_VALUE_TYPE type, const wchar_t *pszFolderName, std::wstring &strPathWithFolder) {
	RESULT r = R_PASS;

	wchar_t *pszValuePath = nullptr;
	long pszValuePath_n = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = static_cast<long>(wcslen(pszValuePath));

	strPathWithFolder = std::wstring(pszValuePath);
	strPathWithFolder += pszFolderName;
	strPathWithFolder += '/';

Error:
	if (pszValuePath != nullptr) {
		delete[] pszValuePath;
		pszValuePath = nullptr;
	}

	return r;
}

std::wstring PathManager::GetFilePath(PATH_VALUE_TYPE type, const wchar_t *pszFileName) {
	RESULT r = R_PASS;
	std::wstring wstrRet;
	wchar_t* pszTempFilePath = nullptr;

	CR(GetFilePath(type, pszFileName, pszTempFilePath));

	wstrRet = std::wstring(pszTempFilePath);

Error:
	if (pszTempFilePath != nullptr) {
		delete[] pszTempFilePath;
		pszTempFilePath = nullptr;
	}

	return wstrRet;
}

std::wstring PathManager::GetDirectoryPathFromFilePath(std::wstring wstrFilePath) {
	return wstrFilePath.substr(0, wstrFilePath.find_last_of(L"\\/")) + L"\\";
}

std::string PathManager::GetDirectoryPathFromFilePath(std::string strFilePath) {
	return strFilePath.substr(0, strFilePath.find_last_of("\\/")) + "\\";
}

bool PathManager::IsRootPath(wchar_t *pwszRoot, wchar_t *pwszFilename) {
	wchar_t *pwszFirst = nullptr;
	bool fRetVal = false;

	pwszFirst = wcschr(pwszFilename, L':');

	if (pwszFirst == nullptr) {
		return false;
	}
	
	size_t sizeOfRoot = (pwszFirst - pwszFilename);

	wchar_t *pwszRootTemp = new wchar_t[sizeOfRoot + 1];
	memset(pwszRootTemp, 0, sizeof(wchar_t) * (sizeOfRoot + 1));
	wcsncpy(pwszRootTemp, pwszFilename, sizeOfRoot);

	if (wcscmp(pwszRootTemp, pwszRoot) == 0) {
		fRetVal = true;
	}
	else {
		fRetVal = false;
	}

	delete[] pwszRootTemp;
	pwszRootTemp = nullptr;

	return fRetVal;
}

bool PathManager::IsAbsolutePath(wchar_t *pwszFilename) {
	wchar_t *pwszFirst = nullptr;
	
	pwszFirst = wcschr(pwszFilename, L':');

	if (pwszFirst == nullptr) {
		return false;
	}
	else {
		return true;
	}
}

bool PathManager::IsDreamPath(wchar_t *pwszFilename) {
	return IsRootPath(DREAM_OS_PATH_WROOT, pwszFilename);
}

RESULT PathManager::DoesPathExist(PATH_VALUE_TYPE type) {
	RESULT r = R_PASS;

	wchar_t *pszValuePath = nullptr;
	long pszValuePath_n = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = static_cast<long>(wcslen(pszValuePath));
	CRM(DoesPathExist(pszValuePath), "Path %S does not exist", pszValuePath);

Error:
	if (pszValuePath != nullptr) {
		delete[] pszValuePath;
		pszValuePath = nullptr;
	}

	return r;
}

// Checks if the path exists, if check file it'll check the file path directly, otherwise
// it'll look at the top level directory
RESULT PathManager::DoesPathExist(std::wstring wstrPath, bool fCheckFile) {
	RESULT r = R_PASS;

	if (!fCheckFile) {
		wstrPath = wstrPath.substr(0, wstrPath.find_last_of(L"\\/"));
	}

	CR(DoesPathExist(wstrPath.c_str()));
	
	if (fCheckFile)
		return R_FILE_FOUND;
	else
		return R_DIRECTORY_FOUND;
		
Error:
	return r;
}

RESULT PathManager::DoesPathExist(std::string strPath, bool fCheckFile) {
	std::wstring wstrPath = util::StringToWideString(strPath.c_str());
	return DoesPathExist(wstrPath, fCheckFile);
}

RESULT PathManager::DoesFileExist(PATH_VALUE_TYPE type, const wchar_t *pszFileName) {
	RESULT r = R_PASS;
	
	wchar_t *pszFilePath = nullptr;
	long pszFilePath_n = 0;

	CRM(GetFilePath(type, pszFileName, pszFilePath), "Failed to file path");	
	pszFilePath_n = static_cast<long>(wcslen(pszFilePath));

	r = DoesPathExist(pszFilePath);
	if (r == R_FILE_FOUND)
		return r;
	else
		return R_FILE_NOT_FOUND;

Error:
	if (pszFilePath != nullptr) {
		delete[] pszFilePath;
		pszFilePath = nullptr;
	}

	return r;
}

RESULT PathManager::GetFileVersionThatExists(PATH_VALUE_TYPE type, version versionFile, const wchar_t *pszFileName, version *versionFileExists) {
	RESULT r = R_PASS;
	std::list<wchar_t*> *pListDirs = new std::list<wchar_t*>();
	version maxVersion = 0;
	//wchar_t *pszMaxVersionDir = NULL;

	// Tabulate directories in given path
	CRM(GetListOfDirectoriesInPath(type, pListDirs), "Failed to get list of directories");

	for (auto it = pListDirs->begin(); it != pListDirs->end(); it++) {
		wchar_t *pszDirectory = (*it);
		long pszDirectory_n = static_cast<long>(wcslen(pszDirectory));

		while (pszDirectory[0] < L'0' || pszDirectory[0] > L'9') {
			pszDirectory++;
			CBM((pszDirectory[0] != L'\0'), "Invalid directory %S found in path %S", (*it), GetPathValueString(type));
		}

		for (int i = 0; i < pszDirectory_n; i++) {
			long value = wcstol(pszDirectory, nullptr, 10);
			version versionDir = version(value);
			if (versionDir > maxVersion && versionDir <= versionFile) {
				maxVersion = versionDir;
				//pszMaxVersionDir = (*it);
			}
		}
	}

	*versionFileExists = maxVersion;

Error:
	while(pListDirs->size() > 0) {
		wchar_t *pszTemp = pListDirs->front();
		pListDirs->pop_front();

		if (pszTemp != nullptr) {
			delete[]pszTemp;
			pszTemp = nullptr;
		}
	}

	if (pListDirs != nullptr) {
		delete pListDirs;
		pListDirs = nullptr;
	}

	return r;
}

RESULT PathManager::GetFilePathForName(PATH_VALUE_TYPE type, const wchar_t *pszName, std::wstring strFilename, std::wstring &strFilePath) {
	RESULT r = R_PASS;

	std::wstring strNameDirPath;
	CR(GetFilePathWithFolder(type, pszName, strNameDirPath));

	strFilePath = strNameDirPath + strFilename;

Error:
	return r;
}

RESULT PathManager::GetFilesForNameInPath(PATH_VALUE_TYPE type, const wchar_t *pszName, std::vector<std::wstring> &vstrFiles, const wchar_t *pszOptExtension) {
	RESULT r = R_PASS;
	std::list<wchar_t*> *pListDirs = new std::list<wchar_t*>();
	bool fFound = false;

	CRM(GetListOfDirectoriesInPath(type, pListDirs), "Failed to get list of directories");

	for (auto it = pListDirs->begin(); it != pListDirs->end(); it++) {
		wchar_t *pszDirectory = (*it);
		long pszDirectory_n = static_cast<long>(wcslen(pszDirectory));

		if(wcscmp(pszDirectory, pszName) == 0) {
			std::wstring strNameDirPath;
			CR(GetFilePathWithFolder(type, pszName, strNameDirPath));

			// Get the files
			CRM(GetListOfFilesInPath(strNameDirPath, vstrFiles, pszOptExtension), "Failed to get list of directories");

			fFound = true;
			break;
		}
	}

	CBM(fFound, "Name %S not found in path %S", pszName, GetPathValueString(type));

Error:
	while (pListDirs->size() > 0) {
		wchar_t *pszTemp = pListDirs->front();
		pListDirs->pop_front();

		if (pszTemp != nullptr) {
			delete[]pszTemp;
			pszTemp = nullptr;
		}
	}

	if (pListDirs != nullptr) {
		delete pListDirs;
		pListDirs = nullptr;
	}

	return r;
}

FILE* PathManager::OpenFile(PATH_VALUE_TYPE type, wchar_t* pszFilename, wchar_t* pszOpenMode) {
	RESULT r = R_PASS;
	FILE *pFile = nullptr;

	// TODO: Move this path code into path manager or something
	wchar_t *pszFilePath = nullptr;

	// Check if this is an absolute path
	if (IsDreamPath(const_cast<wchar_t*>(pszFilename))) {
		// TODO: Set Dream Path
	}
	else if (IsAbsolutePath(const_cast<wchar_t*>(pszFilename))) {
		pszFilePath = pszFilename;
	}
	else {
		// Move to key based file paths
		CRM(GetFilePath(type, pszFilename, pszFilePath), "Failed to get path for %S", pszFilename);
		CN(pszFilePath);
	}

	// Load file from path
	pFile = _wfopen(pszFilePath, pszOpenMode);
	CN(pFile);

	return pFile;

Error:
	if (pFile != nullptr) {
		fclose(pFile);
		pFile = nullptr;
	}

	return nullptr;
}
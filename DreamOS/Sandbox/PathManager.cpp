#include "PathManager.h"
#include "Primitives/Types/Number.h"

// Initialize and allocate the instance
PathManager* PathManager::m_pInstance = NULL;

PathManager::PathManager() :
	m_pmapNVPPaths(NULL)
{
	RESULT r = R_PASS;

	m_cszPathValues_n = sizeof(m_cszPathValues) / sizeof(m_cszPathValues[0]);

	m_pmapNVPPaths = new std::map<PATH_VALUE_TYPE, wchar_t*>();
	ACNM(m_pmapNVPPaths, "Failed to allocate paths map");

	Validate();
	return;

Error:
	Invalidate();
}

PathManager::~PathManager() {
	ACRM(Dealloc(), "PathManager failed to deallocate paths");

	// Dealloc the map
	if (m_pmapNVPPaths != NULL) {
		std::map<PATH_VALUE_TYPE, wchar_t*>::iterator it = m_pmapNVPPaths->begin();
		while (it != m_pmapNVPPaths->end()) {
			delete[] it->second;
			it++;
		}

		delete m_pmapNVPPaths;
		m_pmapNVPPaths = NULL;
	}
}

RESULT PathManager::Dealloc() {
	return R_NOT_IMPLEMENTED;
}

RESULT PathManager::RegisterPath(wchar_t *pszName, wchar_t *pszValue) {
	RESULT r = R_PASS;
	std::pair<std::map<PATH_VALUE_TYPE, wchar_t*>::iterator, bool>retVal;
	wchar_t *pszPath = NULL;

    long pszValueCopy_n = 0;
    wchar_t *pszValueCopy = NULL;
    //errno_t err;

	DEBUG_LINEOUT("Registering Name:%S Value:%S", pszName, pszValue);
	PATH_VALUE_TYPE pathValueType = GetPathValueType(pszName);
	CBM((pathValueType != PATH_INVALID), "Not a valid path value type");

	// Copy the value - save the memory
	pszValueCopy_n = wcslen(pszValue) + 1;
	pszValueCopy = new wchar_t[pszValueCopy_n];
	memset(pszValueCopy, 0, sizeof(wchar_t) * pszValueCopy_n);
    
    CNM(wcscpy(pszValueCopy, pszValue), "Failed to copy over value");

	retVal = m_pmapNVPPaths->insert(std::pair<PATH_VALUE_TYPE, wchar_t*>(pathValueType, pszValueCopy));
	CBM((retVal.second != false), "Failed to insert NVP into Paths map");

	CRM(GetValuePath(pathValueType, pszPath), "Failed to get path value");
	DEBUG_LINEOUT("Registered type:%d Value:%S", pathValueType, pszPath);

Error:
	if (pszPath != NULL) {
		delete[] pszPath;
		pszPath = NULL;
	}

	return r;
}

// Set up path manager
RESULT PathManager::InitializePaths() {
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

RESULT PathManager::GetValuePath(PATH_VALUE_TYPE type, wchar_t* &n_pszPath) {
	RESULT r = R_PASS;

	long n_pszPath_n = 0;
	wchar_t *pszDreamPath = NULL;
	long pszDreamPath_n = 0;
	wchar_t *pszValue = NULL;
	long pszValue_n = 0;

	CRM(IsPathRegistered(type), "Value not registered");
	
	pszValue = m_pmapNVPPaths->at(type);
	pszValue_n = wcslen(pszValue);
	
	CRM(GetDreamPath(pszDreamPath), "Failed to acquire dream path");
	pszDreamPath_n = wcslen(pszDreamPath);

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

	wchar_t *pszValuePath = NULL;
	long pszValuePath_n = 0;

	wchar_t *pszVersionFolder = NULL;
	long pszVersionFolder_n = 0;

	long n_pszVersionPath_n = 0;

	// Get Path
	CRM(GetValuePath(type, pszValuePath), "Failed to get value %S path", GetPathValueString(type));
	pszValuePath_n = wcslen(pszValuePath);

	CRM(GetVersionFolder(ver, pszVersionFolder), "Failed to get version folder");
	pszVersionFolder_n = wcslen(pszVersionFolder);

	n_pszVersionPath_n = (pszValuePath_n + 1) + (pszVersionFolder_n + 1);
	n_pszVersionPath = new wchar_t[n_pszVersionPath_n];
	memset(n_pszVersionPath, 0, sizeof(wchar_t) * n_pszVersionPath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	// TODO: This breaks cross-platform-ness
	wcsncat(n_pszVersionPath, pszValuePath, pszValuePath_n);
	wcsncat(n_pszVersionPath, pszVersionFolder, pszVersionFolder_n);

Error:
	if (pszValuePath != NULL) {
		delete [] pszValuePath;
		pszValuePath = NULL;
	}

	if (pszVersionFolder != NULL) {
		delete [] pszVersionFolder;
		pszVersionFolder = NULL;
	}
	return r;
}

// TODO: Move this to get file / FileManager
RESULT PathManager::GetFilePath(PATH_VALUE_TYPE type, const wchar_t *pszFileName, wchar_t * &n_pszFilePath) {
	RESULT r = R_PASS;
	long pszFileName_n = wcslen(pszFileName);
	long n_pszFilePath_n = 0;

	wchar_t *pszValuePath = NULL;
	long pszValuePath_n = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = wcslen(pszValuePath);

	n_pszFilePath_n = pszValuePath_n + 1 + pszFileName_n + 1;
	n_pszFilePath = new wchar_t[n_pszFilePath_n];
	memset(n_pszFilePath, 0, sizeof(wchar_t) * n_pszFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
    wcsncat(n_pszFilePath, pszValuePath, pszValuePath_n);
    wcsncat(n_pszFilePath, pszFileName, pszFileName_n);

Error:
	// Release memory from GetValuePath
	if (pszValuePath != NULL) {
		delete[] pszValuePath;
		pszValuePath = NULL;
	}

	return r;
}

// TODO: There is a lot of redundancy between this and the GetFilePath call
// intent is different so better to keep it this way for now
RESULT PathManager::GetFilePathVersion(PATH_VALUE_TYPE type, version ver, const wchar_t *pszFileName, wchar_t * &n_pszVersionFilePath) {
	RESULT r = R_PASS;

	long pszFileName_n = wcslen(pszFileName);
	long n_pszVersionFilePath_n = 0;

	wchar_t *pszValueVersionPath = NULL;
	long pszValueVersionPath_n = 0;

	CRM(GetValuePathVersion(type, ver, pszValueVersionPath), "Failed to get value version path");
	pszValueVersionPath_n = wcslen(pszValueVersionPath);

	n_pszVersionFilePath_n = pszValueVersionPath_n + 1 + pszFileName_n + 1;
	n_pszVersionFilePath = new wchar_t[n_pszVersionFilePath_n];
	memset(n_pszVersionFilePath, 0, sizeof(wchar_t) * n_pszVersionFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	wcsncat(n_pszVersionFilePath, pszValueVersionPath, pszValueVersionPath_n);
	wcsncat(n_pszVersionFilePath, pszFileName, pszFileName_n);

Error:
	if (pszValueVersionPath != NULL) {
		delete[] pszValueVersionPath;
		pszValueVersionPath = NULL;
	}

	return r;
}

RESULT PathManager::DoesPathExist(PATH_VALUE_TYPE type) {
	RESULT r = R_PASS;

	wchar_t *pszValuePath = NULL;
	long pszValuePath_n = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = wcslen(pszValuePath);
	CRM(DoesPathExist(pszValuePath), "Path %S does not exist", pszValuePath);

Error:
	if (pszValuePath != NULL) {
		delete[] pszValuePath;
		pszValuePath = NULL;
	}

	return r;
}

RESULT PathManager::DoesFileExist(PATH_VALUE_TYPE type, const wchar_t *pszFileName) {
	RESULT r = R_PASS;
	
	wchar_t *pszFilePath = NULL;
	long pszFilePath_n = 0;

	CRM(GetFilePath(type, pszFileName, pszFilePath), "Failed to file path");	
	pszFilePath_n = wcslen(pszFilePath);

	r = DoesPathExist(pszFilePath);
	if (r == R_FILE_FOUND)
		return r;
	else
		return R_FILE_NOT_FOUND;

Error:
	if (pszFilePath != NULL) {
		delete[] pszFilePath;
		pszFilePath = NULL;
	}

	return r;
}

RESULT PathManager::GetFileVersionThatExists(PATH_VALUE_TYPE type, version versionFile, const wchar_t *pszFileName, version *versionFileExists) {
	RESULT r = R_PASS;
	std::list<wchar_t*> *pListDirs = new std::list<wchar_t*>();

	// Tabulate directories in given path
	CRM(GetListOfDirectoriesInPath(type, pListDirs), "Failed to get list of directories");

Error:
	while(pListDirs->size() > 0) {
		wchar_t *pszTemp = pListDirs->front();
		pListDirs->pop_front();

		if (pszTemp != NULL) {
			delete[]pszTemp;
			pszTemp = NULL;
		}
	}

	if (pListDirs != NULL) {
		delete pListDirs;
		pListDirs = NULL;
	}

	return r;
}
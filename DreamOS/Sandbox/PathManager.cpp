#include "PathManager.h"

PathManager::PathManager() :
	m_pmapNVPPaths(NULL)
{
	m_cszPathValues_n = sizeof(m_cszPathValues) / sizeof(m_cszPathValues[0]);

	m_pmapNVPPaths = new std::map<PATH_VALUE_TYPE, wchar_t*>();
	ACNM(m_pmapNVPPaths, "Failed to allocate paths map");
}

PathManager::~PathManager() {
	ACRM(Dealloc(), "PathManager failed to deallocate paths");

	if (m_pmapNVPPaths != NULL) {
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

	DEBUG_LINEOUT("Registering Name:%S Value:%S", pszName, pszValue);
	PATH_VALUE_TYPE pathValueType = GetPathValueType(pszName);
	CBM((pathValueType != PATH_INVALID), "Not a valid path value type");

	retVal = m_pmapNVPPaths->insert(std::pair<PATH_VALUE_TYPE, wchar_t*>(pathValueType, pszValue));
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

RESULT PathManager::GetValuePath(PATH_VALUE_TYPE type, wchar_t* &n_pszPath) {
	RESULT r = R_PASS;
	int n_pszPath_n = 0;
	wchar_t *pszDreamPath = NULL;
	int pszDreamPath_n = 0;
	wchar_t *pszValue = NULL;
	int pszValue_n = 0;
	errno_t err;

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
	err = wcsncat_s(n_pszPath, n_pszPath_n, pszDreamPath, pszDreamPath_n);		
	err = wcsncat_s(n_pszPath, n_pszPath_n, pszValue, pszValue_n);

Error:
	return r;
}

RESULT PathManager::GetFilePath(PATH_VALUE_TYPE type, wchar_t *pszFileName, wchar_t *n_pszFilePath) {
	RESULT r = R_PASS;
	errno_t err;
	int pszFileName_n = wcslen(pszFileName);
	int n_pszFilePath_n = 0;

	wchar_t *pszValuePath = NULL;
	int pszValuePath_n = 0;

	CRM(GetValuePath(type, pszValuePath), "Failed to get value path");
	pszValuePath_n = wcslen(pszValuePath);

	n_pszFilePath_n = pszValuePath_n + 1 + pszFileName_n + 1;
	n_pszFilePath = new wchar_t[n_pszFilePath_n];
	memset(n_pszFilePath, 0, sizeof(wchar_t) * n_pszFilePath_n);

	// Compose the path
	// TODO: Maybe do some lower level parsing here since ./ will just get attached
	err = wcsncat_s(n_pszFilePath, n_pszFilePath_n, pszValuePath, pszValuePath_n);
	err = wcsncat_s(n_pszFilePath, n_pszFilePath_n, pszFileName, pszFileName_n);

Error:
	// Release memory from GetValuePath
	if (pszValuePath != NULL) {
		delete[] pszValuePath;
		pszValuePath = NULL;
	}

	return r;
}
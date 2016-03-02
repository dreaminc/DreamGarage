#include "OSXPathManager.h"
#include <stdlib.h>
#include <cstring>
#include <limits.h>
#include <unistd.h>

OSXPathManager::OSXPathManager() :
    PathManager()	// Call super
{
    memset(m_pszDreamRootPath, 0, (sizeof(wchar_t) * MAX_PATH_LENGTH_PM));
    m_pszDreamRootPath_n = 0;
    
    memset(m_pszCurDirectiory, 0, (sizeof(wchar_t) * MAX_PATH_LENGTH_PM));
    m_pszCurDirectiory_n = 0;
    
    memset(m_pszShadersPath, 0, (sizeof(wchar_t) * MAX_PATH_LENGTH_PM));
    m_pszShadersPath_n = 0;
    
    // Set up the path values
    
}


RESULT OSXPathManager::Dealloc() {
    RESULT r = R_PASS;
    
    // Empty for now
    
Error:
    return r;
}

/*
 RESULT Win64PathManager::RegisterPath(wchar_t *pszName, wchar_t *pszValue) {
	RESULT r = R_PASS;
 
	DEBUG_LINEOUT("Found Name:%S Value:%S", pszName, pszValue);
 
 Error:
	return r;
 }
 */

// This file runs in current directory
RESULT OSXPathManager::OpenDreamPathsFile() {
    RESULT r = R_PASS;
    
    FILE *pFile = NULL;
    wchar_t szLine[MAX_PATH_LENGTH_PM];
    
    pFile = fopen(DREAM_OS_PATHS_FILE, "r");
    CNM(pFile, "OpenDreamPathsFile failed to open file %S%s", m_pszCurDirectiory, DREAM_OS_PATHS_FILE);
    rewind(pFile);
    
    while(fgetws(szLine, MAX_PATH_LENGTH_PM, pFile) != NULL) {
        wchar_t *pszContext = NULL;
        wchar_t *pszName = wcstok(szLine, L" \t\n\v\f\r", &pszContext);
        
        if (pszName == NULL || pszName[0] == '#')
            continue;
        
        wchar_t *pszValue = wcstok(NULL, L" \t\n\v\f\r", &pszContext);
        CRM(RegisterPath(pszName, pszValue), "Failed to register value %S", pszName);
    }
    
Error:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }
    return r;
}

RESULT OSXPathManager::GetCurrentPath(wchar_t *& pszCurrentPath) {
    RESULT r = R_PASS;
    
    pszCurrentPath = NULL;
    
    CNM(m_pszCurDirectiory, "CurDirectory NULL");
    pszCurrentPath = m_pszCurDirectiory;
    
Error:
    return r;
}

RESULT OSXPathManager::GetDreamPath(wchar_t*&pszDreamPath) {
    RESULT r = R_PASS;
    
    pszDreamPath = NULL;
    
    CNM(m_pszDreamRootPath, "DreamRootPath NULL");
    pszDreamPath = m_pszDreamRootPath;
    
Error:
    return r;
}

RESULT OSXPathManager::SetCurrentPath(wchar_t *pszPath) {
    RESULT r = R_PASS;
    
    //CBM(SetCurrentDirectory(pszPath), );
    long lenPsz = wcslen(pszPath) * sizeof(char) + 1;
    char *pszTempPath = new char[lenPsz];
    memset(pszTempPath, 0, lenPsz);
    
    size_t copyLength = wcstombs(pszTempPath, pszPath, lenPsz);
    CBM(((copyLength + 1) == lenPsz), "Path string copy failed");
    
    CBM((chdir(pszTempPath) == 0), "Failed to set path to %s", pszTempPath);
    
    // On success copy the path over
    //wcscpy(m_pszCurDirectiory, pszPath);
    CRM(UpdateCurrentPath(), "Failed to update current path");
    DEBUG_LINEOUT("Switched current directory to %S", m_pszCurDirectiory);
    
Error:
    if(pszTempPath != NULL) {
        delete [] pszTempPath;
        pszTempPath = NULL;
    }
    
    return r;
}

RESULT OSXPathManager::UpdateCurrentPath() {
    RESULT r = R_PASS;
    
    char *pszTempPath = new char[MAX_PATH_LENGTH_PM];
    memset(pszTempPath, 0, MAX_PATH_LENGTH_PM);
    
    // Initialize Current Path
    CNM(getcwd(pszTempPath, MAX_PATH_LENGTH_PM), "Failed to get current path");

    CBM((mbstowcs(m_pszCurDirectiory, pszTempPath, MAX_PATH_LENGTH_PM) != -1), "Failed to copy over to wide string");

    DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);
    
Error:
    if(pszTempPath != NULL) {
        delete [] pszTempPath;
        pszTempPath = NULL;
    }
    return r;
}

RESULT OSXPathManager::InitializePaths() {
    RESULT r = R_PASS;
    char *pszDreamPath = NULL;
    size_t pszDreamPath_n = 0;
    
    DEBUG_LINEOUT("OSXPathManager Initialize Paths");
    
    // Dream Root Path - Try to find environment variable to set paths
    pszDreamPath = getenv(DREAM_OS_PATH_ENV);
    CNM(pszDreamPath, "DREAM_OS_PATH_ENV not set!");
    
    pszDreamPath_n = strlen(pszDreamPath);
    CBM((pszDreamPath_n > 0), "Failed to get DreamOS PAth Environment variable %s", DREAM_OS_PATH_ENV);
    
    if (pszDreamPath != NULL) {
        DEBUG_LINEOUT("Found %s env variable: %s", DREAM_OS_PATH_ENV, pszDreamPath);
        
        mbstowcs(m_pszDreamRootPath, pszDreamPath, pszDreamPath_n);
        //mbstowcs_s(&m_pszDreamRootPath_n, m_pszDreamRootPath, pszDreamPath, pszDreamPath_n);
        CRM(SetCurrentPath(m_pszDreamRootPath), "Failed to set current path to dream root");
        
        CRM(OpenDreamPathsFile(), "Failed to find Dream Paths file");
    }
    else {
        // Try to back pedal to find dreampaths.txt
        DEBUG_LINEOUT("%s env variable not found", DREAM_OS_PATH_ENV);
    }
    
    
    
Error:
    return r;
}


RESULT OSXPathManager::PrintPaths() {
    RESULT r = R_PASS;
    
    // Some debug / path management
    DEBUG_LINEOUT("Current directory %S", m_pszCurDirectiory);
    DEBUG_LINEOUT("Dream Root Path %S", m_pszDreamRootPath);
    
    
Error:
    return r;
}
#include "CEFExtension.h"

#include "CEFV8Handler.h"
#include "Sandbox/PathManager.h"

CEFExtension::CEFExtension(wchar_t *pszExtensionCodeFilename, CefRefPtr<CEFV8Handler> pCEFV8Handler) :
	m_pszExtensionCodeFilename(pszExtensionCodeFilename),
	m_pCEFV8Handler(pCEFV8Handler)
{
	// empty
}

CEFExtension::~CEFExtension() {
	// empty
}

RESULT CEFExtension::Initialize() {
	RESULT r = R_PASS;

	CNM(m_pCEFV8Handler, "CEFV8Handler invalid");

	CNM(m_pszExtensionCodeFilename, "Extension file path not valid");
	//CRM(LoadExtensionCodeFromFile(m_pszExtensionCodeFilename), "Failed to load extension file %S", m_pszExtensionCodeFilename);
	
	

	{
		// Register the extension.
		//CefString cefStrExtensionCode = CefString(m_strExtensionCode);
		CefString cefStrExtensionCode =
			"var test;"
			"if (!test)"
			"  test = {};"
			"(function() {"
			"  test.myfunc = function() {"
			"    native function myfunc();"
			"    return myfunc();"
			"  };"
			"})();";

		CBM((CefRegisterExtension("v8/test", cefStrExtensionCode, m_pCEFV8Handler)), "Failed to register extension code");
	}

Error:
	return r;
}

RESULT CEFExtension::LoadExtensionCodeFromFile(const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	wchar_t *pFilePath = nullptr;

	CRM(pPathManager->GetFilePath(PATH_SCRIPTS, pszFilename, pFilePath),
		"Failed to get path for %S extension", pszFilename);

	const char *pszExtensionCode = FileRead(pFilePath);
	CNM(pszExtensionCode, "Failed to read file %S", pFilePath);

	m_strExtensionCode = std::string(pszExtensionCode);

	DEBUG_LINEOUT("Loaded new script extension %S", pFilePath);

Error:
	if (pFilePath != nullptr) {
		delete[] pFilePath;
		pFilePath = nullptr;
	}

	return r;
}

// TODO: Move to path manager or utilities 
// TODO: Fix arch, this call returns new memory
char* CEFExtension::FileRead(wchar_t *pszFileName) {
	RESULT r = R_PASS;
	errno_t err;
	char *pszFileContent = nullptr;
	FILE *pFile = nullptr;

	CNM(pszFileName, "Filename cannot be NULL");

	err = _wfopen_s(&pFile, pszFileName, L"r");

	// TODO: print out with unicode support.
	CNM(pFile, "Failed to open file");

	size_t pFile_n = -1;
	err = fseek(pFile, 0, SEEK_END);
	pFile_n = ftell(pFile);
	rewind(pFile);

	CBM((pFile_n > 0), "File %S is empty", pszFileName);

	pszFileContent = new char[pFile_n + 1];
	pFile_n = fread(pszFileContent, sizeof(char), pFile_n, pFile);
	pszFileContent[pFile_n] = '\0';

Error:
	if (pFile != nullptr) {
		fclose(pFile);
		pFile = nullptr;
	}

	return pszFileContent;
}
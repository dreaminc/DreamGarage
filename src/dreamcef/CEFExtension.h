#ifndef CEF_EXTENSION_H_
#define CEF_EXTENSION_H_

#include "core/ehm/EHM.h"

// Dream CEF 
// dos/src/dreamcef/CEFExtension.h

#include "include/internal/cef_ptr.h"

class CEFV8Handler;

class CEFExtension {
public:
	CEFExtension(wchar_t *pszExtensionCodeFilename, CefRefPtr<CEFV8Handler> pCEFV8Handler);
	~CEFExtension();

	RESULT Initialize();
	RESULT LoadExtensionCodeFromFile(const wchar_t *pszFilename);

	char* FileRead(wchar_t *pszFileName);

public:
	wchar_t *m_pszExtensionCodeFilename = nullptr;
	std::string m_strExtensionCode;

	CefRefPtr<CEFV8Handler> m_pCEFV8Handler = nullptr;
};

#endif // !EF_EXTENSION_H_
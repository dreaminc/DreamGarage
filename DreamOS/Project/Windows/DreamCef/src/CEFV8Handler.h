#ifndef CEF_V8_HANLDER_H_
#define CEF_V8_HANLDER_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFV8Handler.h

#include "include/cef_v8.h"

class CEFV8Handler : public CefV8Handler {
public:
	CEFV8Handler();
	~CEFV8Handler();

	virtual bool Execute(const CefString& strName,
						 CefRefPtr<CefV8Value> pCEFV8Value,
						 const CefV8ValueList& cefArguments,
						 CefRefPtr<CefV8Value>& pCEFV8ValueReturn,
						 CefString& strCEFException) override;

	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(CEFV8Handler);
};

#endif	// ! CEF_V8_HANLDER_H_
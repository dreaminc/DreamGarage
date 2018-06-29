#ifndef CEF_V8_HANLDER_H_
#define CEF_V8_HANLDER_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFV8Handler.h

#include "include/cef_v8.h"

class DreamCEFApp;

class CEFV8Observer {
public:
	virtual RESULT DreamFormExecute(CefRefPtr<CefBrowser> browser, const CefString& strName, const CefV8ValueList& CefArguments) = 0;
};

class CEFV8Handler : public CefV8Handler {
public:
	CEFV8Handler();
	~CEFV8Handler();

	virtual bool Execute(const CefString& strName,
						 CefRefPtr<CefV8Value> pCEFV8Value,
						 const CefV8ValueList& cefArguments,
						 CefRefPtr<CefV8Value>& pCEFV8ValueReturn,
						 CefString& strCEFException) override;

	RESULT RegisterObserver(CEFV8Observer *pCEFV8Observer);

	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(CEFV8Handler);

private:
	CEFV8Observer *m_pCEFV8Observer = nullptr;
};

#endif	// ! CEF_V8_HANLDER_H_	
#ifndef CEF_V8_HANLDER_H_
#define CEF_V8_HANLDER_H_

#include "core/ehm/EHM.h"

// Dream CEF
// dos/src/dreamcef/CEFV8Handler.h

#include "include/cef_v8.h"

class CEFV8Handler : public CefV8Handler {
public:
	CEFV8Handler();
	~CEFV8Handler();

	RESULT Initialize();

	virtual bool Execute(const CefString& strName,
						 CefRefPtr<CefV8Value> pCEFV8Value,
						 const CefV8ValueList& cefArguments,
						 CefRefPtr<CefV8Value>& pCEFV8ValueReturn,
						 CefString& strCEFException) override;

	RESULT RegisterExtensionFunction(std::string strName, std::function<RESULT(CefRefPtr<CefBrowser>, const CefV8ValueList&)>);
	RESULT ExecuteExtensionFunction(std::string strName, const CefV8ValueList& cefArguments);

	RESULT HandleDreamFormSuccess(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);
	RESULT HandleDreamFormCancel(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);
	RESULT HandleDreamFormSetCredentials(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);
	RESULT HandleDreamFormSetEnvironmentId(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);

	RESULT HandleDreamFormIsInputFocused(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);
	RESULT HandleDreamFormCanTabNext(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);
	RESULT HandleDreamFormCanTabPrevious(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments);
	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(CEFV8Handler);

private:
	std::map<CefString, std::function<RESULT(CefRefPtr<CefBrowser>, const CefV8ValueList&)>> m_formFunctionMap;
};

#endif	// ! CEF_V8_HANLDER_H_	
#include "CEFV8Handler.h"

CEFV8Handler::CEFV8Handler() {
	// empty
}

CEFV8Handler::~CEFV8Handler() {
	// empty
}

bool CEFV8Handler::Execute(const CefString& strName, 
						   CefRefPtr<CefV8Value> pCEFV8Value, 
						   const CefV8ValueList& cefArguments, 
						   CefRefPtr<CefV8Value>& pCEFV8ValueReturn, 
						   CefString& strCEFException) 
{
	RESULT r = R_PASS;

	if (strName == "myfunc") {
		
		// Return my string value.

		pCEFV8ValueReturn = CefV8Value::CreateString("My Value!");

		CefRefPtr<CefV8Context> pContext = CefV8Context::GetCurrentContext();

		if (pContext != nullptr) {
			CefRefPtr<CefBrowser> pBrowser = CefV8Context::GetCurrentContext()->GetBrowser();
			if (pBrowser != nullptr) {
				pCEFV8ValueReturn = CefV8Value::CreateString("function success!");
				m_pCEFV8Observer->DreamFormSuccess(pBrowser);
			}
			else {
				pCEFV8ValueReturn = CefV8Value::CreateString("no browser!");
			}
		}
		else {
			pCEFV8ValueReturn = CefV8Value::CreateString("no context!");
		}

		return true;
	}
	
	// Function does not exist.
	return false;
}

RESULT CEFV8Handler::RegisterObserver(CEFV8Observer *pCEFV8Observer) {
	m_pCEFV8Observer = pCEFV8Observer;
	return R_PASS;
}
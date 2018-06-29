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

//	if (strName == "myfunc") {
		
		// Return my string value.

//	}

	CefRefPtr<CefV8Context> pContext;
	CefRefPtr<CefBrowser> pBrowser;

	pContext = CefV8Context::GetCurrentContext();
	CN(pContext);

	pBrowser = CefV8Context::GetCurrentContext()->GetBrowser();
	CN(pBrowser);

	//m_pCEFV8Observer->DreamFormSuccess(pBrowser);
//	pCEFV8ValueReturn = CefV8Value::CreateString("function success!");

	r = m_pCEFV8Observer->DreamFormExecute(pBrowser, strName, cefArguments);

	pCEFV8ValueReturn = CefV8Value::CreateInt(r);
	/*
	if (r == R_PASS) {
		pCEFV8ValueReturn = CefV8Value::CreateString("pass");
	}
	else {
		pCEFV8ValueReturn = CefV8Value::CreateString("fail");
	}
	//*/
	CR(r);
	
	// Function does not exist.
	return true;
Error:
	return false;
}

RESULT CEFV8Handler::RegisterObserver(CEFV8Observer *pCEFV8Observer) {
	m_pCEFV8Observer = pCEFV8Observer;
	return R_PASS;
}
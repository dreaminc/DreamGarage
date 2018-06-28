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
		return true;
	}
	
	// Function does not exist.
	return false;
}
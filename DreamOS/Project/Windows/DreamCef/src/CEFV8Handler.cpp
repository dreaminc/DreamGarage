#include "CEFV8Handler.h"

CEFV8Handler::CEFV8Handler() {
	// empty

}

CEFV8Handler::~CEFV8Handler() {
	// empty
}

RESULT CEFV8Handler::Initialize() {

	m_formFunctionMap["success"] = std::bind(&CEFV8Handler::DreamFormSuccess, this, std::placeholders::_1, std::placeholders::_2);
	m_formFunctionMap["cancel"] = std::bind(&CEFV8Handler::DreamFormCancel, this, std::placeholders::_1, std::placeholders::_2);
	m_formFunctionMap["setCredentials"] = std::bind(&CEFV8Handler::DreamFormSetCredentials, this, std::placeholders::_1, std::placeholders::_2);
	m_formFunctionMap["setEnvironmentId"] = std::bind(&CEFV8Handler::DreamFormSetEnvironmentId, this, std::placeholders::_1, std::placeholders::_2);

	return R_PASS;

}

bool CEFV8Handler::Execute(const CefString& strName, 
						   CefRefPtr<CefV8Value> pCEFV8Value, 
						   const CefV8ValueList& cefArguments, 
						   CefRefPtr<CefV8Value>& pCEFV8ValueReturn, 
						   CefString& strCEFException) 
{
	RESULT r = R_PASS;

	CefRefPtr<CefV8Context> pContext;
	CefRefPtr<CefBrowser> pBrowser;

	pContext = CefV8Context::GetCurrentContext();
	CN(pContext);

	pBrowser = CefV8Context::GetCurrentContext()->GetBrowser();
	CN(pBrowser);

	CB(m_formFunctionMap.count(strName) > 0);
	CR(m_formFunctionMap[strName](pBrowser, cefArguments));

	pCEFV8ValueReturn = CefV8Value::CreateInt(r);
	return true;

	// Function does not exist.
Error:
	pCEFV8ValueReturn = CefV8Value::CreateInt(r);
	return false;
}

RESULT CEFV8Handler::DreamFormSuccess(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	RESULT r = R_PASS;

	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::DreamExtension");


	const CefString strType = "Form";
	const CefString strMethod = "success";

	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(2);

	cefProcessMessageArguments->SetString(0, strType);
	cefProcessMessageArguments->SetString(1, strMethod);

	CB(CefArguments.size() == 0);
	CB((browser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return r;
}

RESULT CEFV8Handler::DreamFormCancel(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	return R_PASS;
}

RESULT CEFV8Handler::DreamFormSetCredentials(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	return R_PASS;
}

RESULT CEFV8Handler::DreamFormSetEnvironmentId(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	return R_PASS;
}
#include "CEFV8Handler.h"

CEFV8Handler::CEFV8Handler() {
	// empty

}

CEFV8Handler::~CEFV8Handler() {
	// empty
}

RESULT CEFV8Handler::Initialize() {
	RESULT r = R_PASS;

	CR(RegisterExtensionFunction("success", std::bind(&CEFV8Handler::HandleDreamFormSuccess, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterExtensionFunction("cancel", std::bind(&CEFV8Handler::HandleDreamFormCancel, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterExtensionFunction("setCredentials", std::bind(&CEFV8Handler::HandleDreamFormSetCredentials, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterExtensionFunction("setEnvironmentId", std::bind(&CEFV8Handler::HandleDreamFormSetEnvironmentId, this, std::placeholders::_1, std::placeholders::_2)));

	CR(RegisterExtensionFunction("canTabNext", std::bind(&CEFV8Handler::HandleDreamFormCanTabNext, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterExtensionFunction("canTabPrevious", std::bind(&CEFV8Handler::HandleDreamFormCanTabPrevious, this, std::placeholders::_1, std::placeholders::_2)));

Error:
	return r;
}

bool CEFV8Handler::Execute(const CefString& strName, 
						   CefRefPtr<CefV8Value> pCEFV8Value, 
						   const CefV8ValueList& cefArguments, 
						   CefRefPtr<CefV8Value>& pCEFV8ValueReturn, 
						   CefString& strCEFException) 
{
	RESULT r = R_PASS;

	CR(ExecuteExtensionFunction(strName, cefArguments));

	pCEFV8ValueReturn = CefV8Value::CreateInt(r);
	return true;

	// Function does not exist.
Error:
	pCEFV8ValueReturn = CefV8Value::CreateInt(r);
	return false;
}
		
RESULT CEFV8Handler::RegisterExtensionFunction(std::string strName, std::function<RESULT(CefRefPtr<CefBrowser>, const CefV8ValueList&)> extensionFunction) {
	RESULT r = R_PASS;

	CBM(m_formFunctionMap.count(strName) == 0, "function already registered");
	m_formFunctionMap[strName] = extensionFunction;

Error:
	return r;
}

RESULT CEFV8Handler::ExecuteExtensionFunction(std::string strName, const CefV8ValueList& cefArguments) {
	RESULT r = R_PASS;

	CefRefPtr<CefV8Context> pContext;
	CefRefPtr<CefBrowser> pBrowser;

	CBM(m_formFunctionMap.count(strName) > 0, "function not registered");

	pContext = CefV8Context::GetCurrentContext();
	CN(pContext);
	pBrowser = pContext->GetBrowser();
	CN(pBrowser);

	CR(m_formFunctionMap[strName](pBrowser, cefArguments));

Error:
	return r;
}

RESULT CEFV8Handler::HandleDreamFormSuccess(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
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

RESULT CEFV8Handler::HandleDreamFormCancel(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	RESULT r = R_PASS;

	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::DreamExtension");

	const CefString strType = "Form";
	const CefString strMethod = "cancel";

	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(2);

	cefProcessMessageArguments->SetString(0, strType);
	cefProcessMessageArguments->SetString(1, strMethod);

	CB(CefArguments.size() == 0);
	CB((browser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return r;
}

RESULT CEFV8Handler::HandleDreamFormSetCredentials(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	RESULT r = R_PASS;

	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::DreamExtension");

	const CefString strType = "Form";
	const CefString strMethod = "setCredentials";

	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(4);

	cefProcessMessageArguments->SetString(0, strType);
	cefProcessMessageArguments->SetString(1, strMethod);

	CB(CefArguments.size() == 2);
	cefProcessMessageArguments->SetString(2, CefArguments[0]->GetStringValue());
	cefProcessMessageArguments->SetString(3, CefArguments[1]->GetStringValue());

	CB((browser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return r;
}

RESULT CEFV8Handler::HandleDreamFormSetEnvironmentId(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	RESULT r = R_PASS;

	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::DreamExtension");

	const CefString strType = "Form";
	const CefString strMethod = "setEnvironmentId";

	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(3);

	cefProcessMessageArguments->SetString(0, strType);
	cefProcessMessageArguments->SetString(1, strMethod);

	CB(CefArguments.size() == 1);
	cefProcessMessageArguments->SetInt(2, CefArguments[0]->GetIntValue());

	CB((browser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return r;
}

RESULT CEFV8Handler::HandleDreamFormCanTabNext(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	RESULT r = R_PASS;
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::DreamExtension");

	const CefString strType = "Browser";
	const CefString strMethod = "canTabNext";
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(3);

	cefProcessMessageArguments->SetString(0, strType);
	cefProcessMessageArguments->SetString(1, strMethod);

	CB(CefArguments.size() == 1);
	cefProcessMessageArguments->SetBool(2, CefArguments[0]->GetBoolValue());

	CB((browser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return r;
}

RESULT CEFV8Handler::HandleDreamFormCanTabPrevious(CefRefPtr<CefBrowser> browser, const CefV8ValueList& CefArguments) {
	RESULT r = R_PASS;
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::DreamExtension");

	const CefString strType = "Browser";
	const CefString strMethod = "canTabPrevious";
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(3);

	cefProcessMessageArguments->SetString(0, strType);
	cefProcessMessageArguments->SetString(1, strMethod);

	CB(CefArguments.size() == 1);
	cefProcessMessageArguments->SetBool(2, CefArguments[0]->GetBoolValue());

	CB((browser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return r;
}
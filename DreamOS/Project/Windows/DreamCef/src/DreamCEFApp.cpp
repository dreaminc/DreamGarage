#include "DreamCEFApp.h"

DreamCEFApp::DreamCEFApp() {
	// empty
}

// CefBrowserProcessHandler methods:
void DreamCEFApp::OnContextInitialized() {
	// empty
}

// CefRenderProcessHandler
bool DreamCEFApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
	// empty
	return false;
}

void DreamCEFApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefRefPtr<CefDOMNode> pCEFDOMNode) {
	RESULT r = R_PASS;

	int cefBrowserID = pCEFBrowser->GetIdentifier();
	int cefFrameID = pCEFFrame->GetIdentifier();

	// Create the message object.
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::OnFocusedNodeChanged");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();

	// Populate the argument values.
	//cefProcessMessageArguments->SetInt(0, cefBrowserID);
	//cefProcessMessageArguments->SetInt(1, cefFrameID);

	cefProcessMessageArguments->SetString(0, pCEFDOMNode->GetElementTagName());
	cefProcessMessageArguments->SetString(1, pCEFDOMNode->GetName());
	cefProcessMessageArguments->SetString(2, pCEFDOMNode->GetValue());
	
	cefProcessMessageArguments->SetInt(0, (int)(pCEFDOMNode->GetType()));

	cefProcessMessageArguments->SetBool(0, (bool)(pCEFDOMNode->IsEditable()));

	CB((pCEFBrowser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return;
}

void DreamCEFApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
	// empty
}
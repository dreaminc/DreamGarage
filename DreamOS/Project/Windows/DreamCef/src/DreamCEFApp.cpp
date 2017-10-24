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

void DreamCEFApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) {
	RESULT r = R_PASS;

	// Create the message object.
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("testing");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();

	// Populate the argument values.
	cefProcessMessageArguments->SetString(0, "testing string");
	cefProcessMessageArguments->SetInt(0, 10);

	CB((pCEFBrowser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return;
}

void DreamCEFApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
	// empty
	int a = 5;
}
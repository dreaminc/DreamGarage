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
	int64 cefFrameID = pCEFFrame->GetIdentifier();

	// Create the message object.
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::OnFocusedNodeChanged");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
	cefProcessMessageArguments->SetSize(6);
	// Populate the argument values.
	//cefProcessMessageArguments->SetInt(0, cefBrowserID);
	//cefProcessMessageArguments->SetInt(1, cefFrameID);

	if (pCEFDOMNode != nullptr) {
		cefProcessMessageArguments->SetString(0, pCEFDOMNode->GetElementTagName());
		cefProcessMessageArguments->SetString(1, pCEFDOMNode->GetName());
		cefProcessMessageArguments->SetString(2, pCEFDOMNode->GetValue());

		cefProcessMessageArguments->SetBool(3, pCEFDOMNode->IsEditable());

		int cefDOMNodeType = pCEFDOMNode->GetType();
		cefProcessMessageArguments->SetInt(4, cefDOMNodeType);

		cefProcessMessageArguments->SetString(5, pCEFDOMNode->GetElementAttribute("type"));
	}
	else {
		cefProcessMessageArguments->SetString(0, "");
		cefProcessMessageArguments->SetString(1, "");
		cefProcessMessageArguments->SetString(2, "");
		cefProcessMessageArguments->SetBool(3, false);
		cefProcessMessageArguments->SetInt(4, 0);
		cefProcessMessageArguments->SetString(5, "");
	}

	CB((pCEFBrowser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return;
}

void DreamCEFApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
	// empty
}
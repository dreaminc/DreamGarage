#include "CEFApp.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CEFHandler.h"

CEFApp::CEFApp() {
	// empty
}

void CEFApp::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

	// SimpleHandler implements browser-level callbacks.
	CefRefPtr<CEFHandler> pCEFHandler(CEFHandler::instance());

	// Specify CEF browser settings here.
	CefBrowserSettings cefBrowserSettings;

	std::string strURL;
	strURL = "http://www.google.com";

	// Information used when creating the native window.
	CefWindowInfo cefWindowInfo;

	// On Windows we need to specify certain flags that will be passed to CreateWindowEx().
	cefWindowInfo.SetAsPopup(nullptr, "cefsimple");

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr);
}

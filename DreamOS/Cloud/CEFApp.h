#ifndef CEF_APP_H_
#define CEF_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CEFApp.h
// The CEF App

#include "include\cef_app.h"

#include "include\cef_browser.h"
#include "include\cef_command_line.h"
#include "include\wrapper/cef_helpers.h"

#include "CEFHandler.h"

// TODO: Put in separate file
class CEFApp : public CefApp, public CefBrowserProcessHandler {
public:
	CEFApp() {
		// empty
	}

	~CEFApp() {
		// empty
	}

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(CEFApp);
};

void CEFApp::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

	// Information used when creating the native window.
	CefWindowInfo window_info;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	window_info.SetAsPopup(NULL, "cefsimple");
#endif

	// SimpleHandler implements browser-level callbacks.
	CefRefPtr<CEFHandler> handler(new CEFHandler());

	// Specify CEF browser settings here.
	CefBrowserSettings browser_settings;

	std::string url;

	// Check if a "--url=" value was provided via the command-line. If so, use
	// that instead of the default URL.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

	url = command_line->GetSwitchValue("url");

	if (url.empty())
		url = "http://www.google.com";

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(window_info, handler.get(), url,
		browser_settings, NULL);
}

#endif // !CEF_APP_H_
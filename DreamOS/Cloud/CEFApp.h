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

#endif // !CEF_APP_H_
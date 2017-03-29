#ifndef CEF_APP_H_
#define CEF_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFApp.h

#include "include/cef_app.h"

class CEFApp : public CefApp, public CefBrowserProcessHandler {
public:
	CEFApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CEFApp);
};

#endif // !CEF_HANDLER_H_
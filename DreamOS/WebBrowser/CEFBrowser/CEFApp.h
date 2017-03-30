#ifndef CEF_APP_H_
#define CEF_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFApp.h

#include "Primitives/singleton.h"

#include <memory>
#include <list>

#include "include/cef_app.h"

class WebBrowserController;

class CEFApp : public singleton<CEFApp>, public CefApp, public CefBrowserProcessHandler {
public:
	CEFApp();

public:
	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

	std::shared_ptr<WebBrowserController> CreateBrowser(int width, int height, const std::string& strURL);

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CEFApp);
};

#endif // !CEF_HANDLER_H_
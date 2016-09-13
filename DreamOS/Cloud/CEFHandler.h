#ifndef CEF_HANDLER_H_
#define CEF_HANDLER_H_

#include "RESULT\EHM.h"

// DREAM OS
// DreamOS/Cloud/CEFHandler.h
// The CEF Handler

#include "include\cef_client.h"
#include "include\cef_display_handler.h"
#include "include\cef_life_span_handler.h"
#include "include\cef_load_handler.h"

#include "include\internal\cef_ptr.h"
#include "include\internal\cef_string.h"
#include "include\base\cef_build.h"
#include "include\cef_browser.h"
#include "include\cef_frame.h"

#include <list>

class CEFHandler : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler {
public:
	CEFHandler();
	~CEFHandler();

	// Provide access to the single global instance of this object.
	static CEFHandler* GetInstance();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
		return this;
	}
	
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
		return this;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
		return this;
	}

	// CefDisplayHandler methods:
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsClosing() const { return is_closing_; }

private:
	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;

	bool is_closing_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CEFHandler);
};

#endif // ! CEF_HANDLER_H_
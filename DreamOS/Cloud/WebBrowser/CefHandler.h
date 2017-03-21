#ifndef CEF_HANDLER_H_
#define CEF_HANDLER_H_


// DREAM OS
// DreamOS/Cloud/WebBrowser/CefHandler.h
//

#include "browser.h"

#include "CefBrowserController.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <list>
#include <future>

#include "RESULT/EHM.h"


class CefHandler : 
	public CefApp,
	public CefBrowserProcessHandler,
	public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRenderHandler
{
public:
	CefHandler();
	~CefHandler();

	static CefHandler* GetInstance();

	// CefApp
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// CefBrowserProcessHandler
	virtual void OnContextInitialized() override;

	// CefClient
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { 
		return this;
	};

	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
		return this;
	}

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
		return this;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override {
		return this;
	}

	// CefDisplayHandler
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) override;

	// CefLifeSpanHandler
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) override;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsShuttingDown() const { return m_isShuttingDown; }

	// CefRenderHandler
	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;

public:
	WebBrowserController*	CreateBrowser(unsigned int width, unsigned int height, const std::string& url);

private:
	inline bool DelegateToController(CefRefPtr<CefBrowser> browser, std::function<void(CefBrowserController* controller)> func);

private:
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
	BrowserList m_browsers;

	bool m_isShuttingDown;

	std::map<CefRefPtr<CefBrowser>, CefBrowserController*> m_browserMap;
	std::promise<CefBrowserController*> m_NewWebBrowserControllerPromise;

	IMPLEMENT_REFCOUNTING(CefHandler);
};

#endif // !CEF_HANDLER_H_
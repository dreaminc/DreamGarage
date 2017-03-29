#ifndef CEF_HANDLER_H_
#define CEF_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefHandler.h

#include "WebBrowser/WebBrowserController.h"

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



class CEFHandler : 
	//public CefApp,
	//public CefBrowserProcessHandler,
	public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler
	//public CefRenderHandler
{
private:
	CEFHandler();
	~CEFHandler();

public:

	// CefApp
	/*
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { 
		return this; 
	}
	*/

	// CefBrowserProcessHandler
	//virtual void OnContextInitialized() override;

	// CefClient
	/*
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { 
		return this; 
	}
	*/

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
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

	// CefLifeSpanHandler
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode,
						     const CefString& errorText, const CefString& failedUrl) override;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool fForceClose);
	bool IsShuttingDown() const { return m_fShuttingdown; }

	// CefRenderHandler
	//virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
	//virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;

public:
	WebBrowserController*	CreateBrowser(unsigned int width, unsigned int height, const std::string& url);

private:
	std::list<CefRefPtr<CefBrowser>> m_cefBrowsers;
	bool m_fShuttingdown = false;

	IMPLEMENT_REFCOUNTING(CEFHandler);

	// TODO: Replace with Singleton pattern / manager
	// Singleton Usage
protected:
	static CEFHandler *s_pInstance;

public:
	static CEFHandler *instance() {
		if (!s_pInstance)
			s_pInstance = new CEFHandler();

		return s_pInstance;
	}
};

#endif // !CEF_HANDLER_H_
#ifndef CEF_HANDLER_H_
#define CEF_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefHandler.h

#include "Primitives/singleton.h"

#include "WebBrowser/WebBrowserController.h"

#include "CEFBrowserController.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <list>
#include <future>

class CEFHandler : public singleton<CEFHandler>,
	public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRenderHandler
{
public:
	CEFHandler();
	~CEFHandler();

public:

	class CEFHandlerObserver {
	public:
		virtual RESULT OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) = 0;
	};

	RESULT RegisterCEFHandlerObserver(CEFHandlerObserver* pCEFHandlerObserver);

	// CefClient
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;

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
	virtual bool GetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	virtual void OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, PaintElementType type, const RectList &dirtyRects, const void *pBuffer, int width, int height) override;

private:
	std::list<CefRefPtr<CefBrowser>> m_cefBrowsers;
	bool m_fShuttingdown = false;

	CEFHandlerObserver* m_pCEFHandlerObserver = nullptr;

	IMPLEMENT_REFCOUNTING(CEFHandler);
};

#endif // !CEF_HANDLER_H_
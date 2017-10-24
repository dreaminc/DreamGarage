#ifndef CEF_HANDLER_H_
#define CEF_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefHandler.h

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

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
	//public CefDownloadHandler
{
public:
	CEFHandler();
	~CEFHandler();

public:

	class CEFHandlerObserver {
	public:
		virtual RESULT OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) = 0;
		virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) = 0;
		virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, PaintElementType type, const RectList &dirtyRects, const void *pBuffer, int width, int height) = 0;

		virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) = 0;
		virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) = 0;
		virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) = 0;
	};

	RESULT RegisterCEFHandlerObserver(CEFHandlerObserver* pCEFHandlerObserver);

	// CefClient
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

	//virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
	// CefDownloadHandler methods
	/*
	void OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;
	void OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;
		*/

	// CefDisplayHandler
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

	// CefLifeSpanHandler
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode,
						     const CefString& errorText, const CefString& failedUrl) override;

	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) override;
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType transition_type) override;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;


	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool fForceClose);
	bool IsShuttingDown() const { return m_fShuttingdown; }
	bool IsBrowserRunning() const { return m_fBrowserRunning; }

	// CefRenderHandler
	virtual bool GetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	virtual void OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, PaintElementType type, const RectList &dirtyRects, const void *pBuffer, int width, int height) override;

private:
	std::list<CefRefPtr<CefBrowser>> m_cefBrowsers;
	bool m_fShuttingdown = false;
	bool m_fBrowserRunning = false;

	CEFHandlerObserver* m_pCEFHandlerObserver = nullptr;

	IMPLEMENT_REFCOUNTING(CEFHandler);
};

#endif // !CEF_HANDLER_H_
#ifndef CEF_APP_H_
#define CEF_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFApp.h

#include "Primitives/singleton.h"

#include <memory>
#include <list>

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include "include/cef_app.h"

#include "CEFHandler.h"

#include "CEFAppObserver.h"

class WebBrowserController;

class CEFApp : public singleton<CEFApp>, 
	public CEFHandler::CEFHandlerObserver,
	public CefApp, 
	public CefBrowserProcessHandler
{
public:
	CEFApp();

public:
	/*
	class CEFAppObserver {
	public:
		virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) = 0;
		virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) = 0;
	};
	*/

	RESULT RegisterCEFAppObserver(CEFAppObserver* pCEFAppObserver);

	// CEFHandlerObserver
	virtual RESULT OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) override;

	virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) override;
	virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) override;

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

	std::shared_ptr<WebBrowserController> CreateBrowser(int width, int height, const std::string& strURL);

	// CEFAppObserver
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) override;

private:
	// Include the default reference counting implementation.
	std::promise<std::shared_ptr<CEFBrowserController>> m_promiseCEFBrowserController;

	CEFAppObserver* m_pCEFAppObserver = nullptr;

	IMPLEMENT_REFCOUNTING(CEFApp);
};

#endif // !CEF_HANDLER_H_
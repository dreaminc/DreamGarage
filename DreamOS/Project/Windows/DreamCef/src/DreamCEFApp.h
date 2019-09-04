#ifndef DREAM_CEF_APP_H_
#define DREAM_CEF_APP_H_

#include "RESULT/EHM.h"

// DREAM CEF


#include <memory>
#include <list>

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include "include/cef_app.h"

//#include "CEFHandler.h"
//#include "CEFAppObserver.h"

//class WebBrowserController;
class CEFV8Handler;

class CEFExtension;

class DreamCEFApp :  //public CEFHandler::CEFHandlerObserver, 
	public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler
{
public:
	DreamCEFApp();

public:
	/*
	class CEFAppObserver {
	public:
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) = 0;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) = 0;
	};
	*/

	//RESULT RegisterCEFAppObserver(CEFAppObserver* pCEFAppObserver);

	// CEFHandlerObserver
	//virtual RESULT OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) override;
	//virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) override;
	//virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) override;
	//virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) override;

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
		return this;
	}

	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override {

		command_line->AppendSwitchWithValue(L"autoplay-policy", L"no-user-gesture-required");
		
		command_line->AppendSwitchWithValue(L"disable-blink-features", L"RootLayerScrolling");

		command_line->AppendSwitchWithValue(L"disable-features", L"AsyncWheelEvents");
		command_line->AppendSwitchWithValue(L"disable-features", L"TouchpadAndWheelScrollLatching");

		//command_line->AppendSwitch(L"mute-audio");
		command_line->AppendSwitch(L"enable-widevine-cdm");

		command_line->AppendSwitch(L"enable-media-stream");
		command_line->AppendSwitch(L"use-dream-device-for-media-stream");

		return;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

	//std::shared_ptr<WebBrowserController> CreateBrowser(int width, int height, const std::string& strURL);

	// CefRenderProcessHandler
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
	virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override;
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
	virtual void OnWebKitInitialized() override;

	// CEFAppObserver
	//virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	//virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) override;

private:
	// Include the default reference counting implementation.
	//std::promise<std::shared_ptr<CEFBrowserController>> m_promiseCEFBrowserController;

	CefRefPtr<CEFV8Handler> m_pCEFV8Handler = nullptr;
	CEFExtension *m_pCEFDreamExtension = nullptr;

	IMPLEMENT_REFCOUNTING(DreamCEFApp);
};

#endif // !DREAM_CEF_APP
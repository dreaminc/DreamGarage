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
#include "include/cef_load_handler.h"

#include "CEFHandler.h"

#include "CEFAppObserver.h"

class WebBrowserController;


class CEFApp : public singleton<CEFApp>, 
	public CEFHandler::CEFHandlerObserver,
	public CefApp, 
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler
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
	virtual RESULT OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::ErrorCode errorCode, const CefString& strError, const CefString& strFailedURL) override;
	virtual RESULT OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) override;

	virtual RESULT HandleDreamExtensionCall(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefListValue> pMessageArguments) override;

	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL)  override;
	virtual RESULT CheckForHeaders(std::multimap<std::string, std::string> &headermap, CefRefPtr<CefBrowser> pCefBrowser, std::string strURL) override;
	virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser, cef_errorcode_t cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) override;

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
		//command_line->AppendSwitch(L"enable-exclusive-audio");
		command_line->AppendSwitch(L"enable-widevine-cdm");

		command_line->AppendSwitchWithValue(L"--audio-buffer-size", L"480");

		command_line->AppendSwitch(L"enable-media-stream");
		command_line->AppendSwitch(L"use-dream-device-for-media-stream");

		return;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

	std::shared_ptr<WebBrowserController> CreateBrowser(int width, int height, const std::string& strURL);

	// CefRenderProcessHandler
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
	virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override;
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
	virtual void OnWebKitInitialized() override;

	// CEFAppObserver
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) override;
	virtual RESULT OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
	virtual RESULT OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int frames, int channels, int bitsPerSample, const void* pDataBuffer) override;

private:
	// Include the default reference counting implementation.
	std::promise<std::shared_ptr<CEFBrowserController>> m_promiseCEFBrowserController;

	CEFAppObserver* m_pCEFAppObserver = nullptr;

	IMPLEMENT_REFCOUNTING(CEFApp);
};

#endif // !CEF_HANDLER_H_
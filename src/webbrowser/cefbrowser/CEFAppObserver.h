#ifndef CEF_APP_OBSERVER_H_
#define CEF_APP_OBSERVER_H_

#include "core/ehm/EHM.h"

// Dream WebBrowser CEF
// dos/src/WebBrowser/CEFBrowser/CEFApp.h

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include "include/internal/cef_ptr.h"
#include "include/cef_browser.h"
#include "include/cef_render_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_ssl_info.h"
#include "include/cef_request_handler.h"

#include "cloud/Environment/EnvironmentAsset.h"

class CEFBrowserController;
class CEFDOMNode;

class CEFAppObserver {
public:
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) = 0;
	virtual RESULT OnAfterCreated(CefRefPtr<CefBrowser> pCEFBrowser) = 0;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) = 0;
	virtual RESULT OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) = 0;
	virtual RESULT OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int audioSteamID, int frames, int channels, int bitsPerSample, const void* pDataBuffer) = 0;
	virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) = 0;
	virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) = 0;
	virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) = 0;
	virtual RESULT OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::ErrorCode errorCode, const CefString& strError, const CefString& strFailedURL) = 0;
	virtual RESULT OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) = 0;
	virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser, cef_errorcode_t cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) = 0;
	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) = 0;
	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(int cefBrowserID) = 0;
	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL) = 0;
	virtual RESULT CheckForHeaders(std::multimap<std::string, std::string> &headermap, CefRefPtr<CefBrowser> pCefBrowser, std::string strURL) = 0;
	virtual RESULT HandleDreamExtensionCall(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefListValue> pMessageArguments) = 0;
};

#endif	// ! CEF_APP_OBSERVER_H_
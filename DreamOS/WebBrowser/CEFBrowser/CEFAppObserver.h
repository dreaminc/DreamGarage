#ifndef CEF_APP_OBSERVER_H_
#define CEF_APP_OBSERVER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFApp.h

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

class CEFBrowserController;
class CEFDOMNode;

class CEFAppObserver {
public:
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) = 0;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) = 0;
	virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) = 0;
	virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) = 0;
	virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) = 0;
	virtual RESULT OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) = 0;
	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) = 0;
	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(int cefBrowserID) = 0;
};

#endif	// ! CEF_APP_OBSERVER_H_
#ifndef CEF_BROWSER_CONTROLLER_H_
#define CEF_BROWSER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFBrowserController.h

#include "WebBrowser/WebBrowserController.h"

/*
#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"
*/

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include "include/cef_render_handler.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <list>
#include <future>

class CefBrowser;

class CEFBrowserController :  public WebBrowserController {

	RESULT RegisterCEFAppObserver(CEFBrowserController::observer* pCEFBrowserControllerObserver);
public:
	CEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser);

	// WebBrowserController
	RESULT PollFrame();
	virtual RESULT PollNewDirtyFrames(int &rNumFramesProcessed) override;
	virtual RESULT Resize(unsigned int width, unsigned int height) override;
	virtual RESULT SendKeySequence(const std::string& keys) override;
	virtual RESULT LoadURL(const std::string& url) override;

	// WebBrowser Controller Render Handling
	RESULT OnGetViewRect(CefRect &cefRect);
	RESULT OnPaint(CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height);

	virtual RESULT SendMouseClick(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseUp, int clickCount = 1) override;
	virtual RESULT SendMouseMove(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseLeave = false) override; 

	CefRefPtr<CefBrowser> GetCEFBrowser();

private:
	// browser logical size
	int m_browserWidth = 0;
	int m_browserHeight = 0;

	// Buffer for the browser content to render into
	std::vector<unsigned char> m_vectorBuffer;
											   
	// browser physical size (buffer size)
	int m_bufferWidth = 0;
	int m_bufferHeight = 0;

	// reference to the browser object
	CefRefPtr<CefBrowser> m_pCEFBrowser;

	// Represented new dirty frames since last time they were polled
	CefRenderHandler::RectList m_NewDirtyFrames;

	std::mutex m_BufferMutex;

	//IMPLEMENT_REFCOUNTING(CEFBrowserController);
};

#endif // ! CEF_BROWSER_CONTROLLER_H_
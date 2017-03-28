#ifndef CEF_BROWSER_CONTROLLER_H_
#define CEF_BROWSER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefBrowserController.h
// 

#include "browser.h"

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

class CEFBrowserController : public WebBrowserController, public CefRenderHandler
{
public:
	CEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) :
		m_pCEFBrowser(pCEFBrowser) {}

	// WebBrowserController
	virtual void PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) override;
	virtual int PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) override;
	virtual void Resize(unsigned int width, unsigned int height) override;
	virtual void SendKeySequence(const std::string& keys) override;
	virtual void LoadURL(const std::string& url) override;

	// CefRenderHandler
	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
	virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;

private:
	// browser logical size
	int m_browserWidth = 0;
	int m_browserHeight = 0;

	// buffer for the browser content to render into
	std::vector<unsigned char>	m_buffer;
											   
	// browser physical size (buffer size)
	int m_bufferWidth = 0;
	int m_bufferHeight = 0;

	// reference to the browser object
	CefRefPtr<CefBrowser> m_pCEFBrowser;

	// represented new dirty frames since last time they were polled
	RectList m_NewDirtyFrames; 

	std::mutex m_BufferMutex;

	IMPLEMENT_REFCOUNTING(CEFBrowserController);
};

#endif // !CEF_BROWSER_CONTROLLER_H_
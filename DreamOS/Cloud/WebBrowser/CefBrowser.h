#ifndef CEF_BROWSER_H_
#define CEF_BROWSER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefBrowser.h
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

class CefBrowserServiceImp :
	public WebBrowserService
{
public:
	CefBrowserServiceImp();
	virtual ~CefBrowserServiceImp();

	RESULT Initialize() override;
	std::shared_ptr<WebBrowserController> CreateNewWebBrowser(const std::string& url, unsigned int width, unsigned int height) override;

private:
	void UnInitialize();

	void ServiceThread();

private:
	const std::string	k_CefProcessName = "DreamCef.exe";

	std::thread m_ServiceThread;

	enum class InitState { Initializing, Initialized, Failed };
	InitState m_InitState = InitState::Initializing;
	bool m_IsReady = false;

	std::mutex m_Mutex;
	std::condition_variable m_BrowserInit;
	std::condition_variable m_BrowserReady;

	std::vector<std::shared_ptr<WebBrowserController>>	m_OpenBrowsers;
};

class CefBrowserImp :
	public WebBrowserController,
	public CefClient,
	public CefRenderHandler
{
public:
	virtual ~CefBrowserImp();

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
	virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;

	virtual void PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) override;
	virtual int PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) override;

	IMPLEMENT_REFCOUNTING(CefBrowserImp);
};

class CefBufferedBrowserImp :
	public CefBrowserImp
{
public:
	CefBufferedBrowserImp(unsigned int width, unsigned int height);
	virtual ~CefBufferedBrowserImp();

	CefRefPtr<CefRenderHandler> GetRenderHandler() override;
	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;

	void PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) override;
	int PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) override;

private:
	int m_width		= 0;
	int m_height	= 0;

	std::vector<unsigned char>	m_FrontBuffer; // buffer to write to

	RectList	m_NewDirtyFrames; // representd new dirty frames since last time they were polled
	
	std::mutex	m_BufferMutex;
};

#endif // !CEF_BROWSER_H_
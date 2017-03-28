#ifndef CEF_BROWSER_SERVICE_H_
#define CEF_BROWSER_SERVICE_H_

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefBrowserService.h
// 

#include "WebBrowserService.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include "RESULT/EHM.h"


class CefBrowserService : public WebBrowserService {
public:
	CefBrowserService();
	virtual ~CefBrowserService();

	RESULT Initialize() override;
	WebBrowserController* CreateNewWebBrowser(const std::string& url, unsigned int width, unsigned int height) override;

private:
	void UnInitialize();

	void ServiceThread();

private:
	const std::string	k_CefProcessName = "DreamCef.exe";

	std::thread m_ServiceThread;

	enum class InitState { Initializing, Initialized, Failed };
	InitState m_InitState = InitState::Initializing;

	std::mutex m_Mutex;
	std::condition_variable m_BrowserInit;
};

#endif // !CEF_BROWSER_SERVICE_H_
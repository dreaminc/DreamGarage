#ifndef CEF_BROWSER_SERVICE_H_
#define CEF_BROWSER_SERVICE_H_

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefBrowserService.h
// 

#include "WebBrowser/WebBrowserService.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include "RESULT/EHM.h"

#define CEF_PROCESS_NAME_DEFAULT "DreamCef.exe"

class CEFBrowserService : public WebBrowserService {
	enum class state {
		UNINITIALIZED,
		INITIALIZING,
		INITIALIZED, 
		INITIALIZATION_FAILED,
		INVALID
	};

public:
	CEFBrowserService();
	virtual ~CEFBrowserService();

	virtual RESULT Initialize() override;
	virtual WebBrowserController* CreateNewWebBrowser(const std::string& strURL, unsigned int width, unsigned int height) override;

private:
	RESULT Deinitialize();
	RESULT ServiceThread();

private:
	const std::string k_CEFProcessName = CEF_PROCESS_NAME_DEFAULT;
	std::thread m_ServiceThread;

	state m_state = state::UNINITIALIZED;

	std::mutex m_Mutex;
	std::condition_variable m_BrowserInit;
};

#endif // !CEF_BROWSER_SERVICE_H_
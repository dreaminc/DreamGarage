#ifndef CEF_BROWSER_MANAGER_H_
#define CEF_BROWSER_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/CEFBrowser/CEFBrowserManager.h
// The CEF implementation of browser manager

#define CEF_PROCESS_NAME_DEFAULT "DreamCef.exe"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include "WebBrowser/WebBrowserManager.h"

class CEFApp;
class CEFBrowserController;

class CEFBrowserManager : public WebBrowserManager {
public:
	enum class state {
		UNINITIALIZED,
		INITIALIZING,
		INITIALIZED,
		INITIALIZATION_FAILED,
		INVALID
	};

public:
	virtual RESULT Initialize() override;
	virtual RESULT Update() override;
	virtual RESULT Shutdown() override;

	virtual std::shared_ptr<WebBrowserController> MakeNewBrowser(int width, int height, const std::string& strURL) override;

private:
	RESULT CEFManagerThread();

private:
	state m_state = state::UNINITIALIZED;

	const std::string k_CEFProcessName = CEF_PROCESS_NAME_DEFAULT;
	std::thread m_ServiceThread;

	std::mutex m_mutex;
	std::condition_variable m_condBrowserInit;
};

#endif // ! CEF_BROWSER_MANAGER_H_
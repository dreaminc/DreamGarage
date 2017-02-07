#include "CefBrowserService.h"

#include "CefHandler.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "easylogging++.h"


CefBrowserService::CefBrowserService() {

}

CefBrowserService::~CefBrowserService() {
	UnInitialize();
}

RESULT CefBrowserService::Initialize() {
	RESULT r = R_PASS;

	LOG(INFO) << "Initializing Cef thread..";

	m_ServiceThread = std::thread(&CefBrowserService::ServiceThread, this);

	// wait for Cef to initialize

	std::unique_lock<std::mutex> lk(m_Mutex);
	m_BrowserInit.wait(lk, [&] {return (m_InitState != InitState::Initializing); });

	LOG(INFO) << "Initializing Cef thread..";

	switch (m_InitState)
	{
	case CefBrowserService::InitState::Initializing:
		LOG(ERROR) << "Cef initialize failed (Initializing state)";
		r = R_FAIL;
		break;
	case CefBrowserService::InitState::Initialized:
		LOG(INFO) << "Cef initialize complete";
		r = R_PASS;
		break;
	case CefBrowserService::InitState::Failed:
		LOG(ERROR) << "Cef initialize failed (Failed state)";
		r = R_FAIL;
		break;
	default:
		break;
	}

	return r;
}

void CefBrowserService::UnInitialize() {
	LOG(INFO) << "Cef force shutdown";

	if (CefHandler::GetInstance()) {
		CefHandler::GetInstance()->CloseAllBrowsers(true);
	}

	if (m_ServiceThread.joinable())
		m_ServiceThread.join();
}

void CefBrowserService::ServiceThread() {
	CefSettings m_CEFSettings;

	const CefMainArgs CEFMainArgs(GetModuleHandle(NULL));

	void* CEFSandboxInfo = nullptr;

	int exitCode = CefExecuteProcess(CEFMainArgs, nullptr, nullptr);

	LOG(INFO) << "CefExecuteProcess returned " << exitCode;

	// Initialize CEF.
	CefString(&m_CEFSettings.browser_subprocess_path) = k_CefProcessName;
	CefString(&m_CEFSettings.locale) = "en";

	CefRefPtr<CefHandler> handler(new CefHandler());

	if (!CefInitialize(CEFMainArgs, m_CEFSettings, handler.get(), nullptr)) {
		LOG(ERROR) << "CefInitialized faild.";

		m_InitState = InitState::Failed;
		m_BrowserInit.notify_one();
		return;
	}

	LOG(INFO) << "CefInitialize completed successfully";

	m_InitState = InitState::Initialized;
	m_BrowserInit.notify_one();

	LOG(INFO) << "Run message loop";

	CefRunMessageLoop();

	LOG(INFO) << "Shutting down...";

	CefShutdown();

	LOG(INFO) << "Exited";
}

WebBrowserController* CefBrowserService::CreateNewWebBrowser(const std::string& url, unsigned int width, unsigned int height) {
	if (!CefHandler::GetInstance()) {
		LOG(ERROR) << "CefHandler not initialized";
		return nullptr;
	}

	return CefHandler::GetInstance()->CreateBrowser(width, height, url);
}

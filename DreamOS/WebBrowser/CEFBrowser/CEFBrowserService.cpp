#include "CefBrowserService.h"

#include "CefHandler.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "easylogging++.h"


CEFBrowserService::CEFBrowserService()  {
	// empty
}

CEFBrowserService::~CEFBrowserService() {
	RESULT r = R_PASS;

	CR(Deinitialize());

Error:
	return;
}

RESULT CEFBrowserService::Initialize() {
	RESULT r = R_PASS;

	LOG(INFO) << "Initializing CEF thread..";

	// TODO: Formalize
	m_ServiceThread = std::thread(&CEFBrowserService::ServiceThread, this);

	// Wait for CEF to initialize

	std::unique_lock<std::mutex> lk(m_Mutex);
	m_BrowserInit.wait(lk, [&] {return (m_state != state::INITIALIZING); });

	LOG(INFO) << "Initializing CEF thread..";

	switch (m_state) {
		case CEFBrowserService::state::INITIALIZING: {
			LOG(ERROR) << "CEF initialize failed (Initializing state)";
			r = R_FAIL;
		} break;

		case CEFBrowserService::state::INITIALIZED: {
			LOG(INFO) << "CEF initialize complete";
			r = R_PASS;
		} break;

		case CEFBrowserService::state::INITIALIZATION_FAILED: {
			LOG(ERROR) << "CEF initialize failed (Failed state)";
			r = R_FAIL;
		} break;
	}

//Error:
	return r;
}

RESULT CEFBrowserService::Deinitialize() {
	RESULT r = R_PASS;

	LOG(INFO) << "CEF force shutdown";

	if (CEFHandler::GetInstance()) {
		CEFHandler::GetInstance()->CloseAllBrowsers(true);
	}

	if (m_ServiceThread.joinable())
		m_ServiceThread.join();

//Error:
	return r;
}

RESULT CEFBrowserService::ServiceThread() {
	RESULT r = R_PASS;

	CefSettings m_CEFSettings;

	const CefMainArgs CEFMainArgs(GetModuleHandle(NULL));

	void* CEFSandboxInfo = nullptr;

	int exitCode = CefExecuteProcess(CEFMainArgs, nullptr, nullptr);
	LOG(INFO) << "CefExecuteProcess returned " << exitCode;

	// Initialize CEF.
	CefString(&m_CEFSettings.browser_subprocess_path) = k_CEFProcessName;
	CefString(&m_CEFSettings.locale) = "en";

	CefRefPtr<CEFHandler> cefHandler(new CEFHandler());

	if (!CefInitialize(CEFMainArgs, m_CEFSettings, cefHandler.get(), nullptr)) {
		LOG(ERROR) << "CEFInitialized failed.";

		m_state = state::INITIALIZATION_FAILED;
		m_BrowserInit.notify_one();
		return r ;
	}

	LOG(INFO) << "CefInitialize completed successfully";

	m_state = state::INITIALIZED;
	m_BrowserInit.notify_one();

	LOG(INFO) << "Run message loop";
	CefRunMessageLoop();

	LOG(INFO) << "Shutting down...";

//Error:
	CefShutdown();
	LOG(INFO) << "Exited";
	return r;
}

WebBrowserController* CEFBrowserService::CreateNewWebBrowser(const std::string& strURL, unsigned int width, unsigned int height) {
	if (!CEFHandler::GetInstance()) {
		LOG(ERROR) << "CefHandler not initialized";
		return nullptr;
	}

	return CEFHandler::GetInstance()->CreateBrowser(width, height, strURL);
}

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

	DEBUG_LINEOUT("Initializing CEF thread..");

	// TODO: Formalize
	m_ServiceThread = std::thread(&CEFBrowserService::ServiceThread, this);

	// Wait for CEF to initialize

	std::unique_lock<std::mutex> lockCEFBrowserInitialization(m_Mutex);

	m_BrowserInit.wait(lockCEFBrowserInitialization, 
		[&] {
			return (m_state != state::UNINITIALIZED); 
		}
	);

	switch (m_state) {
		case CEFBrowserService::state::INITIALIZING: {
			DEBUG_LINEOUT("CEF initialize failed (Initializing state)");
			r = R_FAIL;
		} break;

		case CEFBrowserService::state::INITIALIZED: {
			DEBUG_LINEOUT("CEF initialize complete");
			r = R_PASS;
		} break;

		case CEFBrowserService::state::INITIALIZATION_FAILED: {
			DEBUG_LINEOUT("CEF initialize failed (Failed state)");
			r = R_FAIL;
		} break;
	}

//Error:
	return r;
}

RESULT CEFBrowserService::Deinitialize() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEF force shutdown");

	if (CEFHandler::instance()) {
		CEFHandler::instance()->CloseAllBrowsers(true);
	}

	if (m_ServiceThread.joinable())
		m_ServiceThread.join();

//Error:
	return r;
}

RESULT CEFBrowserService::ServiceThread() {
	RESULT r = R_PASS;

	CefSettings cefSettings;

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	CefMainArgs CEFMainArgs(hInstance);

	int exitCode = CefExecuteProcess(CEFMainArgs, nullptr, nullptr);
	DEBUG_LINEOUT("CefExecuteProcess returned %d", exitCode);

	// Initialize CEF
	CefString(&cefSettings.browser_subprocess_path) = k_CEFProcessName;
	CefString(&cefSettings.locale) = "en";

	//m_CEFSettings.no_sandbox = true;

	CefRefPtr<CEFHandler> pCEFHandler(CEFHandler::instance());

	/*
	if (CefInitialize(CEFMainArgs, cefSettings, pCEFHandler.get(), nullptr) == false) {
		DEBUG_LINEOUT("CEFInitialized failed.");

		m_state = state::INITIALIZATION_FAILED;
		m_BrowserInit.notify_one();

		return r;
	}
	*/

	DEBUG_LINEOUT("CefInitialize completed successfully");

	m_state = state::INITIALIZED;
	m_BrowserInit.notify_one();

	DEBUG_LINEOUT("CEF Run message loop");
	CefRunMessageLoop();

	DEBUG_LINEOUT("CEF Shutting down...");

//Error:
	CefShutdown();
	DEBUG_LINEOUT("CEF Exited");
	return r;
}

WebBrowserController* CEFBrowserService::CreateNewWebBrowser(const std::string& strURL, unsigned int width, unsigned int height) {
	if (!CEFHandler::instance()) {
		DEBUG_LINEOUT("CefHandler not initialized");
		return nullptr;
	}

	//return CEFHandler::instance()->CreateBrowser(width, height, strURL);
	return nullptr;
}

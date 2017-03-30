#include "CEFApp.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CEFHandler.h"

// Initialize and allocate the instance
CEFApp* singleton<CEFApp>::s_pInstance = nullptr;

CEFApp::CEFApp() {
	// empty
}

void CEFApp::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

	// SimpleHandler implements browser-level callbacks.
	//CefRefPtr<CEFHandler> pCEFHandler = CefRefPtr<CEFHandler>(CEFHandler::instance());

	/*
	// Specify CEF browser settings here.
	CefBrowserSettings cefBrowserSettings;

	std::string strURL;
	strURL = "http://www.google.com";

	// Information used when creating the native window.
	CefWindowInfo cefWindowInfo;

	// On Windows we need to specify certain flags that will be passed to CreateWindowEx().
	cefWindowInfo.SetAsPopup(nullptr, "cefsimple");

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr);
	*/
}

std::shared_ptr<WebBrowserController> CEFApp::CreateBrowser(int width, int height, const std::string& strURL) {
	RESULT r = R_PASS;
	std::shared_ptr<WebBrowserController> pWebBrowserController = nullptr;

	DEBUG_LINEOUT("CEFApp: CreateBrowser");

	CefRefPtr<CEFHandler> pCEFHandler = CefRefPtr<CEFHandler>(CEFHandler::instance());

	CefWindowInfo cefWindowInfo;
	CefBrowserSettings cefBrowserSettings;

	//cefWindowInfo.SetAsWindowless(0, false);
	cefWindowInfo.SetAsPopup(nullptr, "cefsimple");
	cefWindowInfo.width = width;
	cefWindowInfo.height = height;

	// clear the promise for reuse
	/*
	m_NewWebBrowserControllerPromise = std::promise<CEFBrowserController*>();
	auto newBrowser = m_NewWebBrowserControllerPromise.get_future();
	*/

	if (CefBrowserHost::CreateBrowser(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr) == false) {
		DEBUG_LINEOUT("CreateBrowser failed");
		return nullptr;
	}

	// Blocks until promise is settled
	/*
	WebBrowserController* pBrowserController = newBrowser.get();
	pBrowserController->Resize(width, height);
	return pBrowserController;
	*/

//Error:
	return pWebBrowserController;
}

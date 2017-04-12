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

RESULT CEFApp::RegisterCEFAppObserver(CEFAppObserver* pCEFAppObserver) {
	RESULT r = R_PASS;

	CBM((m_pCEFAppObserver == nullptr), "CEFAppObserver already registered");
	CN(pCEFAppObserver);

	m_pCEFAppObserver = pCEFAppObserver;

Error:
	return r;
}

// CEFAppObserver
RESULT CEFApp::OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnGetViewRect(pCEFBrowser, cefRect));

Error:
	return r;
}

RESULT CEFApp::OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnPaint(pCEFBrowser, type, dirtyRects, pBuffer, width, height));

Error:
	return r;
}

void CEFApp::OnContextInitialized() {
	RESULT r = R_PASS;

	CEF_REQUIRE_UI_THREAD();

	// SimpleHandler implements browser-level callbacks.
	CEFHandler *pCEFHandler = CEFHandler::instance();
	CN(pCEFHandler);

	CR(pCEFHandler->RegisterCEFHandlerObserver(this));

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

Error:
	return;
}

RESULT CEFApp::OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) {
	RESULT r = R_PASS;

	CN(pCEFBrowserController);

	m_promiseCEFBrowserController.set_value(pCEFBrowserController);

Error:
	return r;
}

std::shared_ptr<WebBrowserController> CEFApp::CreateBrowser(int width, int height, const std::string& strURL) {
	RESULT r = R_PASS;
	std::shared_ptr<WebBrowserController> pWebBrowserController = nullptr;

	DEBUG_LINEOUT("CEFApp: CreateBrowser");

	CefRefPtr<CEFHandler> pCEFHandler = CefRefPtr<CEFHandler>(CEFHandler::instance());

	CefWindowInfo cefWindowInfo;
	CefBrowserSettings cefBrowserSettings;

	cefWindowInfo.SetAsWindowless(0, true);
	//cefWindowInfo.SetAsPopup(nullptr, "cefsimple");
	cefWindowInfo.width = width;
	cefWindowInfo.height = height;

	// Set up the promise (Will be set in OnBrowserCreated
	
	m_promiseCEFBrowserController = std::promise<std::shared_ptr<CEFBrowserController>>();
	std::future<std::shared_ptr<CEFBrowserController>> futureCEFBrowserController = m_promiseCEFBrowserController.get_future();

	if (CefBrowserHost::CreateBrowser(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr) == false) {
	//if (CefBrowserHost::CreateBrowserSync(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr) == false) {
		DEBUG_LINEOUT("CreateBrowser failed");
		return nullptr;
	}

	// Blocks until promise is settled
	std::shared_ptr<CEFBrowserController> pCEFBrowserController = futureCEFBrowserController.get();
	CR(pCEFBrowserController->Resize(width, height));

	return pCEFBrowserController;

Error:
	return pWebBrowserController;
}
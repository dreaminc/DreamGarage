#include "WebBrowserControllerFactory.h"

// Leaving this here since we'll need it soon
#if defined(_WIN32) 
	#if defined(_WIN64)
		#include "WebBrowser/CEFBrowser/CEFBrowserController.h"
	#else
#include "WebBrowser/CEFBrowser/CEFBrowserController.h"
	#endif
#elif defined(__APPLE__)
	#include "WebBrowser/CEFBrowser/CEFBrowserController.h"
#elif defined(__linux__)
	#include "WebBrowser/CEFBrowser/CEFBrowserController.h"
#endif

WebBrowserController* WebBrowserControllerFactory::MakeWebBrowserController(WEB_BROWSER_CONTROLLER_TYPE type) {
	RESULT r = R_PASS;
	WebBrowserController *pWebBrowserController = nullptr;

	switch (type) {
		case WEB_BROWSER_CONTROLLER_TYPE::CEF: {

		/*
		std::unique_ptr<WebBrowserService> pWebBrowserService = std::unique_ptr<WebBrowserService>(new CefBrowserService());

		// Initialize
		CR(pWebBrowserService->Initialize());
		//*/

		//pWebBrowserController = new CEFBrowserController();
	} break;

	
		case WEB_BROWSER_CONTROLLER_TYPE::INVALID:
		default: {
			pWebBrowserController = nullptr;
			DEBUG_LINEOUT("Web Browser Controller type %d not supported on this platform!", type);
		} break;
	}

	// Success:
	return pWebBrowserController;

//Error:
	if (pWebBrowserController != nullptr) {
		pWebBrowserController = nullptr;
	}

	return nullptr;
}
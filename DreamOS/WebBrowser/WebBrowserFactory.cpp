#include "WebBrowserFactory.h"

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

#include "DOMNode.h"

WebBrowserController* WebBrowserFactory::MakeWebBrowserController(WEB_BROWSER_TYPE type) {
	RESULT r = R_PASS;
	WebBrowserController *pWebBrowserController = nullptr;

	switch (type) {
		case WEB_BROWSER_TYPE::CEF: {

		/*
		std::unique_ptr<WebBrowserService> pWebBrowserService = std::unique_ptr<WebBrowserService>(new CefBrowserService());

		// Initialize
		CR(pWebBrowserService->Initialize());
		//*/

		//pWebBrowserController = new CEFBrowserController();
	} break;

	
		case WEB_BROWSER_TYPE::INVALID:
		default: {
			pWebBrowserController = nullptr;
			DEBUG_LINEOUT("Web Browser Controller type %d not supported on this platform!", type);
		} break;
	}

	// Success:
	return pWebBrowserController;

//Error:
	if (pWebBrowserController != nullptr) {
		delete pWebBrowserController;
		pWebBrowserController = nullptr;
	}

	return nullptr;
}

/*
DOMNode* WebBrowserFactory::MakeDOMNode(WEB_BROWSER_TYPE type, void *pSource) {
	RESULT r = R_PASS;

	DOMNode *pDOMNode = nullptr;

	switch (type) {
		case WEB_BROWSER_TYPE::CEF: {
			pDOMNode = new CEFDOMNode();
		} break;

		case WEB_BROWSER_TYPE::INVALID:
		default: {
			pDOMNode = nullptr;
			DEBUG_LINEOUT("DOM Node type %d not supported on this platform!", type);
		} break;
	}


// Success:
	return pDOMNode;

Error:
	if (pDOMNode != nullptr) {
		delete pDOMNode;
		pDOMNode = nullptr;
	}

	return nullptr;
}
*/
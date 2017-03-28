#include "WebBrowserController.h"
#include "WebBrowserService.h"

// TODO: Move this into a factory
std::unique_ptr<WebBrowserService> WebBrowser::CreateNewCefBrowserService() {
	RESULT r = R_PASS;

	std::unique_ptr<WebBrowserService> pWebBrowserService = std::unique_ptr<WebBrowserService>(new CefBrowserService());
	
	// Initialize
	CR(pWebBrowserService->Initialize());


// Success:
	return pWebBrowserService;

Error:
	if (pWebBrowserService != nullptr) {
		pWebBrowserService = nullptr;
	}

	return nullptr;
}


#include "browser.h"
#include "CefBrowserService.h"

#include "easylogging++.h"

// empty stub for pure virtual destructor
WebBrowserService::~WebBrowserService() {

}

namespace WebBrowser {

std::unique_ptr<WebBrowserService> WebBrowser::CreateNewCefBrowserService() {
	auto service = std::unique_ptr<WebBrowserService>(new CefBrowserService());
	
	// init service
	if (service->Initialize() == R_FAIL) {
		return nullptr;
	}

	return service;
}

}
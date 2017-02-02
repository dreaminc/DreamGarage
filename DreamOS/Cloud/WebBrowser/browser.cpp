#include "browser.h"
#include "CefBrowser.h"

#include "easylogging++.h"

// empty stub for pure virtual destructor
WebBrowserService::~WebBrowserService() {

}

namespace WebBrowser {

std::unique_ptr<WebBrowserService> WebBrowser::CreateNewWebBrowserService() {
	auto service = std::unique_ptr<WebBrowserService>(new CefBrowserServiceImp());
	
	// init service
	if (service->Initialize() == R_FAIL) {
		return nullptr;
	}

	return service;
}

}
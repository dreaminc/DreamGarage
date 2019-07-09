#ifndef WEB_BROWSER_SERVICE_H_
#define WEB_BROWSER_SERVICE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/WebBrowserService.h

#include <string>

class WebBrowserController;

class WebBrowserService {
public:
	WebBrowserService() {
		// empty
	}

	// empty stub for pure virtual destructor
	virtual WebBrowserService::~WebBrowserService() {
		// empty
	}

public:
	virtual RESULT Initialize() = 0;

	// creates new web browser
	virtual WebBrowserController* CreateNewWebBrowser(const std::string& url, unsigned int width, unsigned int height) = 0;
};

#endif // ! WEB_BROWSER_SERVICE_H_
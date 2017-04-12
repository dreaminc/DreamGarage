#ifndef WEB_BROWSER_FACTORY_H_
#define WEB_BROWSER_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/CEFBrowser/WebBrowserControllerFactory.h

enum class WEB_BROWSER_CONTROLLER_TYPE {
	CEF,
	INVALID
};

class WebBrowserController;

class WebBrowserControllerFactory {
public:
	static WebBrowserController* MakeWebBrowserController(WEB_BROWSER_CONTROLLER_TYPE type);
};

#endif // ! WEB_BROWSER_FACTORY_H_
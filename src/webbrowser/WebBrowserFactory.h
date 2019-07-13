#ifndef WEB_BROWSER_FACTORY_H_
#define WEB_BROWSER_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/CEFBrowser/WebBrowserControllerFactory.h

enum class WEB_BROWSER_TYPE {
	CEF,
	INVALID
};

class DOMNode;
class DOMDocument;
class WebBrowserController;

class WebBrowserFactory {
public:
	static WebBrowserController* MakeWebBrowserController(WEB_BROWSER_TYPE type);
	static DOMNode* MakeDOMNode(WEB_BROWSER_TYPE type, void *pSource = nullptr);
};

#endif // ! WEB_BROWSER_FACTORY_H_
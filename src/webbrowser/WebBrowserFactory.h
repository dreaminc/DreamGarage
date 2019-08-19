#ifndef WEB_BROWSER_FACTORY_H_
#define WEB_BROWSER_FACTORY_H_

#include "core/ehm/EHM.h"

// Dream Webbrowser
// dos/src/webbrowser/WebBrowserControllerFactory.h

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
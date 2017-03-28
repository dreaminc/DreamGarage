#ifndef WEB_BROWSER_FACTORY_H_
#define WEB_BROWSER_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/CEFBrowser/WebBrowserFactor.h

enum class WEB_BROWSER_TYPE {
	CEF,
	INVALID
};

class WebBrowserFactory {
public:
	static OpenGLRenderingContext* MakeOpenGLRenderingContext(OPEN_GL_RC_TYPE type);
};

#endif // ! WEB_BROWSER_FACTORY_H_
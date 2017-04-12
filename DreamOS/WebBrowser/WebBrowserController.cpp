#include "WebBrowserController.h"

RESULT WebBrowserController::RegisterWebBrowserControllerObserver(WebBrowserController::observer* pCEFBrowserControllerObserver) {
	RESULT r = R_PASS;

	CBM((m_pWebBrowserControllerObserver == nullptr), "CEFBrowserControllerObserver already registered");
	CN(pCEFBrowserControllerObserver);

	m_pWebBrowserControllerObserver = pCEFBrowserControllerObserver;

Error:
	return r;
}
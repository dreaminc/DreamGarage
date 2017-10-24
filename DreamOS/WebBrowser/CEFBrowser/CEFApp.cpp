#include "CEFApp.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CEFHandler.h"
#include "CEFDOMNode.h"

// Initialize and allocate the instance
CEFApp* singleton<CEFApp>::s_pInstance = nullptr;

CEFApp::CEFApp() {
	// empty
}

RESULT CEFApp::RegisterCEFAppObserver(CEFAppObserver* pCEFAppObserver) {
	RESULT r = R_PASS;

	CBM((m_pCEFAppObserver == nullptr), "CEFAppObserver already registered");
	CN(pCEFAppObserver);

	m_pCEFAppObserver = pCEFAppObserver;

Error:
	return r;
}

// CEFAppObserver
RESULT CEFApp::OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnGetViewRect(pCEFBrowser, cefRect));

Error:
	return r;
}

RESULT CEFApp::OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnPaint(pCEFBrowser, type, dirtyRects, pBuffer, width, height));

Error:
	return r;
}

RESULT CEFApp::OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnLoadingStateChanged(pCEFBrowser, fLoading, fCanGoBack, fCanGoForward));

Error:
	return r;
}

RESULT CEFApp::OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnLoadStart(pCEFBrowser, pCEFFrame, transition_type));

Error:
	return r;
}

RESULT CEFApp::OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnLoadEnd(pCEFBrowser, pCEFFrame, httpStatusCode));

Error:
	return r;
}


void CEFApp::OnContextInitialized() {
	RESULT r = R_PASS;

	CEF_REQUIRE_UI_THREAD();

	// SimpleHandler implements browser-level callbacks.
	CEFHandler *pCEFHandler = CEFHandler::instance();
	CN(pCEFHandler);

	CR(pCEFHandler->RegisterCEFHandlerObserver(this));

	/*
	// Specify CEF browser settings here.
	CefBrowserSettings cefBrowserSettings;

	std::string strURL;
	strURL = "http://www.google.com";

	// Information used when creating the native window.
	CefWindowInfo cefWindowInfo;

	// On Windows we need to specify certain flags that will be passed to CreateWindowEx().
	cefWindowInfo.SetAsPopup(nullptr, "cefsimple");

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr);
	*/

Error:
	return;
}

// This handles IPC between render and browser processes 
bool CEFApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> pCEFBrowser, CefProcessId cefSourceProcessID, CefRefPtr<CefProcessMessage> pCEFProcessMessage) {
	RESULT r = R_PASS;

	if (cefSourceProcessID == PID_BROWSER) {
		/*
		std::shared_ptr<DOMNode> pDOMNode = nullptr;

		auto pCEFFrame = m_pCEFBrowser->GetFocusedFrame();
		CN(pCEFFrame);

		// Create a visitor
		CefRefPtr<CEFDOMVisitor> pCEFDOMVisitor = new CEFDOMVisitor(std::shared_ptr<CEFBrowserController>(this));
		CN(pCEFDOMVisitor);

		// Send task to render thread as needed by VisitDOM
		CBM((CefPostTask(TID_RENDERER, base::Bind(&CefFrame::VisitDOM, pCEFFrame, pCEFDOMVisitor))), 
			"Failed to post visit dom to render thread");
			*/
		int a = 5;
		printf("hi");
	}
	else if (cefSourceProcessID == PID_RENDERER) {
		int a = 5;
		printf("hi");
	}

//Error: 
	return false;
}

void CEFApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefRefPtr<CefDOMNode> pCEFDOMNode) {
	RESULT r = R_PASS;

	// This enters a cross-process boundary so we cannot really trust a lot of the handles provided
	// so capture this here and pass on

	//CEFDOMNode cefDOMNode = CEFDOMNode(pCEFDOMNode);
	//int cefBrowserID = pCEFBrowser->GetIdentifier();
	//int cefFrameID = pCEFFrame->GetIdentifier();
	//
	//if (m_pCEFAppObserver != nullptr) {
	//	CR(m_pCEFAppObserver->OnFocusedNodeChanged(cefBrowserID, cefFrameID, cefDOMNode));
	//}

	// Create the message object.
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("testing");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();

	// Populate the argument values.
	cefProcessMessageArguments->SetString(0, "testing string");
	cefProcessMessageArguments->SetInt(0, 10);

	CB((pCEFBrowser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return;
}

// This doesn't do much right now
// but this gives us back a different browser than the browser process (render process)
void CEFApp::OnBrowserCreated(CefRefPtr<CefBrowser> pCEFBrowser) {
	RESULT r = R_PASS;

	CN(pCEFBrowser);

	int browserID = pCEFBrowser->GetIdentifier();

	/*
	{
		auto pCEFBrowserController = m_pCEFAppObserver->GetCEFBrowserController(pCEFBrowser);
		CN(pCEFBrowserController);
	}
	*/

Error:
	return;
}

RESULT CEFApp::OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) {
	RESULT r = R_PASS;

	CN(pCEFBrowserController);

	m_promiseCEFBrowserController.set_value(pCEFBrowserController);

Error:
	return r;
}

std::shared_ptr<WebBrowserController> CEFApp::CreateBrowser(int width, int height, const std::string& strURL) {
	RESULT r = R_PASS;
	std::shared_ptr<WebBrowserController> pWebBrowserController = nullptr;

	DEBUG_LINEOUT("CEFApp: CreateBrowser");

	CefRefPtr<CEFHandler> pCEFHandler = CefRefPtr<CEFHandler>(CEFHandler::instance());

	CefWindowInfo cefWindowInfo;
	CefBrowserSettings cefBrowserSettings;

	cefWindowInfo.SetAsWindowless(0, true);
	//cefWindowInfo.SetAsPopup(nullptr, "cefsimple");
	cefWindowInfo.width = width;
	cefWindowInfo.height = height;

	// Set up the promise (Will be set in OnBrowserCreated)
	
	m_promiseCEFBrowserController = std::promise<std::shared_ptr<CEFBrowserController>>();
	std::future<std::shared_ptr<CEFBrowserController>> futureCEFBrowserController = m_promiseCEFBrowserController.get_future();

	if (CefBrowserHost::CreateBrowser(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr) == false) {
	//if (CefBrowserHost::CreateBrowserSync(cefWindowInfo, pCEFHandler, strURL, cefBrowserSettings, nullptr) == false) {
		DEBUG_LINEOUT("CreateBrowser failed");
		return nullptr;
	}

	// Blocks until promise is settled
	std::shared_ptr<CEFBrowserController> pCEFBrowserController = futureCEFBrowserController.get();
	CR(pCEFBrowserController->Resize(width, height));

	return pCEFBrowserController;

Error:
	return pWebBrowserController;
}
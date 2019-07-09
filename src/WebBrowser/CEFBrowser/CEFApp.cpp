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

RESULT CEFApp::OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int audioSteamID, int frames, int channels, int bitsPerSample, const void* pDataBuffer) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnAudioData(pCEFBrowser, audioSteamID, frames, channels, bitsPerSample, pDataBuffer));

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

RESULT CEFApp::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnPopupSize(browser, rect));

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

RESULT CEFApp::OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::ErrorCode errorCode, const CefString& strError, const CefString& strFailedURL) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);
	CR(m_pCEFAppObserver->OnLoadError(pCEFBrowser, pCEFFrame, errorCode, strError, strFailedURL));

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

//Error: 
	return false;
}

// If this takes too long debug will screw up the render process 
// If this is an issue, we'll need to create a pending queue arch to avoid the delay
// Meanwhile, this is a duplication of the code in DreamCEFApp in DreamCEF
// TODO: Find a good way to merge across DreamCEF and main client app
void CEFApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefRefPtr<CefDOMNode> pCEFDOMNode) {
	RESULT r = R_PASS;

	int cefBrowserID = pCEFBrowser->GetIdentifier();
	int cefFrameID = pCEFFrame->GetIdentifier();

	// Create the message object.
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("DreamCEFApp::OnFocusedNodeChanged");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> cefProcessMessageArguments = pCEFProcessMessage->GetArgumentList();

	// Populate the argument values.
	//cefProcessMessageArguments->SetInt(0, cefBrowserID);
	//cefProcessMessageArguments->SetInt(1, cefFrameID);

	if (pCEFDOMNode != nullptr) {
		cefProcessMessageArguments->SetString(0, pCEFDOMNode->GetElementTagName());
		cefProcessMessageArguments->SetString(1, pCEFDOMNode->GetName());
		cefProcessMessageArguments->SetString(2, pCEFDOMNode->GetValue());

		cefProcessMessageArguments->SetBool(3, pCEFDOMNode->IsEditable());

		//TODO: should also use the height and y coordinate of this field
		// to properly set the position of the screen during OnNodeFocusChanged
		//pCEFDOMNode->GetElementBounds();

		int cefDOMNodeType = pCEFDOMNode->GetType();
		cefProcessMessageArguments->SetInt(4, cefDOMNodeType);
	}
	else {
		cefProcessMessageArguments->SetString(0, "");
		cefProcessMessageArguments->SetString(1, "");
		cefProcessMessageArguments->SetString(2, "");
		cefProcessMessageArguments->SetBool(3, false);
		cefProcessMessageArguments->SetInt(4, 0);
	}

	CB((pCEFBrowser->SendProcessMessage(PID_BROWSER, pCEFProcessMessage)));

Error:
	return;
}

// This is the inter-process boundary call back (vs the one above)
RESULT CEFApp::OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) {
	RESULT r = R_PASS;

	if (m_pCEFAppObserver != nullptr) {
		CR(m_pCEFAppObserver->OnFocusedNodeChanged(cefBrowserID, cefFrameID, pCEFDOMNode));
	}

Error:
	return r;
}

RESULT CEFApp::HandleDreamExtensionCall(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefListValue> pMessageArguments) {
	RESULT r = R_PASS;

	if (m_pCEFAppObserver != nullptr) {
		CR(m_pCEFAppObserver->HandleDreamExtensionCall(pCefBrowser, pMessageArguments));
	}

Error:
	return r;

}

RESULT CEFApp::GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL) {
	RESULT r = R_PASS;

	if (m_pCEFAppObserver != nullptr) {
		CR(m_pCEFAppObserver->GetResourceHandlerType(resourceHandlerType, pCefBrowser, strCEFURL));
	}

Error:
	return r;
}

RESULT CEFApp::CheckForHeaders(std::multimap<std::string, std::string> &headermap, CefRefPtr<CefBrowser> pCefBrowser, std::string strURL) {
	RESULT r = R_PASS;

	if (m_pCEFAppObserver != nullptr) {
		CR(m_pCEFAppObserver->CheckForHeaders(headermap, pCefBrowser, strURL));
	}

Error:
	return r;
}

bool CEFApp::OnCertificateError(CefRefPtr<CefBrowser> browser, cef_errorcode_t cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) {
	RESULT r = R_PASS;

	CN(m_pCEFAppObserver);

	return m_pCEFAppObserver->OnCertificateError(browser, cert_error, request_url, ssl_info, callback);
Error:
	return false;
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

void CEFApp::OnWebKitInitialized() {
	/*
	RESULT r = R_PASS;

	DOSLOG(DreamLogger::Level::INFO, "OnWebKitInitialized");

	// Create an instance of my CefV8Handler object.
	m_pCEFV8Handler = new CEFV8Handler();
	CN(m_pCEFV8Handler);

	// Register Extension

	m_pCEFDreamExtension = new CEFExtension(L"DreamCEFExtension.js", m_pCEFV8Handler);
	CNM(m_pCEFDreamExtension, "Failed to allocate cef extension object");
	CRM(m_pCEFDreamExtension->Initialize(), "Failed to initialize cef extension");

Error:*/
	return;
}

RESULT CEFApp::OnBrowserCreated(std::shared_ptr<CEFBrowserController> pCEFBrowserController) {
	RESULT r = R_PASS;

	CN(pCEFBrowserController);

	m_promiseCEFBrowserController.set_value(pCEFBrowserController);

	m_pCEFAppObserver->OnAfterCreated(pCEFBrowserController->GetCEFBrowser());

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

	//cefWindowInfo.SetAsWindowless(0, true);
	cefWindowInfo.SetAsWindowless(NULL);
	//cefWindowInfo.SetAsPopup(nullptr, "cefsimple");
	cefWindowInfo.width = width;
	cefWindowInfo.height = height;
	//cefWindowInfo.transparent_painting_enabled = 0;// false;

	// Set background color to opaque white
	cefBrowserSettings.background_color = 0xFFFFFFFF;
	cefBrowserSettings.windowless_frame_rate = 24;
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

	//m_pCEFAppObserver->OnAfterCreated(pCEFBrowserController->GetCEFBrowser());

	return pCEFBrowserController;

Error:
	return pWebBrowserController;
}
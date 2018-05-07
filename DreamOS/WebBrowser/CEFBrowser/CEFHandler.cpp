#include "CefHandler.h"
#include "CefBrowserController.h"

//#include "easylogging++.h"

#include "include\cef_client.h"

#include <sstream>
#include <string>

// cef triggers this warning
#pragma warning(disable : 4067)

#include "include/cef_browser.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "CEFApp.h"

#include "Core/Utilities.h"
#include "CEFDOMNode.h"

#include "CEFResourceHandler.h"

#pragma warning(default : 4067)

// Initialize and allocate the instance
CEFHandler* singleton<CEFHandler>::s_pInstance = nullptr;

// This is not the way to do a singleton
// TODO: Do we want this to be a singleton?
CEFHandler::CEFHandler() :
	m_fShuttingdown(false) 
{
	// empty
}

CEFHandler::~CEFHandler() {
	// empty
}

RESULT CEFHandler::RegisterCEFHandlerObserver(CEFHandlerObserver* pCEFHandlerObserver) {
	RESULT r = R_PASS;

	CBM((m_pCEFHandlerObserver == nullptr), "CEFHandlerObserver already registered");
	CN(pCEFHandlerObserver);

	m_pCEFHandlerObserver = pCEFHandlerObserver;

Error:
	return r;
}

// Handler Routing
// Render handler is done at CEFApp to get access to all of the Browser Controllers

#ifdef CEF_AUDIO_MIRROR
CefRefPtr<CefAudioHandler> CEFHandler::GetAudioHandler() {
	return this;
}
#endif

CefRefPtr<CefRenderHandler> CEFHandler::GetRenderHandler() { 
	return this;
}

CefRefPtr<CefDisplayHandler> CEFHandler::GetDisplayHandler() {
	return this;
}

CefRefPtr<CefLifeSpanHandler> CEFHandler::GetLifeSpanHandler() {
	return this;
}

CefRefPtr<CefLoadHandler> CEFHandler::GetLoadHandler() {
	return this;
}

CefRefPtr<CefRequestHandler> CEFHandler::GetRequestHandler() {
	return this;
}

bool CEFHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> pCefBrowser, CefProcessId sourceCEFProcessID, CefRefPtr<CefProcessMessage> pCEFProcessMessage) {
	RESULT r = R_PASS;

	const std::string& strMessageName = pCEFProcessMessage->GetName();
	CEFDOMNode *pCEFDOMNode = nullptr;

	bool fHandled = false;

	auto tokens = util::TokenizeString(strMessageName, "::");
	CB((tokens.size() == 2));

	{
		std::string strObjectName = tokens[0];
		std::string strMethodName = tokens[1];

		if (strObjectName == "DreamCEFApp") {
			if (strMethodName == "OnFocusedNodeChanged") {
				
				auto pCEFProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
				size_t numArgs = pCEFProcessMessageArguments->GetSize();

				CB((numArgs > 0));

				int cefBrowserID = pCefBrowser->GetIdentifier();

				//CB((cefBrowserID == cefProcBrowserID));

				// Strings
				std::string strElementTagName = pCEFProcessMessageArguments->GetString(0);

				std::string strName = pCEFProcessMessageArguments->GetString(1);
				std::string strValue = pCEFProcessMessageArguments->GetString(2);

				// Editable
				bool fEditable = pCEFProcessMessageArguments->GetBool(3);

				// Type
				cef_dom_node_type_t cefDOMNodeType = (cef_dom_node_type_t)(pCEFProcessMessageArguments->GetInt(4));
				
				// Attributes
				std::string strNodeTypeAttributeValue = pCEFProcessMessageArguments->GetString(5);

				// Create the node
				pCEFDOMNode = new CEFDOMNode(cefDOMNodeType, strName, strElementTagName, strValue, fEditable, strNodeTypeAttributeValue);
			

				if (m_pCEFHandlerObserver != nullptr) {
					CR(m_pCEFHandlerObserver->OnFocusedNodeChanged(cefBrowserID, -1, pCEFDOMNode));
				}

				fHandled = true;
			}
		}
	}

Error:
	if (pCEFDOMNode != nullptr) {
		delete pCEFDOMNode;
		pCEFDOMNode = nullptr;
	}

	return fHandled;
}

/*
CefRefPtr<CefDownloadHandler> CEFHandler::GetDownloadHandler() {
	return this;
}
*/

void PlatformTitleChange(CefRefPtr<CefBrowser> pCEFBrowser, const CefString& strTitle) {
	CefWindowHandle hwnd = pCEFBrowser->GetHost()->GetWindowHandle();
	SetWindowText(hwnd, std::wstring(strTitle).c_str());
}

void CEFHandler::OnTitleChange(CefRefPtr<CefBrowser> pCEFBrowser, const CefString& strTitle) {
	DEBUG_LINEOUT("CEFHANDLE: OnTitleChange %S", std::wstring(strTitle).c_str());

	CEF_REQUIRE_UI_THREAD();

	PlatformTitleChange(pCEFBrowser, strTitle);
}

void CEFHandler::OnAfterCreated(CefRefPtr<CefBrowser> pCEFBrowser) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEFHANDLE: OnAfterCreated");

	CEF_REQUIRE_UI_THREAD();
	
	m_cefBrowsers.push_back(pCEFBrowser);
	m_fBrowserRunning = true;

	int browserID = pCEFBrowser->GetIdentifier();

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = std::make_shared<CEFBrowserController>(pCEFBrowser);
	CN(pCEFBrowserController);

	if (m_pCEFHandlerObserver != nullptr) {
		CR(m_pCEFHandlerObserver->OnBrowserCreated(pCEFBrowserController));
	}

Error:
	return;
}

bool CEFHandler::DoClose(CefRefPtr<CefBrowser> pCEFBrowser) {
	DEBUG_LINEOUT("CEFHANDLE: DoClose");
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed description of this
	// process.
	if (m_cefBrowsers.size() > 1) {
		// Set a flag to indicate that the window close should be allowed.
		m_fShuttingdown = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void CEFHandler::OnBeforeClose(CefRefPtr<CefBrowser> pCEFBrowser) {
	DEBUG_LINEOUT("CEFHANDLE: OnBeforeClose");

	CEF_REQUIRE_UI_THREAD();

	///*
	for (auto it = m_cefBrowsers.begin(); it != m_cefBrowsers.end(); it++) {
		if ((*it)->IsSame(pCEFBrowser)) {
			m_cefBrowsers.erase(it);
			break;
		}
	}
	//*/

	// All browser windows have closed. Quit the application message loop.
	if (m_cefBrowsers.empty()) {
		m_fBrowserRunning = false;
		//CefQuitMessageLoop();
	}
}

bool CEFHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access) {

	// false to allow pop up, true to cancel creation
	CefRefPtr<CefRequest> pCEFRequest = CefRequest::Create();
	pCEFRequest->SetURL(target_url);
	pCEFRequest->SetMethod(L"GET");
	browser->GetFocusedFrame()->LoadRequest(pCEFRequest);

	return true;
}

void CEFHandler::OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, ErrorCode errorCode,
	const CefString& strError, const CefString& strFailedURL) 
{
	DEBUG_LINEOUT("CEFHANDLE: OnLoadError %S url: %S", strError.c_str(), strFailedURL.c_str());

	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(strFailedURL) <<
		" with error " << std::string(strError) << " (" << errorCode <<
		").</h2></body></html>";

	pCEFFrame->LoadString(ss.str(), strFailedURL);
}

void CEFHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: OnLoadEnd");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnLoadingStateChanged(browser, isLoading, canGoBack, canGoForward));

Error:
	return;
}

void CEFHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType transition_type) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: OnLoadStart");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnLoadStart(browser, frame, transition_type));

Error:
	return;
}

void CEFHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: OnPaint");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnLoadEnd(browser, frame, httpStatusCode));

Error:
	return;
}

void CEFHandler::CloseAllBrowsers(bool fForceClose) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEFHANDLE: CloseAllBrowsers");

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CEFHandler::CloseAllBrowsers, this, fForceClose));
		return;
	}

	if (m_cefBrowsers.empty()) {
		return;
	}

	// Make a copy - threading issue
	std::list<CefRefPtr<CefBrowser>> tempCEFBrowsers = std::list<CefRefPtr<CefBrowser>>(m_cefBrowsers);

	for (auto &pCEFBrowser : tempCEFBrowsers) {
		if (pCEFBrowser != nullptr) {
			pCEFBrowser->GetHost()->CloseBrowser(fForceClose);
		}
	}

//Error:
	return;
}

bool CEFHandler::GetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: GetViewRect");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnGetViewRect(pCEFBrowser, cefRect));

Error:
	//return (r >= 0);
	return true;
}

void CEFHandler::OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, PaintElementType type, const RectList &dirtyRects, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;
	//DEBUG_LINEOUT("CEFHANDLE: OnPaint");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnPaint(pCEFBrowser, type, dirtyRects, pBuffer, width, height));

Error:
	return;
}

#ifdef CEF_AUDIO_MIRROR
// Audio Handler
void CEFHandler::OnAudioData(CefRefPtr<CefBrowser> browser, int frames, int channels, int bits_per_sample, const void* data_buffer) {
	RESULT r = R_PASS;

	//DEBUG_LINEOUT("CEFHandle: OnAudioData");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnAudioData(browser, frames, channels, bits_per_sample, data_buffer));

Error:
	return;
}
#endif

RESULT CEFHandler::GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL) {
	RESULT r = R_PASS;

	CR(m_pCEFHandlerObserver->GetResourceHandlerType(resourceHandlerType, pCefBrowser, strCEFURL));

Error:
	return r;
}

CefRequestHandler::ReturnValue CEFHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) {
	
	CefString strRequestURL = request->GetURL();
	CefRequest::HeaderMap requestHeaders;
	request->GetHeaderMap(requestHeaders);
	bool fUsesAuthentication = false;

	// Check for an authorization token
	for (std::multimap<CefString, CefString>::iterator it = requestHeaders.begin(); it != requestHeaders.end(); ++it) {
		if (it->first == "Authorization") {
			fUsesAuthentication = true;
		}
	}

	if (fUsesAuthentication) {	// If the link uses an authorization token we need to save it
		std::map<CefString, std::multimap<CefString, CefString>>::iterator it;
		it = m_savedRequestHeaders.find(strRequestURL);

		if (it != m_savedRequestHeaders.end()) {	// If the link already exists, update the headers
			it->second = requestHeaders;
		}

		else if (it == m_savedRequestHeaders.end()) {	// Otherwise insert it
			m_savedRequestHeaders.insert(std::pair<CefString, std::multimap<CefString, CefString>>(strRequestURL, requestHeaders));
		}
	}

	else {	// If the link doesn't have an auth header we need to check if it's one of the saved links
		std::map<CefString, std::multimap<CefString, CefString>>::iterator it;
		it = m_savedRequestHeaders.find(strRequestURL);

		if (it != m_savedRequestHeaders.end()) {	// If it is, insert the headers
			requestHeaders = it->second;
		}
	}

	request->SetHeaderMap(requestHeaders);
	
	return RV_CONTINUE;
}

bool CEFHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response) {
	
	/*
	// This is a gut check to see that the right headers are in the right place
	// TODO: comment out if not in testing
	CefRequest::HeaderMap cefHeaders;
	response->GetHeaderMap(cefHeaders);

	CefResponse::HeaderMap::iterator headeritem;
	DEBUG_LINEOUT("OnResourceResponse::Response headers size = %i", (int)cefHeaders.size());

	int i = 0;
	for (headeritem = cefHeaders.begin(); headeritem != cefHeaders.end(); headeritem++) {
		DEBUG_LINEOUT("[%i]: ['%s','%s']", i++, headeritem->first.ToString().c_str(), headeritem->second.ToString().c_str());
	}
	//*/

	return false;
}

CefRefPtr<CefResourceHandler> CEFHandler::GetResourceHandler(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefFrame> pCefFrame, CefRefPtr<CefRequest> pCefRequest) {
	RESULT r = R_PASS;
	
	// Uncomment to skip all custom resource handling code
	//return nullptr;

	ResourceHandlerType resourceHandlerType;
	CefString strCEFURL = pCefRequest->GetURL();
	GetResourceHandlerType(resourceHandlerType, pCefBrowser, strCEFURL);	

	switch (resourceHandlerType) {
	case (ResourceHandlerType::DREAM): {
		CefRefPtr<CefResourceHandler> pCefResourceHandler = CefRefPtr<CefResourceHandler>(new CEFResourceHandler(pCefBrowser, pCefFrame, pCefRequest));
		CN(pCefResourceHandler);

		return pCefResourceHandler;
	} break;

	case (ResourceHandlerType::DEFAULT): {
		return nullptr;
	} break;
	}

Error:
	return nullptr;
}

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
			auto pCEFProcessMessageArguments = pCEFProcessMessage->GetArgumentList();
			if (strMethodName == "OnFocusedNodeChanged") {
				
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
			else if (strMethodName == "DreamExtension") {
				CR(m_pCEFHandlerObserver->HandleDreamExtensionCall(pCefBrowser, pCEFProcessMessageArguments));

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

bool CEFHandler::OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access) {
	
	// false to allow pop up, true to cancel creation
	CefRefPtr<CefRequest> pCEFRequest = CefRequest::Create();

	pCEFRequest->SetURL(target_url);
	pCEFRequest->SetMethod(L"GET");
	frame->LoadRequest(pCEFRequest);

	return true;
}

void CEFHandler::OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, ErrorCode errorCode,
	const CefString& strError, const CefString& strFailedURL) 
{
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: OnLoadError %S url: %S", strError.c_str(), strFailedURL.c_str());

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnLoadError(pCEFBrowser, pCEFFrame, errorCode, strError, strFailedURL));

Error:
	return;
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

RESULT CEFHandler::HandleDreamExtensionCall(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefListValue> pMessageArguments) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: DreamExtension");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->HandleDreamExtensionCall(pCefBrowser, pMessageArguments));

Error:
	return r;
	
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

void CEFHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) {
	RESULT r = R_PASS;
	//DEBUG_LINEOUT("CEFHANDLE: OnPaint");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnPopupSize(browser, rect));

Error:
	return;
}

#ifdef CEF_AUDIO_MIRROR
// Audio Handler
void CEFHandler::OnAudioData(CefRefPtr<CefBrowser> browser, int audio_stream_id, int frames, int channels, int bits_per_sample, const void* data_buffer) {
	RESULT r = R_PASS;

	//DEBUG_LINEOUT("CEFHandle: OnAudioData");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnAudioData(browser, audio_stream_id, frames, channels, bits_per_sample, data_buffer));

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

RESULT CEFHandler::CheckForHeaders(std::multimap<std::string, std::string> &headermap, CefRefPtr<CefBrowser> pCefBrowser, std::string strURL) {
	return m_pCEFHandlerObserver->CheckForHeaders(headermap, pCefBrowser, strURL);
}

CefRequestHandler::ReturnValue CEFHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) {
	
	CefString cefstrRequestURL = request->GetURL();
	CefRequest::HeaderMap requestHeaders;
	request->GetHeaderMap(requestHeaders);
	std::multimap<std::string, std::string> checkForRequestHeaders;
	std::string strURL = cefstrRequestURL;
	
	// replacing with full headers list from server instead
	/*
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
		it = m_savedRequestHeaders.find(cefstrRequestURL);

		if (it != m_savedRequestHeaders.end()) {	// If the link already exists, update the headers
			it->second = requestHeaders;
		}

		else if (it == m_savedRequestHeaders.end()) {	// Otherwise insert it
			m_savedRequestHeaders.insert(std::pair<CefString, std::multimap<CefString, CefString>>(cefstrRequestURL, requestHeaders));
		}
	}

	else {	// If the link doesn't have an auth header we need to check if it's one of the saved links
		std::map<CefString, std::multimap<CefString, CefString>>::iterator it;
		it = m_savedRequestHeaders.find(cefstrRequestURL);

		if (it != m_savedRequestHeaders.end()) {	// If it is, insert the headers
			requestHeaders = it->second;
		}
	}*/
	
	//*
	CheckForHeaders(checkForRequestHeaders, browser, strURL);

	if (!checkForRequestHeaders.empty()) {
		for (std::multimap<std::string, std::string>::iterator itr = checkForRequestHeaders.begin(); itr != checkForRequestHeaders.end(); ++itr) {
			std::string strKey = itr->first;

			CefString cefstrKey = util::StringToWideString(strKey);
			std::string strValue = itr->second;
			CefString cefstrValue = util::StringToWideString(strValue);

			requestHeaders.insert(std::pair<std::wstring, std::wstring>(cefstrKey, cefstrValue));
		}
		request->SetHeaderMap(requestHeaders);
	}
	//*/

	return RV_CONTINUE;
}

bool CEFHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response) {
	
	/* TESTING:
	// This is a gut check to see that the right headers are in the right place
	CefRequest::HeaderMap cefHeaders;
	response->GetHeaderMap(cefHeaders);
	CefRequest::HeaderMap cefReqHeaders;
	request->GetHeaderMap(cefReqHeaders);

	CefResponse::HeaderMap::iterator headeritem;
	DOSLOG(INFO, "OnResourceResponse::Response headers size = %i", (int)cefHeaders.size());

	int i = 0;
	for (headeritem = cefHeaders.begin(); headeritem != cefHeaders.end(); headeritem++) {
		DOSLOG(INFO, "[%i]: ['%s','%s']", i++, headeritem->first.ToString().c_str(), headeritem->second.ToString().c_str());
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

bool CEFHandler::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, CefRequestHandler::WindowOpenDisposition target_disposition, bool user_gesture) {
	CefRefPtr<CefRequest> pCEFRequest = CefRequest::Create();

	pCEFRequest->SetURL(target_url);
	pCEFRequest->SetMethod(L"GET");
	frame->LoadRequest(pCEFRequest);

	return true;
}

bool CEFHandler::OnCertificateError(CefRefPtr<CefBrowser> browser, cef_errorcode_t cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) {

//	callback->Continue(true);

	return m_pCEFHandlerObserver->OnCertificateError(browser, cert_error, request_url, ssl_info, callback);
}

void CEFHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution) {
	m_pCEFHandlerObserver->OnCertificateError(browser, (cef_errorcode_t)(0), url, nullptr, nullptr);
}

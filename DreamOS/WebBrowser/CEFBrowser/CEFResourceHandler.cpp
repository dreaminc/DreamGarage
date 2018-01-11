#include "CEFResourceHandler.h"


CEFResourceHandler::CEFResourceHandler(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefFrame> pCefFrame, CefRefPtr<CefRequest> pCefRequest) :
	m_pCEFBrowser(pCefBrowser),
	m_pCEFFrame(pCefFrame),
	m_pCEFRequest(pCefRequest)
{
	// empty
}

CEFResourceHandler::~CEFResourceHandler() {
	// empty
}

// Begin processing the request. To handle the request return true and call
// CefCallback::Continue() once the response header information is available
// (CefCallback::Continue() can also be called from inside this method if
// header information is available immediately). To cancel the request return
// false.
///
/*--cef()--*/
bool CEFResourceHandler::ProcessRequest(CefRefPtr<CefRequest> pCefRequest, CefRefPtr<CefCallback> pCefCallback) {
	RESULT r = R_PASS;

	/*
	// Here for testing purposes, insert a header
	CefRequest::HeaderMap requestHeaders;
	pCefRequest->GetHeaderMap(requestHeaders);

	CefString cstrKey = "testKey";
	CefString cstrValue = "testValue";
	requestHeaders.insert(std::multimap<CefString, CefString>::value_type(cstrKey, cstrValue));

	pCefRequest->SetHeaderMap(requestHeaders);
	
	// Here for testing purposes (this will output the request headers)
	CefRequest::HeaderMap cefHeaders;
	pCefRequest->GetHeaderMap(cefHeaders);

	CefResponse::HeaderMap::iterator headeritem;
	DEBUG_LINEOUT("Request headers size = %i", (int)cefHeaders.size());

	int i = 0;
	for (headeritem = cefHeaders.begin(); headeritem != cefHeaders.end(); headeritem++) {
		DEBUG_LINEOUT("[%i]: ['%s','%s']", i++, headeritem->first.ToString().c_str(), headeritem->second.ToString().c_str());
	}
	//*/

	// Currently using bland settings
	// Set here if needed (maybe like cache path)
	CefRequestContextSettings cefRequestContextSettings;
	cefRequestContextSettings.size = sizeof(cefRequestContextSettings);


	// Save callback for later
	m_pCefProcessRequestCallback = pCefCallback;

	// Set up the context
	auto pCefRequestContext = CefRequestContext::CreateContext(cefRequestContextSettings, nullptr);
	CN(pCefRequestContext);

	// 
	//pCefRequest->SetFlags(UR_FLAG_ALLOW_CACHED_CREDENTIALS);

	//DEBUG_LINEOUT("%S request: %S", pCefRequest->GetMethod().c_str(), pCefRequest->GetURL().c_str());

	// Create the URL request here
	//m_pCefURLRequest = CefURLRequest::Create(pCefRequest, this, pCefRequestContext);
	m_pCefURLRequest = CefURLRequest::Create(pCefRequest, this, nullptr);
	CN(m_pCefURLRequest);

	return true;

Error:
	return false;
}

///
// Retrieve response header information. If the response length is not known
// set |response_length| to -1 and ReadResponse() will be called until it
// returns false. If the response length is known set |response_length|
// to a positive value and ReadResponse() will be called until it returns
// false or the specified number of bytes have been read. Use the |response|
// object to set the mime type, http status code and other optional header
// values. To redirect the request to a new URL set |redirectUrl| to the new
// URL. If an error occurred while setting up the request you can call
// SetError() on |response| to indicate the error condition.
///
/*--cef()--*/
void CEFResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> pCefResponse, int64& responseLength, CefString& strRedirectURL) {
	
	CefRefPtr<CefResponse> pLocalResponse = m_pCefURLRequest->GetResponse();

	CefResponse::HeaderMap cefHeaders;
	pLocalResponse->GetHeaderMap(cefHeaders);

	/*
	bool fResponseReadOnly = m_pCefURLRequest->GetResponse()->IsReadOnly();

	CefResponse::HeaderMap::iterator headeritem;
	DEBUG_LINEOUT("Response headers size = %i readonly:%i", (int)cefHeaders.size(), fResponseReadOnly);

	int i = 0;
	for (headeritem = cefHeaders.begin(); headeritem != cefHeaders.end(); headeritem++) {
		DEBUG_LINEOUT("[%i]: ['%s','%s']", i++, headeritem->first.ToString().c_str(), headeritem->second.ToString().c_str());
	}
	//*/

	// Copy the headers over (this is where we'd manipulate them)
	// TODO: Adjust content-disposition as needed
	std::string strContentDisposition = pLocalResponse->GetHeader("content-disposition");
	for (CefResponse::HeaderMap::iterator itr = cefHeaders.begin(); itr != cefHeaders.end(); ++itr) {
		CefString strKey = itr->first;
		if (strKey == "content-disposition") {
			if (strContentDisposition.substr(0, 10) == "attachment") {		// for now, we always want content-disposition to be inline
				strContentDisposition.replace(0, 10, "inline");
				itr->second = strContentDisposition;
			}
		}
		if (strKey == "content-security-policy") {
			itr->second = "";	// using x-xss-protection instead of a csp for now - sandboxing breaks pdfs from dropbox
			cefHeaders.insert(std::multimap<CefString, CefString>::value_type("x-xss-protection", "1; mode=block"));
		}
	}

	pCefResponse->SetHeaderMap(cefHeaders);

	// Copy over response settings 
	
	// MIME Type
	auto strMimeType = pLocalResponse->GetMimeType();
	if(!strMimeType.empty())
		pCefResponse->SetMimeType(strMimeType);

	// Status
	auto responseStatus = pLocalResponse->GetStatus();
	pCefResponse->SetStatus(responseStatus);

	auto strResponseStatusText = pLocalResponse->GetStatusText();
	if(!strResponseStatusText.empty())
		pCefResponse->SetStatusText(strResponseStatusText);

	// Error Code
	auto responseError = pLocalResponse->GetError();
	pCefResponse->SetError(responseError);

	// Response Length (used by ReadResponse)
	responseLength = m_rxDownloadBuffer.size();
}

///
// Read response data. If data is available immediately copy up to
// |bytes_to_read| bytes into |data_out|, set |bytes_read| to the number of
// bytes copied, and return true. To read the data at a later time set
// |bytes_read| to 0, return true and call CefCallback::Continue() when the
// data is available. To indicate response completion return false.
///
/*--cef()--*/
bool CEFResourceHandler::ReadResponse(void* pDataOutBuffer, int bytesToRead, int& bytesRead, CefRefPtr<CefCallback> pCefCallback) {
	RESULT r = R_PASS;

	bytesRead = 0;
	uint8_t *pTypedDataBuffer = (uint8_t*)pDataOutBuffer;

	CN(pTypedDataBuffer);
	CB(m_rxDownloadBuffer.size() >= bytesToRead);

	memcpy(pDataOutBuffer, m_rxDownloadBuffer.data(), (size_t)(bytesToRead));
	bytesRead = bytesToRead;

	m_rxDownloadBuffer.erase(m_rxDownloadBuffer.begin(), m_rxDownloadBuffer.begin() + bytesRead);

	// This should be a one shot
	// if this returns less than bytesToRead there was an error
	CB((bytesRead > 0));

	return true;

Error:
	return false;
}

///
// Request processing has been canceled.
///
/*--cef()--*/
void CEFResourceHandler::Cancel() {
	// empty
}

void CEFResourceHandler::OnRequestComplete(CefRefPtr<CefURLRequest> pCefRequest) {
	// The request is done
	m_fRequestReady = true;

	if (m_pCefProcessRequestCallback != nullptr) {
		m_pCefProcessRequestCallback->Continue();
	}
}
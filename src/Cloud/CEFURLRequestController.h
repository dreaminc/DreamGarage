#ifndef CEF_URL_REQUEST_CONTROLLER_H_
#define CEF_URL_REQUEST_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CEFApp.h
// The CEF App

#include "include\cef_app.h"

#include "include\cef_browser.h"
#include "include\cef_command_line.h"
#include "include\wrapper/cef_helpers.h"

#include "include\cef_urlrequest.h"

#include <list>
#include <string>

class CEFURLRequestClient : public CefURLRequestClient {
public:
	CEFURLRequestClient(std::wstring& strURL) :
		m_UploadTotal(0),
		m_DownloadTotal(0),
		m_strURL(strURL)
	{
		m_pCefRequest = CefRequest::Create();
		CefString cefstrURL(m_strURL);

		// Set URL and Method
		m_pCefRequest->SetURL(cefstrURL);
		m_pCefRequest->SetMethod("GET");
		m_pCefRequest->SetFlags(UR_FLAG_SKIP_CACHE);
	}

	~CEFURLRequestClient() {
		// empty
	}

	void OnRequestComplete(CefRefPtr<CefURLRequest> request) OVERRIDE {
		CefURLRequest::Status status = request->GetRequestStatus();
		CefURLRequest::ErrorCode error = request->GetRequestError();
		CefRefPtr<CefResponse> response = request->GetResponse();

		// Do something with the response...
		// TODO: 

		DEBUG_LINEOUT("CEF OnRequestComplete for %S", m_strURL.c_str());
	}

	void OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) OVERRIDE {
		m_UploadTotal = total;
	}

	void OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) OVERRIDE {
		m_DownloadTotal = total;
	}

	void OnDownloadData(CefRefPtr<CefURLRequest> request, const void* data, size_t data_length) {
		m_strDownloadData += std::string(static_cast<const char *>(data), data_length);
	}

	bool GetAuthCredentials(bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) OVERRIDE {
		DEBUG_LINEOUT("GetAuthCredentials Not Implemented!");
		return false;
	}

	RESULT Start() {
		RESULT r = R_PASS;

		DEBUG_LINEOUT("CEF request %S on ui thread %d", m_strURL.c_str(), CefCurrentlyOn(TID_UI));

		m_pCefUrlRequest = CefURLRequest::Create(m_pCefRequest, this, nullptr);
		CNM(m_pCefUrlRequest, "CEF URL Request failed to create");

	Error:
		return r;
	}

private:
	std::wstring m_strURL;
	CefRefPtr<CefRequest> m_pCefRequest;
	CefRefPtr<CefURLRequest> m_pCefUrlRequest;

	uint64 m_UploadTotal;
	uint64 m_DownloadTotal;
	std::string m_strDownloadData;

private:
	IMPLEMENT_REFCOUNTING(CEFURLRequestClient);
};

class CEFURLRequestController {
public:
	CEFURLRequestController() {
		// empty
	}

	~CEFURLRequestController() {
		// empty
		// TODO: Release the request
	}

	RESULT CreateNewURLRequest(std::wstring& strURL) {
		RESULT r = R_PASS;

		CefRefPtr<CEFURLRequestClient> pNewCEFURLRequestClient = new CEFURLRequestClient(strURL);
		CRM(pNewCEFURLRequestClient->Start(), "Failed to start CEF URL Request");

		m_CEFURLRequestClients.push_front(std::move(pNewCEFURLRequestClient));

	Error:
		return r;
	}

private:
	std::list<CefRefPtr<CEFURLRequestClient>> m_CEFURLRequestClients;
};



#endif // ! CEF_URL_REQUEST_CONTROLLER_H_
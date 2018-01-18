#ifndef CEF_RESOURCE_HANDLER_H_
#define CEF_RESOURCE_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFResourceHandler.h

#include "include\cef_resource_handler.h"
#include "include\cef_urlrequest.h"

class CEFResourceHandler : 
	public CefResourceHandler,
	public CefURLRequestClient 
{

public:
	CEFResourceHandler(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefFrame> pCefFrame, CefRefPtr<CefRequest> pCefRequest);
	~CEFResourceHandler();

	// CefResourceHandler

	// Begin processing the request. To handle the request return true and call
	// CefCallback::Continue() once the response header information is available
	// (CefCallback::Continue() can also be called from inside this method if
	// header information is available immediately). To cancel the request return
	// false.
	///
	/*--cef()--*/
	virtual bool ProcessRequest(CefRefPtr<CefRequest> pCefRequest, CefRefPtr<CefCallback> pCefCallback) override;

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
	virtual void GetResponseHeaders(CefRefPtr<CefResponse> pCefResponse, int64& response_length, CefString& strRedirectURL) override;

	///
	// Read response data. If data is available immediately copy up to
	// |bytes_to_read| bytes into |data_out|, set |bytes_read| to the number of
	// bytes copied, and return true. To read the data at a later time set
	// |bytes_read| to 0, return true and call CefCallback::Continue() when the
	// data is available. To indicate response completion return false.
	///
	/*--cef()--*/
	virtual bool ReadResponse(void* pDataOutBuffer, int bytesToRead, int& bytesRead, CefRefPtr<CefCallback> pCefCallback) override;

	///
	// Request processing has been canceled.
	///
	/*--cef()--*/
	virtual void Cancel() override;


	///*
	// CefURLRequestClient 

	///
	// Notifies the client that the request has completed. Use the
	// CefURLRequest::GetRequestStatus method to determine if the request was
	// successful or not.
	///
	virtual void OnRequestComplete(CefRefPtr<CefURLRequest> pCefRequest) override;

	///
	// Notifies the client of upload progress. |current| denotes the number of
	// bytes sent so far and |total| is the total size of uploading data (or -1 if
	// chunked upload is enabled). This method will only be called if the
	// UR_FLAG_REPORT_UPLOAD_PROGRESS flag is set on the request.
	///
	virtual void OnUploadProgress(CefRefPtr<CefURLRequest> pCefRequest, int64 current, int64 total) override {
		// empty
	}

	///
	// Notifies the client of download progress. |current| denotes the number of
	// bytes received up to the call and |total| is the expected total size of the
	// response (or -1 if not determined).
	///
	virtual void OnDownloadProgress(CefRefPtr<CefURLRequest> pCefRequest, int64 current, int64 total) override {
		// empty
	}

	///
	// Called when some part of the response is read. |data| contains the current
	// bytes received since the last call. This method will not be called if the
	// UR_FLAG_NO_DOWNLOAD_DATA flag is set on the request.
	///
	virtual void OnDownloadData(CefRefPtr<CefURLRequest> pCefRequest, const void* pDataBuffer, size_t dataLength) override {
		uint8_t *pTypedDataBuffer = (uint8_t*)pDataBuffer;

		if (pTypedDataBuffer != nullptr) {

			for (size_t i = 0; i < dataLength; i++) {
				m_rxDownloadBuffer.push_back(pTypedDataBuffer[i]);
			}
		}
	}

	///
	// Called on the IO thread when the browser needs credentials from the user.
	// |isProxy| indicates whether the host is a proxy server. |host| contains the
	// hostname and |port| contains the port number. Return true to continue the
	// request and call CefAuthCallback::Continue() when the authentication
	// information is available. Return false to cancel the request. This method
	// will only be called for requests initiated from the browser process.
	///
	virtual bool GetAuthCredentials(
		bool fProxy,
		const CefString& strHost,
		int port,
		const CefString& strRealm,
		const CefString& strScheme,
		CefRefPtr<CefAuthCallback> pCefAuthCallback) override 
	{
		DEBUG_LINEOUT("Credentials required - currently this is non-supported");
		return false;
	}
	//*/

private:
	CefRefPtr<CefBrowser> m_pCEFBrowser = nullptr;
	CefRefPtr<CefFrame> m_pCEFFrame = nullptr;
	CefRefPtr<CefRequest> m_pCEFRequest = nullptr;
	CefRefPtr<CefCallback> m_pCefProcessRequestCallback = nullptr;

	CefRefPtr<CefURLRequest> m_pCefURLRequest = nullptr;
	bool m_fRequestReady = false;
	
	std::vector<uint8_t> m_rxDownloadBuffer;

	IMPLEMENT_REFCOUNTING(CEFResourceHandler);
	//IMPLEMENT_LOCKING(CEFResourceHandler);
};

#endif //CEF_RESOURCE_HANDLER_H_
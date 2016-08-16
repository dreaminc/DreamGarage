#include "HTTPController.h"

#include <curl/curl.h>
#include <iostream>
#include <future>

#ifdef _WIN32
	#define SHORT_SLEEP Sleep(100)
#else
	#define SHORT_SLEEP usleep(100000)
#endif

// Static initialization of Singleton
HTTPController* HTTPController::s_pInstance = nullptr;

HTTPController::HTTPController() :
	m_fRunning(false)
{
	Start();
}

HTTPController::~HTTPController() {
	Stop();
}

void HTTPController::ProcessingThread() {
	m_fRunning = true;

	timeval timeout;

	while (m_fRunning) {
		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		int maxfd = -1;

		long timeoutCURL;

		curl_multi_timeout(m_pCURLMultiHandle, &timeoutCURL);

		if (timeoutCURL < 0)
			timeoutCURL = 1000;

		timeout.tv_sec = timeoutCURL / 1000;
		timeout.tv_usec = (timeoutCURL % 1000) * 1000;

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		// Get file descriptors from the transfers
		// TODO: Add error handling here?
		CURLMcode mc = curl_multi_fdset(m_pCURLMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);

		int rc = 0;

		if (maxfd == -1) {
			SHORT_SLEEP;
			rc = 0;
		}
		else {
			rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
		}

		switch (rc) {
			case -1: {
				// select error => continue
			} break;
			case 0:
			default: {
				// timeout or readable/writable sockets
				Update();
			} break;
		}
	}

	DEBUG_LINEOUT("HTTP Thread Exit");

}

RESULT HTTPController::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Http::Start");

	m_pCURLMultiHandle = curl_multi_init();
	CNM(m_pCURLMultiHandle, "Failed to initialzie CURL multi handle");

	m_CURLMultiHandleCount = 0;
	m_thread = std::thread(&HTTPController::ProcessingThread, this);

Error:
	return r;
}

RESULT HTTPController::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Http::Stop");

	m_fRunning = false;
	m_thread.join();

	// TODO: Test 
	CURLMcode cmc = curl_multi_cleanup(m_pCURLMultiHandle);
	CBM((cmc == CURLM_OK), "CURL failed to clean up multi handle");

	curl_global_cleanup();
	
Error:
	return r;
}

void HTTPController::Update() {
	curl_multi_perform(m_pCURLMultiHandle, &m_CURLMultiHandleCount);
}

RESULT HTTPController::Request(std::function<HTTPRequestHandler*(CURL*)> request) {
	RESULT r = R_PASS;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	HTTPRequestHandler*	cb = request(pCURL);

	curl_easy_setopt(pCURL, CURLOPT_URL, cb->m_pHTTPRequest->m_strURI.c_str());
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, cb);

	curl_multi_add_handle(m_pCURLMultiHandle, pCURL);

Error:
	return r;
}

RESULT HTTPController::AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse* pHTTPResponse) {
	return Request([&](CURL* pCURL) -> HTTPRequestHandler*
	{
		HTTPRequestHandler* cb = new HTTPRequestHandler(
			new HTTPRequest(pCURL, strURI, strHeaders),
			(pHTTPResponse) ? pHTTPResponse : &m_defaultResponse,
			nullptr);

		curl_easy_setopt(pCURL, CURLOPT_URL, cb->m_pHTTPRequest->m_strURI.c_str());
		curl_easy_setopt(pCURL, CURLOPT_VERBOSE, 1L);

		struct curl_slist *pCURLList = NULL;

		for (const auto& strHeader : cb->m_pHTTPRequest->m_strHeaders) {
			pCURLList = curl_slist_append(pCURLList, strHeader.c_str());
		}

		curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);

		return cb;
	});
}

RESULT HTTPController::AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnHTTPResponseCallback) {
	return Request([&](CURL* pCURL) -> HTTPRequestHandler*
	{
		HTTPRequestHandler* cb = new HTTPRequestHandler(
			new HTTPRequest(pCURL, strURI, strHeaders),
			nullptr,
			fnHTTPResponseCallback);

		curl_easy_setopt(pCURL, CURLOPT_URL, cb->m_pHTTPRequest->m_strURI.c_str());
		struct curl_slist *pCURLList = NULL;

		for (const auto& strHeader : cb->m_pHTTPRequest->m_strHeaders) {
			pCURLList = curl_slist_append(pCURLList, strHeader.c_str());
		}

		curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);

		return cb;
	});
}

RESULT HTTPController::GET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse& httpResponse) {
	RESULT r = R_PASS;

	std::promise<std::string> httpPromise;
	std::future<std::string> httpFuture = httpPromise.get_future();

	CR(AGET(strURI, strHeaders, [&](std::string&& in) {httpPromise.set_value(in); }));

	{
		// Future Timeout
		std::chrono::system_clock::time_point httpTimeout = std::chrono::system_clock::now() + std::chrono::seconds(HTTP_DELAY_SECONDS);
		std::future_status statusFuture = httpFuture.wait_until(httpTimeout);

		CBM((statusFuture == std::future_status::ready), "POST future timed out");
	}

	httpResponse.PutResponse(httpFuture.get());

Error:
	return r;
}

RESULT HTTPController::APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse* pHTTPResponse) {
	return Request([&](CURL* pCURL) -> HTTPRequestHandler*
	{
		HTTPRequestHandler*	cb = new HTTPRequestHandler(
			new HTTPRequest(pCURL, strURI, strHeaders, strBody),
			(pHTTPResponse) ? pHTTPResponse : &m_defaultResponse,
			nullptr);


		curl_easy_setopt(pCURL, CURLOPT_URL, cb->m_pHTTPRequest->m_strURI.c_str());
		curl_easy_setopt(pCURL, CURLOPT_POST, 1L);

		struct curl_slist *pCURLList = NULL;

		for (const auto& strHeader : cb->m_pHTTPRequest->m_strHeaders) {
			pCURLList = curl_slist_append(pCURLList, strHeader.c_str());
		}

		curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
		curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, cb->m_pHTTPRequest->m_strBody.c_str());

		return cb;
	});
}

RESULT HTTPController::APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponseCallback fnHTTPResponseCallback) {
	return Request([&](CURL* pCURL) -> HTTPRequestHandler*
	{
		HTTPRequestHandler*	cb = new HTTPRequestHandler(
			new HTTPRequest(pCURL, strURI, strHeaders, strBody),
			nullptr,
			fnHTTPResponseCallback);

		curl_easy_setopt(pCURL, CURLOPT_URL, cb->m_pHTTPRequest->m_strURI.c_str());
		curl_easy_setopt(pCURL, CURLOPT_POST, 1L);

		struct curl_slist *pCURLList = NULL;

		for (const auto& strHeader : cb->m_pHTTPRequest->m_strHeaders) {
			pCURLList = curl_slist_append(pCURLList, strHeader.c_str());
		}

		curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
		curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, cb->m_pHTTPRequest->m_strBody.c_str());

		return cb;
	});
}

RESULT HTTPController::POST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse& httpResponse) {
	RESULT r = R_PASS;

	std::promise<std::string> httpPromise;
	std::future<std::string> httpFuture = httpPromise.get_future();

	CR(APOST(strURI, strHeaders, strBody, [&](std::string&& strFutureResponse) { httpPromise.set_value(strFutureResponse); }));
	{
		// Future Timeout
		std::chrono::system_clock::time_point httpTimeout = std::chrono::system_clock::now() + std::chrono::seconds(HTTP_DELAY_SECONDS);
		std::future_status statusFuture = httpFuture.wait_until(httpTimeout);
		
		CBM((statusFuture == std::future_status::ready), "POST future timed out");
	}

	httpResponse.PutResponse(httpFuture.get());

Error:
	return r;
}

size_t HTTPController::RequestCallback(void *pContext, size_t size, size_t nmemb, HTTPRequestHandler *pHTTPRequestHandler) {
	
	// callback error, should we log out a warning(?)
	if (!pContext) {
		DEBUG_LINEOUT("HTTP callback error");
		return 0;
	}

	if (pContext && pHTTPRequestHandler) {
		std::string strResponse(static_cast<char*>(pContext));

		if (pHTTPRequestHandler->m_pHTTPResponse) {
			// using HttpResponse
			pHTTPRequestHandler->m_pHTTPResponse->OnResponse(std::move(strResponse));
			delete pHTTPRequestHandler->m_pHTTPRequest;
			pHTTPRequestHandler->m_pHTTPRequest = nullptr;
			pHTTPRequestHandler->m_pHTTPResponse = nullptr;
		}
		else if (pHTTPRequestHandler->m_fnResponseCallback) {
			// using HttpResponseFunc
			pHTTPRequestHandler->m_fnResponseCallback(std::move(strResponse));
			delete pHTTPRequestHandler->m_pHTTPRequest;
			pHTTPRequestHandler->m_pHTTPRequest = nullptr;
			pHTTPRequestHandler->m_fnResponseCallback = nullptr;
		}

		// cleanup should be made after the callback(!)
		//curl_easy_cleanup(cb->request->curl);
		//CURL* c = cb->request->curl;
		//cb->response = &m_defaultResponse;
		//delete cb;
		//curl_easy_cleanup(c);
	}

	return (size * nmemb);
}

// Default response
void HTTPResponse::OnResponse(std::string&& strResponse) {
	DEBUG_LINEOUT("HTTP response: %s", strResponse.c_str());
}


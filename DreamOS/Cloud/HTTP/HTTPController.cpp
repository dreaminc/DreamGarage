#include "HTTPController.h"

#include <curl/curl.h>
#include <iostream>
#include <future>

#include "HTTPRequestHandler.h"
#include "HTTPRequestFileHandler.h"

#include "Cloud/CloudController.h"

// Static initialization of Singleton
HTTPController* HTTPController::s_pInstance = nullptr;

HTTPController* HTTPController::CreateHTTPController() {
	RESULT r = R_PASS;

	HTTPController *pHTTPController = nullptr;

	pHTTPController = new HTTPController();
	CNM(pHTTPController, "Failed to allocate http controller");

	CRM(pHTTPController->Initialize(), "Failed to initialize http controller");

// Success:
	return pHTTPController;

Error:
	if (pHTTPController != nullptr) {
		delete pHTTPController;
		pHTTPController = nullptr;
	}

	return nullptr;
}

HTTPController::HTTPController() :
	m_fRunning(false)
{
	// empty
}

HTTPController::~HTTPController() {
	Shutdown();
}

RESULT HTTPController::Initialize() {
	RESULT r = R_PASS;

	CRM(Start(), "Failed to start HTTP Controller");

Error:
	return r;
}

RESULT HTTPController::Shutdown() {
	RESULT r = R_PASS;

	CRM(Stop(), "Failed to shut down HTTP Controller");

Error:
	return r;
}

// Reference https://curl.haxx.se/libcurl/c/multi-post.html
void HTTPController::CURLMultihandleThreadProcess() {
	RESULT r = R_PASS;
	m_fRunning = true;

	timeval tvTimeout;
	CURLMcode curlMC;

	while (m_fRunning) {
		int maxFileDescriptors = -1;
		int rc = 0;
		long timeoutCURL;

		fd_set fileDescriptorsRead;
		fd_set fileDescriptorsWrite;
		fd_set fileDescriptorsExcep;

		curlMC = curl_multi_timeout(m_pCURLMultiHandle, &timeoutCURL);
		CBM((curlMC == CURLM_OK), "curl_multi_fdset failed");

		if (timeoutCURL < 0) {
			timeoutCURL = 1000;
		}

		tvTimeout.tv_sec = timeoutCURL / 1000;
		tvTimeout.tv_usec = (timeoutCURL % 1000) * 1000;

		FD_ZERO(&fileDescriptorsRead);
		FD_ZERO(&fileDescriptorsWrite);
		FD_ZERO(&fileDescriptorsExcep);

		// Get file descriptors from the transfers
		// TODO: Add error handling here?
		curlMC = curl_multi_fdset(m_pCURLMultiHandle, &fileDescriptorsRead, &fileDescriptorsWrite, &fileDescriptorsExcep, &maxFileDescriptors);
		CBM((curlMC == CURLM_OK), "curl_multi_fdset failed");

		if (maxFileDescriptors == -1) {
			#ifdef _WIN32
				Sleep(100);
			#else
				usleep(100000);
			#endif

			rc = 0;
		}
		else {
			rc = select(maxFileDescriptors + 1, &fileDescriptorsRead, &fileDescriptorsWrite, &fileDescriptorsExcep, &tvTimeout);
		}

		switch (rc) {
			case -1: {
				// select error => continue
			} break;

			case 0:
			default: {
				// Timeout or readable/writable sockets
				curlMC = curl_multi_perform(m_pCURLMultiHandle, &m_CURLMultiHandleCount);
			} break;
		}
	}

	DEBUG_LINEOUT("HTTP Thread Exit");

Error:
	if (m_pCURLMultiHandle != nullptr) {
		curl_multi_cleanup(m_pCURLMultiHandle);
		m_pCURLMultiHandle = nullptr;
	}

	return;
}

RESULT HTTPController::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Http::Start");

	m_pCURLMultiHandle = curl_multi_init();
	CNM(m_pCURLMultiHandle, "Failed to initialzie CURL multi handle");

	m_CURLMultiHandleCount = 0;
	m_thread = std::thread(&HTTPController::CURLMultihandleThreadProcess, this);

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

RESULT HTTPController::Request(std::function<HTTPRequestHandler*(CURL*)> fnHTTPRequestHandler) {
	RESULT r = R_PASS;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	HTTPRequestHandler*	pHTTPRequestHandler = fnHTTPRequestHandler(pCURL);

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->m_pHTTPRequest->m_strURI.c_str());
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestHandler);

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

// FILE DOWNLOAD
/*
RESULT HTTPController::AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::string &strDesPath, HTTPResponse* pHTTPResponse) {
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
*/

RESULT HTTPController::RequestFile(std::function<HTTPRequestFileHandler*(CURL*)> fnHTTPRequestFileCallback) {
	RESULT r = R_PASS;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	HTTPRequestFileHandler*	pHTTPRequestFileHandler = fnHTTPRequestFileCallback(pCURL);

	

	

Error:
	return r;
}

RESULT HTTPController::AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponseCallback fnResponseCallback) {
	RESULT r = R_PASS;

	CURLMcode curlMC = CURLM_OK;
	CURLcode curlC = CURLE_OK;

	HTTPRequestFileHandler*	pHTTPRequestFileHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestFileHandler = new HTTPRequestFileHandler(new HTTPRequest(pCURL, strURI, strHeaders, strBody),
														 nullptr,
														 fnResponseCallback);

	// Add the headers
	for (const auto& strHeader : pHTTPRequestFileHandler->m_pHTTPRequest->m_strHeaders) {
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());
	}

	// CURL
	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestFileHandler->m_pHTTPRequest->m_strURI.c_str());
	curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestFileCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestFileHandler);

	// Add multi handle (will get picked up in thread)
	curlMC = curl_multi_add_handle(m_pCURLMultiHandle, pCURL);
	CB((curlMC == CURLM_OK));

Error:
	return r;
}

RESULT HTTPController::FILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponse& httpResponse) {
	RESULT r = R_PASS;

	std::promise<std::string> httpPromise;
	std::future<std::string> httpFuture = httpPromise.get_future();

	CR(AFILE(strURI, strHeaders, strBody, strDestinationPath,
		[&](std::string&& strFutureResponse) { 
			httpPromise.set_value(strFutureResponse); 
		}
	));

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


size_t HTTPController::RequestFileCallback(char *pBuffer, size_t pBuffer_n, size_t numBuffers, void *pContext) {
	RESULT r = R_PASS;

	HTTPRequestFileHandler *pHTTPRequestFileHandler = reinterpret_cast<HTTPRequestFileHandler*>(pContext);
	CN(pHTTPRequestFileHandler);

	// callback error, should we log out a warning(?)
	if (!pContext) {
		DEBUG_LINEOUT("HTTP callback error");
		return 0;
	}

	if (pContext && pHTTPRequestFileHandler) {
		std::string strResponse(static_cast<char*>(pContext));

		if (pHTTPRequestFileHandler->m_pHTTPResponse) {
			// Using HttpResponse
			pHTTPRequestFileHandler->m_pHTTPResponse->OnResponse(std::move(strResponse));
			
			pHTTPRequestFileHandler->m_pHTTPRequest = nullptr;
			pHTTPRequestFileHandler->m_pHTTPResponse = nullptr;

			delete pHTTPRequestFileHandler->m_pHTTPRequest;

		}
		else if (pHTTPRequestFileHandler->m_fnResponseCallback) {
			// Using HttpResponseFunc
			pHTTPRequestFileHandler->m_fnResponseCallback(std::move(strResponse));
			
			pHTTPRequestFileHandler->m_pHTTPRequest = nullptr;
			pHTTPRequestFileHandler->m_fnResponseCallback = nullptr;

			delete pHTTPRequestFileHandler->m_pHTTPRequest;
		}

		// cleanup should be made after the callback(!)
		//curl_easy_cleanup(cb->request->curl);
		//CURL* c = cb->request->curl;
		//cb->response = &m_defaultResponse;
		//delete cb;
		//curl_easy_cleanup(c);
	}

// Success:
	return (pBuffer_n * numBuffers);

Error:
	return -1;
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

HTTPControllerProxy* HTTPController::GetHTTPControllerProxy() {
	return (HTTPControllerProxy*)(this);
}

// Menu Controller Proxy
CLOUD_CONTROLLER_TYPE HTTPController::GetControllerType() {
	return CLOUD_CONTROLLER_TYPE::HTTP;
}

// TODO: Should we add this implementation?
RESULT HTTPController::RegisterControllerObserver(ControllerObserver* pControllerObserver) {
	return R_NOT_IMPLEMENTED;
}

RESULT HTTPController::RequestFile(std::string strURI, std::wstring strDestinationPath) {
	RESULT r = R_PASS;

	std::vector<std::string> strHeaders;
	std::string strBody;
	HTTPResponse httpResponse;

	DEBUG_LINEOUT("Requesting file %s to path %S", strURI.c_str(), strDestinationPath.c_str());

	//CR(FILE(strURI, strHeaders, strBody, strDestinationPath, httpResponse));
	CR(AFILE(strURI, strHeaders, strBody, strDestinationPath, nullptr));

Error:
	return r;
}
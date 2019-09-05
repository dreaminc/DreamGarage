#include "HTTPController.h"

#include <iostream>
#include <future>

#include <curl/curl.h>

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
			timeoutCURL = 5000;
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
			
				int numCURLMessagesInQueue = 0;
				struct CURLMsg *pCURLMsg = nullptr;
				
				while((pCURLMsg = curl_multi_info_read(m_pCURLMultiHandle, &numCURLMessagesInQueue)) != nullptr) {
					if (pCURLMsg->msg == CURLMSG_DONE) {
						CURL *pCURL = pCURLMsg->easy_handle;
						
						//transfers--;

						std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler = PopPendingHTTPRequestHandler(pCURL);
						if (pHTTPRequestHandler != nullptr) {
							// Check for Timeout
							if (pCURLMsg->data.result == CURLE_COULDNT_RESOLVE_HOST || pCURLMsg->data.result == CURLE_COULDNT_CONNECT) {
								CR(pHTTPRequestHandler->OnHTTPRequestTimeout());
							}
							else {
								CR(pHTTPRequestHandler->OnHTTPRequestComplete());
							}
						}

						curlMC = curl_multi_remove_handle(m_pCURLMultiHandle, pCURL);
						CBM((curlMC == CURLM_OK), "curl_multi_remove_handle");

						curl_easy_cleanup(pCURL);
					}
				}

				// TODO: Not sure if this is the right way
				// but if no more multi handles then clear the pending requests
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

size_t HTTPController::NumberOfPendingHTTPRequestHandlers() {
	return m_PendingHTTPRequestHandlers.size();
}

bool HTTPController::IsHTTPRequestHandlerPending() {
	return (NumberOfPendingHTTPRequestHandlers() > 0);
}

std::shared_ptr<HTTPRequestHandler> HTTPController::PopPendingHTTPRequestHandler(CURL *pCURL) {
	RESULT r = R_PASS;

	std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler = FindPendingHTTPRequestHandler(pCURL);
	CN(pHTTPRequestHandler);

	if (pHTTPRequestHandler != nullptr) {
		CR(RemovePendingHTTPRequestHandler(pHTTPRequestHandler));
	}

// Success:
	return pHTTPRequestHandler;

Error:
	if (pHTTPRequestHandler != nullptr) {
		pHTTPRequestHandler = nullptr;
	}
	return nullptr;
}

std::shared_ptr<HTTPRequestHandler> HTTPController::FindPendingHTTPRequestHandler(CURL *pCURL) {
	for (auto it = m_PendingHTTPRequestHandlers.begin(); it != m_PendingHTTPRequestHandlers.end(); it++) {
		if (pCURL == (*it)->GetCURLHandle()) {
			return (*it);
		}
	}

	return nullptr;
}

std::shared_ptr<HTTPRequestHandler> HTTPController::FindPendingHTTPRequestHandler(std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler) {
	auto it = std::find(m_PendingHTTPRequestHandlers.begin(), m_PendingHTTPRequestHandlers.end(), pHTTPRequestHandler);	
	
	if (it != m_PendingHTTPRequestHandlers.end())
		return (*it);

	return nullptr;
}

RESULT HTTPController::AddPendingHTTPRequestHandler(std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler) {
	RESULT r = R_PASS;

	CB((FindPendingHTTPRequestHandler(pHTTPRequestHandler) == nullptr));
	m_PendingHTTPRequestHandlers.push_back(pHTTPRequestHandler);

Error:
	return r;
}

RESULT HTTPController::RemovePendingHTTPRequestHandler(std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler) {
	RESULT r = R_PASS;

	CN(FindPendingHTTPRequestHandler(pHTTPRequestHandler));

	m_PendingHTTPRequestHandlers.remove(pHTTPRequestHandler);

Error:
	return r;
}

RESULT HTTPController::ClearPendingHTTPRequstHandlers() {
	m_PendingHTTPRequestHandlers.clear();
	return R_PASS;
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
	WCBM((cmc == CURLM_OK), "CURL failed to clean up multi handle");

	curl_global_cleanup();
	
Error:
	return r;
}

// TODO: Replace these functions with HTTPSession functionality
RESULT HTTPController::AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse* pHTTPResponse) {
	RESULT r = R_PASS;

	std::shared_ptr<HTTPRequestHandler>	pHTTPRequestHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestHandler = std::make_shared<HTTPRequestHandler>(new HTTPRequest(pCURL, strURI, strHeaders),
															   (pHTTPResponse) ? pHTTPResponse : &m_defaultResponse,
															   nullptr);
	CN(pHTTPRequestHandler);
	CR(AddPendingHTTPRequestHandler(pHTTPRequestHandler));

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curl_easy_setopt(pCURL, CURLOPT_VERBOSE, 1L);

	for (const auto& strHeader : pHTTPRequestHandler->GetRequestHeaders()) 
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestHandler.get());

	curl_multi_add_handle(m_pCURLMultiHandle, pCURL);

Error:
	return r;
}

RESULT HTTPController::AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnHTTPResponseCallback) {
	RESULT r = R_PASS;

	std::shared_ptr<HTTPRequestHandler>	pHTTPRequestHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestHandler = std::make_shared<HTTPRequestHandler>(new HTTPRequest(pCURL, strURI, strHeaders),
															   nullptr,
															   fnHTTPResponseCallback);
	CN(pHTTPRequestHandler);
	CR(AddPendingHTTPRequestHandler(pHTTPRequestHandler));

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());

	for (const auto& strHeader : pHTTPRequestHandler->GetRequestHeaders()) 
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestHandler.get());

	curl_multi_add_handle(m_pCURLMultiHandle, pCURL);

Error:
	return r;
}

RESULT HTTPController::AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnHTTPResponseCallback, HTTPTimeoutCallback fnHTTPTimeoutCallback, long timeout) {
	RESULT r = R_PASS;

	std::shared_ptr<HTTPRequestHandler>	pHTTPRequestHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestHandler = std::make_shared<HTTPRequestHandler>(new HTTPRequest(pCURL, strURI, strHeaders),
																nullptr,
																fnHTTPResponseCallback,
																fnHTTPTimeoutCallback);
	CN(pHTTPRequestHandler);
	CR(AddPendingHTTPRequestHandler(pHTTPRequestHandler));

	// Set to zero to switch to the default built - in connection timeout - 300 seconds.
	curl_easy_setopt(pCURL, CURLOPT_CONNECTTIMEOUT, timeout);
	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());

	for (const auto& strHeader : pHTTPRequestHandler->GetRequestHeaders())
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestHandler.get());

	curl_multi_add_handle(m_pCURLMultiHandle, pCURL);

Error:
	return r;
}

RESULT HTTPController::GET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse& httpResponse) {
	RESULT r = R_PASS;

	std::promise<std::string> httpPromise;
	std::future<std::string> httpFuture = httpPromise.get_future();

	CR(AGET(strURI, strHeaders, 
		[&](std::string&& in) -> RESULT {
			httpPromise.set_value(in); 
			return R_PASS; 
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

RESULT HTTPController::APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse* pHTTPResponse) {
	RESULT r = R_PASS;
	
	std::shared_ptr<HTTPRequestHandler>	pHTTPRequestHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestHandler = std::make_shared<HTTPRequestHandler>(new HTTPRequest(pCURL, strURI, strHeaders, strBody),
															   (pHTTPResponse) ? pHTTPResponse : &m_defaultResponse,
															   nullptr);
	CN(pHTTPRequestHandler);
	CR(AddPendingHTTPRequestHandler(pHTTPRequestHandler));

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curl_easy_setopt(pCURL, CURLOPT_POST, 1L);

	for (const auto& strHeader : pHTTPRequestHandler->GetRequestHeaders()) 
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
	curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, pHTTPRequestHandler->GetRequestBody().c_str());

	curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestHandler.get());

	curl_multi_add_handle(m_pCURLMultiHandle, pCURL);

Error:
	return r;
}

RESULT HTTPController::APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponseCallback fnHTTPResponseCallback) {
	RESULT r = R_PASS;

	CURLcode curlC = CURLE_OK;

	std::shared_ptr<HTTPRequestHandler>	pHTTPRequestHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestHandler = std::make_shared<HTTPRequestHandler>(new HTTPRequest(pCURL, strURI, strHeaders, strBody),
															   nullptr,
															   fnHTTPResponseCallback);
	CN(pHTTPRequestHandler);
	CR(AddPendingHTTPRequestHandler(pHTTPRequestHandler));
		
	curlC = curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curlC = curl_easy_setopt(pCURL, CURLOPT_POST, 1L);

	for (const auto& strHeader : pHTTPRequestHandler->GetRequestHeaders()) 
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	curlC = curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
	curlC = curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, pHTTPRequestHandler->GetRequestBody().c_str());

	curlC = curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestHandler->GetRequestURI().c_str());
	curlC = curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curlC = curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestHandler.get());

	curl_multi_add_handle(m_pCURLMultiHandle, pCURL);

Error:
	return r;
}

RESULT HTTPController::POST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse& httpResponse) {
	RESULT r = R_PASS;

	std::promise<std::string> httpPromise;
	std::future<std::string> httpFuture = httpPromise.get_future();

	CR(APOST(strURI, strHeaders, strBody, 
		[&](std::string&& strFutureResponse) -> RESULT { 
			httpPromise.set_value(strFutureResponse); 
			return R_PASS;
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

size_t HTTPController::RequestCallback(char *pBuffer, size_t elementSize, size_t numElements, void *pContext) {
	RESULT r = R_PASS;

	size_t retVal = -1;
	// TODO: This might be overkill 
	//std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler = std::shared_ptr<HTTPRequestHandler>(reinterpret_cast<HTTPRequestHandler*>(pContext));

	// Passed as a raw pointer above
	HTTPRequestHandler* pHTTPRequestHandler = reinterpret_cast<HTTPRequestHandler*>(pContext);
	CN(pHTTPRequestHandler);

	// callback error, should we log out a warning(?)
	CNM(pBuffer, "HTTP callback error");
	CNM(pHTTPRequestHandler, "HTTP callback error");

	CR(pHTTPRequestHandler->HandleHTTPResponse(pBuffer, elementSize, numElements));

	// Want to ensure single path exit to release http handler,
	// so only set retVal to correct value if everything went well.
	retVal = elementSize * numElements;

Error:
	/*
	// Do this in thread
	if (pHTTPRequestHandler != nullptr) {
		delete pHTTPRequestHandler;
		pHTTPRequestHandler = nullptr;
	}
	*/

	return retVal;
}

// FILE DOWNLOAD
RESULT HTTPController::AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponse* pHTTPResponse) {
	RESULT r = R_PASS;

	CURLMcode curlMC = CURLM_OK;
	CURLcode curlC = CURLE_OK;

	std::shared_ptr<HTTPRequestFileHandler>	pHTTPRequestFileHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestFileHandler = std::make_shared<HTTPRequestFileHandler>(new HTTPRequest(pCURL, strURI, strHeaders, strBody),
																	   pHTTPResponse,
																	   nullptr);
	CN(pHTTPRequestFileHandler);
	CR(AddPendingHTTPRequestHandler(pHTTPRequestFileHandler));

	if(strDestinationPath.length() > 0)
		pHTTPRequestFileHandler->SetDestinationFilePath(strDestinationPath);

	// Add the headers
	for (const auto& strHeader : pHTTPRequestFileHandler->GetRequestHeaders()) 
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	// CURL
	curlC = curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestFileHandler->GetRequestURI().c_str());
	curlC = curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
	curlC = curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curlC = curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestFileHandler.get());

	// Add multi handle (will get picked up in thread)
	curlMC = curl_multi_add_handle(m_pCURLMultiHandle, pCURL);
	CB((curlMC == CURLM_OK));

Error:
	return r;
}


RESULT HTTPController::AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponseFileCallback fnResponseFileCallback, void *pContext) {
	RESULT r = R_PASS;

	CURLMcode curlMC = CURLM_OK;
	CURLcode curlC = CURLE_OK;

	std::shared_ptr<HTTPRequestFileHandler>	pHTTPRequestFileHandler = nullptr;
	struct curl_slist *pCURLList = nullptr;

	CURL* pCURL = curl_easy_init();
	CN(pCURL);

	pHTTPRequestFileHandler = std::make_shared<HTTPRequestFileHandler>(new HTTPRequest(pCURL, strURI, strHeaders, strBody),
																	   nullptr,
																	   fnResponseFileCallback);
	CN(pHTTPRequestFileHandler);

	if (pContext != nullptr) {
		pHTTPRequestFileHandler->SetHandlerContext(pContext);
	}

	CR(AddPendingHTTPRequestHandler(pHTTPRequestFileHandler));

	if (strDestinationPath.length() > 0)
		pHTTPRequestFileHandler->SetDestinationFilePath(strDestinationPath);

	// Add the headers
	for (const auto& strHeader : pHTTPRequestFileHandler->GetRequestHeaders()) 
		pCURLList = curl_slist_append(pCURLList, strHeader.c_str());

	// CURL
	curlC = curl_easy_setopt(pCURL, CURLOPT_URL, pHTTPRequestFileHandler->GetRequestURI().c_str());
	curlC = curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, pCURLList);
	curlC = curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, &HTTPController::RequestCallback);
	curlC = curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, pHTTPRequestFileHandler.get());

	// Add multi handle (will get picked up in thread)
	curlMC = curl_multi_add_handle(m_pCURLMultiHandle, pCURL);
	CB((curlMC == CURLM_OK));

Error:
	return r;
}

/*
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
*/

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

RESULT HTTPController::RequestFile(std::string strURI, std::vector<std::string> strHeaders, std::string strBody, HTTPResponseFileCallback fnResponseFileCallback, void *pContext) {
	RESULT r = R_PASS;

	//std::vector<std::string> strHeaders;
	//std::string strBody;
	HTTPResponse httpResponse;

	DEBUG_LINEOUT("Requesting file %s to buffer", strURI.c_str());

	CR(AFILE(strURI, strHeaders, strBody, L"", fnResponseFileCallback, pContext));

Error:
	return r;
}

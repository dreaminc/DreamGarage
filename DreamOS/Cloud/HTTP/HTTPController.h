#ifndef HTTP_CONTROLLER_H_
#define HTTP_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include "curl/curl.h"

#include <string>
#include <vector>
#include <functional>
#include <thread>

#include "HTTPResponse.h"
#include "HTTPRequest.h"

#define HTTP_DELAY_SECONDS 5

typedef std::function<void(std::string&&)> HTTPResponseCallback;

class HTTPRequestHandler {
public:
	HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
		m_pHTTPRequest(pHTTPRequest),
		m_pHTTPResponse(pHTTPResponse),
		m_fnResponseCallback(fnResponseCallback)
	{
		// empty
	}

	HTTPRequest* m_pHTTPRequest;
	HTTPResponse* m_pHTTPResponse = nullptr;
	HTTPResponseCallback m_fnResponseCallback;
};

class HTTPController {
public:
	HTTPController();
	~HTTPController();

public:
	RESULT Start();
	RESULT Stop();

	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse* pHTTPResponse  = nullptr);
	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnResponseCallback);
	RESULT GET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse& httpResponse);

	RESULT APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse* pHTTPResponse = nullptr);
	RESULT APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponseCallback fnResponseCallback);
	RESULT POST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse& httpResponse);

	static const std::vector<std::string> ContentHttp() {
		return { "Content-Type: application/x-www-form-urlencoded" };
	}

	static const std::vector<std::string> ContentAcceptJson() {
		return { "Content-Type: application/json", "Accept: application/json; version=1.0" };
	}

private:
	static size_t RequestCallback(void *ptr, size_t size, size_t nmemb, HTTPRequestHandler *cb);

	RESULT Request(std::function<HTTPRequestHandler*(CURL*)> fnHTTPRequestCallback);
	
	// Thread processing http request / response
	void ProcessingThread();

	// Updates the requests
	void Update();

public:
	// Singleton
	static HTTPController *s_pInstance;

	static HTTPController *instance() {
		if (s_pInstance == nullptr)
			s_pInstance = new HTTPController();

		return s_pInstance;
	}

private:
	std::thread	m_thread;
	bool m_fRunning;

	CURLM* m_pCURLMultiHandle;
	int m_CURLMultiHandleCount;

	// used as a default response when a response callback is not set
	HTTPResponse m_defaultResponse;
};

#endif // HTTP_CONTROLLER_H_

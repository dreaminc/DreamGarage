#ifndef HTTP_REQUEST_HANDLER_H_
#define HTTP_REQUEST_HANDLER_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/HTTP/HTTPRequestHandler.h

// Base User object

#include <vector>
#include <functional>

#include "HTTPResponse.h"
#include "HTTPCommon.h"

class HTTPRequest;

class HTTPRequestHandler {
public:
	HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback, HTTPTimeoutCallback fnTimeoutCallback = nullptr);
	~HTTPRequestHandler();

	virtual RESULT OnHTTPRequestComplete();
	virtual RESULT OnHTTPRequestTimeout();
	virtual RESULT HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements);
	RESULT HandleHTTPResponse(std::string strResponse);

public:
	const std::string& GetRequestURI();
	std::vector<std::string> GetRequestHeaders();
	const std::string& GetRequestBody();

	CURL *GetCURLHandle();

private:
	HTTPRequest* m_pHTTPRequest;
	HTTPResponse* m_pHTTPResponse = nullptr;
	HTTPResponseCallback m_fnResponseCallback = nullptr;
	HTTPTimeoutCallback m_fnTimeoutCallback = nullptr;
};


#endif	// ! HTTP_REQUEST_HANDLER_H_
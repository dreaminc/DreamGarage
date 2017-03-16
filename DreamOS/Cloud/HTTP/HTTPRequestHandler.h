#ifndef HTTP_REQUEST_HANDLER_H_
#define HTTP_REQUEST_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include <functional>
#include "HTTPResponse.h"

class HTTPRequest;

typedef std::function<void(std::string&&)> HTTPResponseCallback;

class HTTPRequestHandler {
public:
	HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback);

	RESULT HandleHTTPResponse(std::string strResponse);

public:
	std::string GetRequestURI();
	std::vector<std::string> GetRequestHeaders();
	std::string GetRequestBody();

private:
	HTTPRequest* m_pHTTPRequest;
	HTTPResponse* m_pHTTPResponse = nullptr;
	HTTPResponseCallback m_fnResponseCallback = nullptr;
};


#endif	// ! HTTP_REQUEST_HANDLER_H_
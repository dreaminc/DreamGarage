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
	HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
		m_pHTTPRequest(pHTTPRequest),
		m_pHTTPResponse(pHTTPResponse),
		m_fnResponseCallback(fnResponseCallback)
	{
		// empty
	}

	HTTPRequest* m_pHTTPRequest;
	HTTPResponse* m_pHTTPResponse = nullptr;
	HTTPResponseCallback m_fnResponseCallback = nullptr;
};


#endif	// ! HTTP_REQUEST_HANDLER_H_
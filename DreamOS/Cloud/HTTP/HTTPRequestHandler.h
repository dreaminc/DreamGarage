#ifndef HTTP_REQUEST_HANDLER_H_
#define HTTP_REQUEST_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include <vector>
#include <functional>
#include "HTTPResponse.h"

class HTTPRequest;

typedef std::function<void(std::string&&)> HTTPResponseCallback;

class HTTPRequestHandler {
public:
	HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback);
	~HTTPRequestHandler();

	virtual RESULT HandleHTTPResponse(char *pBuffer, size_t pBuffer_n);
	RESULT HandleHTTPResponse(std::string strResponse);

public:
	const std::string& GetRequestURI();
	std::vector<std::string> GetRequestHeaders();
	const std::string& GetRequestBody();

private:
	HTTPRequest* m_pHTTPRequest;
	HTTPResponse* m_pHTTPResponse = nullptr;
	HTTPResponseCallback m_fnResponseCallback = nullptr;
};


#endif	// ! HTTP_REQUEST_HANDLER_H_
#ifndef HTTP_REQUEST_FILE_HANDLER_H_
#define HTTP_REQUEST_FILE_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include <functional>

#include "HTTPRequestHandler.h"

class HTTPRequest;
class HTTPResponse;

class HTTPRequestFileHandler : public HTTPRequestHandler {
public:
	HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback);
	
	~HTTPRequestFileHandler();

private:
	HTTPRequest* m_pHTTPRequest = nullptr;
	HTTPResponse* m_pHTTPResponse = nullptr;
	HTTPResponseCallback m_fnResponseCallback = nullptr;
};


#endif	// ! HTTP_REQUEST_FILE_HANDLER_H_
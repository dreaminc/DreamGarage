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

	RESULT SetDestinationFilePath(std::wstring wstrDestinationFilepath);
	virtual RESULT HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements) override;

private:
	std::wstring m_wstrDestinationFilePath;

};


#endif	// ! HTTP_REQUEST_FILE_HANDLER_H_
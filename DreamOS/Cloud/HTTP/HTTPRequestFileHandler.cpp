#include "HTTPRequestFileHandler.h"

HTTPRequestFileHandler::HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
	HTTPRequestHandler(pHTTPRequest, pHTTPResponse, fnResponseCallback)
{ 
	// empty
}

RESULT HTTPRequestFileHandler::SetDestinationFilePath(std::wstring wstrDestinationFilepath) {
	m_wstrDestinationFilePath = wstrDestinationFilepath;
	return R_PASS;
}

RESULT HTTPRequestFileHandler::HandleHTTPResponse(char *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	CN(pBuffer);
	CB(pBuffer_n > 0);

	// Save to file
	int a = 5;

Error:
	return r;
}
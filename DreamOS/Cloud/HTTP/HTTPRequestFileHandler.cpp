#include "HTTPRequestFileHandler.h"

HTTPRequestFileHandler::HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
	HTTPRequestHandler(pHTTPRequest, pHTTPResponse, fnResponseCallback)
{ 
	// empty
}

HTTPRequestFileHandler::~HTTPRequestFileHandler() {
	if (m_pHTTPRequest != nullptr) {
		delete m_pHTTPRequest;
		m_pHTTPRequest = nullptr;
	}

	if (m_pHTTPResponse != nullptr) {
		delete m_pHTTPResponse;
		m_pHTTPResponse = nullptr;
	}

	if (m_fnResponseCallback != nullptr) {
		m_fnResponseCallback = nullptr;
	}
}

// 
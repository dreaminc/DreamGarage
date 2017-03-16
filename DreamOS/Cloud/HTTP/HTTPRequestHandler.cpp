#include "HTTPRequestHandler.h"

#include "HTTPRequest.h"
#include "HTTPResponse.h"

HTTPRequestHandler::HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
	m_pHTTPRequest(pHTTPRequest),
	m_pHTTPResponse(pHTTPResponse),
	m_fnResponseCallback(fnResponseCallback)
{
	// empty
}

std::string HTTPRequestHandler::GetRequestURI() {
	if (m_pHTTPRequest != nullptr) {
		return m_pHTTPRequest->GetURI();
	}

	return std::string();
}

std::vector<std::string> HTTPRequestHandler::GetRequestHeaders() {
	if(m_pHTTPRequest != nullptr)
		return m_pHTTPRequest->GetHeaders();

	return std::vector<std::string>();
}

std::string HTTPRequestHandler::GetRequestBody() {
	if(m_pHTTPRequest != nullptr)
		return m_pHTTPRequest->GetBody();

	return std::string();
}


RESULT HTTPRequestHandler::HandleHTTPResponse(std::string strResponse) {
	RESULT r = R_PASS;

	if (m_pHTTPResponse != nullptr) {
		// Hand off response to response object
		CR(m_pHTTPResponse->HandleHTTPResponse(strResponse));
	}

	if (m_fnResponseCallback != nullptr) {
		m_fnResponseCallback(std::move(strResponse));	
	}

Error:
	return r;
}

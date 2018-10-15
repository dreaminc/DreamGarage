#include "HTTPRequestHandler.h"

#include "HTTPRequest.h"
#include "HTTPResponse.h"

HTTPRequestHandler::HTTPRequestHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback, HTTPTimeoutCallback fnTimeoutCallback) :
	m_pHTTPRequest(pHTTPRequest),
	m_pHTTPResponse(pHTTPResponse),
	m_fnResponseCallback(fnResponseCallback),
	m_fnTimeoutCallback(fnTimeoutCallback)
{
	// empty
}

HTTPRequestHandler::~HTTPRequestHandler() {
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

const std::string& HTTPRequestHandler::GetRequestURI() {
	ACN(m_pHTTPRequest);
	return m_pHTTPRequest->GetURI();
}

std::vector<std::string> HTTPRequestHandler::GetRequestHeaders() {
	ACN(m_pHTTPRequest);
	return m_pHTTPRequest->GetHeaders();
}

const std::string& HTTPRequestHandler::GetRequestBody() {
	ACN(m_pHTTPRequest);
	return m_pHTTPRequest->GetBody();
}

CURL* HTTPRequestHandler::GetCURLHandle() {
	return m_pHTTPRequest->GetCURLHandle();
}

RESULT HTTPRequestHandler::OnHTTPRequestComplete() {
	return R_NOT_IMPLEMENTED;
}

RESULT HTTPRequestHandler::OnHTTPRequestTimeout() {
	RESULT r = R_PASS;

	if (m_fnTimeoutCallback != nullptr) {
		m_fnTimeoutCallback();
	}

	return r;
}

RESULT HTTPRequestHandler::HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements) {
	RESULT r = R_PASS;

	size_t pBuffer_n = elementSize * numElements;

	CN(pBuffer);
	CB(pBuffer_n > 0);

	// Convert into string, this call will limit to the size of the buffer passed in 
	CR(HandleHTTPResponse(std::string(pBuffer, pBuffer_n)));

Error:
	return r;
}

RESULT HTTPRequestHandler::HandleHTTPResponse(std::string strResponse) {
	RESULT r = R_PASS;

	if (m_pHTTPResponse != nullptr) {
		// Hand off response to response object
		CR(m_pHTTPResponse->OnResponse(std::move(strResponse)));
	}

	if (m_fnResponseCallback != nullptr) {
		m_fnResponseCallback(std::move(strResponse));	
	}

Error:
	return r;
}

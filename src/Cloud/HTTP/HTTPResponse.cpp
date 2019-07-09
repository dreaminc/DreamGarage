#include "HTTPResponse.h"

RESULT HTTPResponse::OnResponse(std::string&& strResponse) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("HTTP response: %s", strResponse.c_str());

//Error:
	return r;
}

const std::string& HTTPResponse::GetResponse() {
	return m_strResponse;
}

const std::string& HTTPResponse::PullResponse() {
	return std::move(m_strResponse);
}

void HTTPResponse::PutResponse(std::string& strResponse) {
	m_strResponse = std::move(strResponse);
}
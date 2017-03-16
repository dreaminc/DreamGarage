#include "HTTPResponse.h"

void HTTPResponse::OnResponse(std::string&& strResponse) {
	DEBUG_LINEOUT("HTTP response: %s", strResponse.c_str());
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
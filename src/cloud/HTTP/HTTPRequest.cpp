#include "HTTPRequest.h"

HTTPRequest::HTTPRequest(CURL *pCURL, std::string strURI, std::vector<std::string> strHeaders) :
	m_pCURL(pCURL),
	m_strURI(strURI),
	m_strHeaders(strHeaders)
{
	// empty
}

HTTPRequest::HTTPRequest(CURL *pCURL, std::string strURI, std::vector<std::string> strHeaders, std::string strBody) :
	m_pCURL(pCURL),
	m_strURI(strURI),
	m_strHeaders(strHeaders),
	m_strBody(strBody)
{
	// empty
}

HTTPRequest::~HTTPRequest() {
	// This is actually handled in the multi-handle thread code
	// doing this here screws up the multi-handle
	/*
	if (m_pCURL != nullptr) {
		curl_easy_cleanup(m_pCURL);
		m_pCURL = nullptr;
	}
	*/
}

const std::string& HTTPRequest::GetURI() {
	return m_strURI;
}

CURL* HTTPRequest::GetCURLHandle() {
	return m_pCURL;
}

std::vector<std::string> HTTPRequest::GetHeaders() {
	return m_strHeaders;
}

const std::string& HTTPRequest::GetBody() {
	return m_strBody;
}
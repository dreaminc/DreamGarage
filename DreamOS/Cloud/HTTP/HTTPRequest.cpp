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
	if (m_pCURL != nullptr) {
		curl_easy_cleanup(m_pCURL);
		m_pCURL = nullptr;
	}
}
std::string HTTPRequest::GetURI() {
	return m_strURI;
}

std::vector<std::string> HTTPRequest::GetHeaders() {
	return m_strHeaders;
}

std::string HTTPRequest::GetBody() {
	return m_strBody;
}
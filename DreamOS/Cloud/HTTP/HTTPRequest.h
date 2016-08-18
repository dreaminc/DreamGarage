#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/HTTP/HTTPRequest.h
// HTTP Request Object

#include "curl/curl.h"

class HTTPRequest {
public:
	HTTPRequest(CURL *pCURL, std::string strURI, std::vector<std::string> strHeaders) :
		m_pCURL(pCURL),
		m_strURI(strURI),
		m_strHeaders(strHeaders)
	{
		// empty
	}

	HTTPRequest(CURL *pCURL, std::string strURI, std::vector<std::string> strHeaders, std::string strBody) :
		m_pCURL(pCURL),
		m_strURI(strURI),
		m_strHeaders(strHeaders),
		m_strBody(strBody)
	{
		// empty
	}

public:
	CURL* m_pCURL;
	std::string m_strURI;
	std::vector<std::string> m_strHeaders;
	std::string m_strBody;
};

#endif	// !HTTP_REQUEST_H_
#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/HTTP/HTTPRequest.h
// HTTP Request Object

#include "curl/curl.h"

#include <string>
#include <vector>

class HTTPRequest {
public:
	HTTPRequest(CURL *pCURL, std::string strURI, std::vector<std::string> strHeaders);
	HTTPRequest(CURL *pCURL, std::string strURI, std::vector<std::string> strHeaders, std::string strBody);
	~HTTPRequest();

	std::vector<std::string> GetHeaders();
	const std::string& GetBody();
	const std::string& GetURI();
	CURL *GetCURLHandle();

private:
	CURL* m_pCURL;
	std::string m_strURI;
	std::vector<std::string> m_strHeaders;
	std::string m_strBody;
};

#endif	// !HTTP_REQUEST_H_
#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "core/ehm/EHM.h"

// Dream Cloud HTTP
// dos/src/cloud/HTTP/HTTPRequest.h

// HTTP Request Object

// TODO: move into CMake config unless this breaks the build anyways
#include "third_party/libcurl/curl-7.49.1/include/curl/curl.h"

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
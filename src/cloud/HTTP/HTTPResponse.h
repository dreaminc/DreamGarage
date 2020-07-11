#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "core/ehm/EHM.h"

// Dream Cloud HTTP
// dos/src/cloud/HTTP/HTTPResponse.h

// HTTP Response object

#include <string>
#include <functional>

// TODO: move into CMake config unless this breaks the build anyways
//#include "third_party/libcurl/curl-7.49.1/include/curl/curl.h"

class CURL;

class HTTPResponse {
	friend class HTTPController;

public:
	virtual RESULT OnResponse(std::string&& strResponse);

	const std::string& GetResponse();
	const std::string& PullResponse();

protected:
	void PutResponse(std::string& strResponse);

private:
	std::string	m_strResponse;
};

#endif	// ! HTTP_RESPONSE_H_
#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/HTTP/HTTPResponse.h
// HTTP Response object

#include "curl/curl.h"

#include <string>
#include <functional>

typedef std::function<void(std::string&&)> HTTPResponseCallback;

class HTTPResponse {
	friend class HTTPController;

public:
	virtual void OnResponse(std::string&& strResponse);

	const std::string& GetResponse();
	const std::string& PullResponse();

protected:
	void PutResponse(std::string& strResponse);

private:
	std::string	m_strResponse;
};

#endif	// ! HTTP_RESPONSE_H_
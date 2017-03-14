#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/HTTP/HTTPResponse.h
// HTTP Response object

#include "curl/curl.h"

typedef std::function<void(std::string&&)> HTTPResponseCallback;

class HTTPResponse {
public:
	virtual void OnResponse(std::string&& response);

	const std::string& GetResponse() {
		return m_strResponse;
	}

	const std::string& PullResponse() {
		return std::move(m_strResponse);
	}

protected:
	friend class HTTPController;

	void PutResponse(std::string& strResponse) {
		m_strResponse = std::move(strResponse);
	}

private:
	std::string	m_strResponse;
};

#endif	// ! HTTP_RESPONSE_H_
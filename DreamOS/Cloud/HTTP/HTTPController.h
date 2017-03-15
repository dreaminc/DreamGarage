#ifndef HTTP_CONTROLLER_H_
#define HTTP_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include "curl/curl.h"

#include <string>
#include <vector>
#include <functional>
#include <thread>

#include "HTTPResponse.h"
#include "HTTPRequest.h"

#include "Cloud/ControllerProxy.h"

#define HTTP_DELAY_SECONDS 5

class HTTPRequestHandler;
class HTTPRequestFileHandler;

class HTTPControllerProxy : public ControllerProxy {
public:
	//virtual RESULT RequestFile(std::string strURI, std::string strDestinationPath) = 0;
};


class HTTPController : public HTTPControllerProxy {
public:
	HTTPController();
	~HTTPController();

public:
	RESULT Initialize();
	RESULT Shutdown();
	RESULT Start();
	RESULT Stop();

	// GET
	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse* pHTTPResponse  = nullptr);
	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnResponseCallback);
	RESULT GET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse& httpResponse);

	// POST
	RESULT APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse* pHTTPResponse = nullptr);
	RESULT APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponseCallback fnResponseCallback);
	RESULT POST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse& httpResponse);

	// FILE DOWNLOAD
	//RESULT AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::string &strDesPath, HTTPResponse* pHTTPResponse = nullptr);
	RESULT AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::string &strDesPath, HTTPResponseCallback fnResponseCallback);
	RESULT FILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::string &strDestPath, HTTPResponse& httpResponse);


	static const std::vector<std::string> ContentHttp() {
		return { "Content-Type: application/x-www-form-urlencoded" };
	}

	static const std::vector<std::string> ContentAcceptJson() {
		return { "Content-Type: application/json", "Accept: application/json; version=1.0" };
	}

public:
	HTTPControllerProxy* GetHTTPControllerProxy();

	// Menu Controller Proxy
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) override;

private:
	static size_t RequestCallback(void *pContext, size_t size, size_t nmemb, HTTPRequestHandler *pHTTPRequestHandler);
	static size_t RequestFileCallback(void *pContext, size_t size, size_t nmemb, HTTPRequestFileHandler *pHTTPRequestFileHandler);

	RESULT Request(std::function<HTTPRequestHandler*(CURL*)> fnHTTPRequestCallback);
	RESULT RequestFile(std::function<HTTPRequestFileHandler*(CURL*)> fnHTTPRequestFileCallback);
	
	// Thread processing http request / response
	void ProcessingThread();

	// Updates the requests
	void Update();

public:
	// TODO: Remove the singleton
	// Singleton
	static HTTPController *s_pInstance;

	static HTTPController *CreateHTTPController();

	static HTTPController *instance() {
		if (s_pInstance == nullptr)
			s_pInstance = CreateHTTPController();

		return s_pInstance;
	}

private:
	std::thread	m_thread;
	bool m_fRunning;

	CURLM* m_pCURLMultiHandle;
	int m_CURLMultiHandleCount;

	// used as a default response when a response callback is not set
	HTTPResponse m_defaultResponse;
};

#endif // HTTP_CONTROLLER_H_

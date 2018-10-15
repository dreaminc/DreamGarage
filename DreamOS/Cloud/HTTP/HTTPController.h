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
#include <list>
#include <memory>

#include "HTTPResponse.h"
#include "HTTPRequest.h"

#include "Cloud/ControllerProxy.h"

#define HTTP_DELAY_SECONDS 5

class HTTPRequestHandler;
class HTTPRequestFileHandler;

#include "HTTPCommon.h"

class HTTPControllerProxy : public ControllerProxy {
public:
	virtual RESULT RequestFile(std::string strURI, std::wstring strDestinationPath) = 0;
	//virtual RESULT RequestFile(std::string strURI, HTTPResponseFileCallback fnResponseFileCallback) = 0;
	virtual RESULT RequestFile(std::string strURI, std::vector<std::string> strHeaders, std::string strBody, HTTPResponseFileCallback fnResponseFileCallback, void *pContext = nullptr) = 0;
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

	// TODO: Might want to rename these
	// TODO: Wrap up in HTTPRequest that is passed to HTTPController
	// which sets it's params (POST/GET/FILE etc and handles everything on one path)

	// TODO: Replace these functions with HTTPSession functionality

	// GET
	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse* pHTTPResponse  = nullptr);
	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnResponseCallback);
	RESULT AGET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponseCallback fnResponseCallback, HTTPTimeoutCallback fnTimeoutCallback, long timout);
	RESULT GET(const std::string& strURI, const std::vector<std::string>& strHeaders, HTTPResponse& httpResponse);

	// POST
	RESULT APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse* pHTTPResponse = nullptr);
	RESULT APOST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponseCallback fnResponseCallback);
	RESULT POST(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, HTTPResponse& httpResponse);

	// FILE DOWNLOAD
	RESULT AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponse* pHTTPResponse = nullptr);
	RESULT AFILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponseFileCallback fnResponseFileCallback, void *pContext = nullptr);
	//RESULT FILE(const std::string& strURI, const std::vector<std::string>& strHeaders, const std::string& strBody, const std::wstring &strDestinationPath, HTTPResponse& httpResponse);

	static const std::vector<std::string> ContentHttp() {
		return { "Content-Type: application/x-www-form-urlencoded" };
	}

	static const std::vector<std::string> ContentAcceptJson() {
		return { "Content-Type: application/json", "Accept: application/json; version=1.0" };
	}

	static const std::string AuthorizationHeader(std::string& strAccessToken) {
		return "Authorization: Bearer " + strAccessToken;
	}

public:
	HTTPControllerProxy* GetHTTPControllerProxy();

	// Menu Controller Proxy
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) override;

	virtual RESULT RequestFile(std::string strURI, std::wstring strDestinationPath) override;
	//virtual RESULT RequestFile(std::string strURI, HTTPResponseFileCallback fnResponseFileCallback) override;
	virtual RESULT RequestFile(std::string strURI, std::vector<std::string> strHeaders, std::string strBody, HTTPResponseFileCallback fnResponseFileCallback, void *pContext = nullptr) override;

private:
	// CURL Callbacks
	static size_t RequestCallback(char *pBuffer, size_t elementSize, size_t numElements, void *pContext);
	
	// Thread processing http request / response
	void CURLMultihandleThreadProcess();

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
	size_t NumberOfPendingHTTPRequestHandlers();
	bool IsHTTPRequestHandlerPending();
	std::shared_ptr<HTTPRequestHandler> PopPendingHTTPRequestHandler(CURL *pCURL);
	std::shared_ptr<HTTPRequestHandler> FindPendingHTTPRequestHandler(CURL *pCURL);
	std::shared_ptr<HTTPRequestHandler> FindPendingHTTPRequestHandler(std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler);
	RESULT AddPendingHTTPRequestHandler(std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler);
	RESULT RemovePendingHTTPRequestHandler(std::shared_ptr<HTTPRequestHandler> pHTTPRequestHandler);
	RESULT ClearPendingHTTPRequstHandlers();

	std::list<std::shared_ptr<HTTPRequestHandler>> m_PendingHTTPRequestHandlers;

private:
	std::thread	m_thread;
	bool m_fRunning;

	CURLM* m_pCURLMultiHandle;
	int m_CURLMultiHandleCount;

	// used as a default response when a response callback is not set
	HTTPResponse m_defaultResponse;
};

#endif // HTTP_CONTROLLER_H_

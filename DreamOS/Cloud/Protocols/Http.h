#pragma once

#include "curl/curl.h"

#include <string>
#include <vector>
#include <functional>
#include <thread>

class HttpResponse
{
public:
	virtual void OnResponse(std::string&& response);

	const std::string& GetResponse() {
		return m_response;
	}

	const std::string& PullResponse() {
		return std::move(m_response);
	}

protected:
	friend class Http;

	void PutResponse(std::string& response)
	{
		m_response = std::move(response);
	}

private:
	std::string	m_response;
};

struct HttpRequest
{
	CURL*	curl;
	std::string uri;
	std::vector<std::string> header;
	std::string body;
};

class Http
{
public:
	static Http* GetHttp()
	{
		static Http http;
		return &http;
	}

	Http();
	~Http();

	void Start();
	void Stop();

	typedef std::function<void(std::string&&)> HttpResponseFunc_t;

	typedef struct HttpRequestHandler_t
	{
		HttpRequestHandler_t(HttpRequest* req,
			HttpResponse* res,
			HttpResponseFunc_t resFunc) :
			request(req),
			response(res),
			responseFunc(resFunc)
		{
			
		}

		HttpRequest*	request;
		HttpResponse*	response = nullptr;
		HttpResponseFunc_t responseFunc;
	};

	bool AGET(const std::string& uri, const std::vector<std::string>& header, HttpResponse* response = nullptr);
	bool AGET(const std::string& uri, const std::vector<std::string>& header, HttpResponseFunc_t responseFunc);
	bool GET(const std::string& uri, const std::vector<std::string>& header, HttpResponse& response);

	bool APOST(const std::string& uri, const std::vector<std::string>& header, const std::string& body, HttpResponse* response = nullptr);
	bool APOST(const std::string& uri, const std::vector<std::string>& header, const std::string& body, HttpResponseFunc_t responseFunc);
	bool POST(const std::string& uri, const std::vector<std::string>& header, const std::string& body, HttpResponse& response);

	static const std::vector<std::string> JsonHeader() {
		return { "Content-Type: application/json", "Accept: application/json; version=1.0" };
	}

private:
	static size_t callback(void *ptr, size_t size, size_t nmemb, HttpRequestHandler_t *cb);

	bool Request(std::function<HttpRequestHandler_t*(CURL*)> request);
	
	// Thread processing http request / response
	void ProcessingThread();

	// Updates the requests
	void Update();

private:
	std::thread	m_thread;
	bool	m_isRunning;

	CURLM* multi_handle;
	int handle_count;

	// used as a default response when a response callback is not set
	HttpResponse	m_defaultResponse;
};

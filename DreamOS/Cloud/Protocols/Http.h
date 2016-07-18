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
	Http();
	~Http();

	void Start();
	void Stop();

	typedef struct HttpRequestHandler_t
	{
		HttpRequest*	request;
		HttpResponse*	response;
	};

	bool GET(const std::string& uri, HttpResponse* response = nullptr);
	bool POST(const std::string& uri, const std::vector<std::string>& header, const std::string& body, HttpResponse* response = nullptr);

private:
	static size_t callback(void *ptr, size_t size, size_t nmemb, HttpRequestHandler_t *cb);

	bool Request(std::function<HttpRequestHandler_t*(CURL*)> request, HttpResponse* response);
	
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

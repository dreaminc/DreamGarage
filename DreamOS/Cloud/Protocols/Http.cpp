#include "Http.h"

#include <curl/curl.h>
#include <iostream>

#ifdef _WIN32
#define SHORT_SLEEP Sleep(100)
#else
#define SHORT_SLEEP usleep(100000)
#endif


Http::Http() :
	m_isRunning(false)
{
	Start();
}

Http::~Http()
{
	Stop();
}

void Http::ProcessingThread()
{
	m_isRunning = true;

	timeval timeout;

	while (m_isRunning)
	{
		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		int maxfd = -1;

		long curl_timeo;

		curl_multi_timeout(multi_handle, &curl_timeo);
		if (curl_timeo < 0)
			curl_timeo = 1000;

		timeout.tv_sec = curl_timeo / 1000;
		timeout.tv_usec = (curl_timeo % 1000) * 1000;

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		// get file descriptors from the transfers
		CURLMcode mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

		int rc = 0;

		if (maxfd == -1) {
			SHORT_SLEEP;
			rc = 0;
		}
		else
			rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);

		switch (rc) {
		case -1:
			// select error => continue
			break;
		case 0:
		default:
			// timeout or readable/writable sockets
			Update();
			break;
		}
	}
}

void Http::Start()
{
	std::cout << "Http::Start" << std::endl;

	multi_handle = curl_multi_init();
	handle_count = 0;

	m_thread = std::thread(&Http::ProcessingThread, this);
}

void Http::Stop()
{
	std::cout << "Http::Stop" << std::endl;

	m_isRunning = false;
	m_thread.join();

	curl_multi_cleanup(multi_handle);
	curl_global_cleanup();
}

void Http::Update()
{
	curl_multi_perform(multi_handle, &handle_count);
}

bool Http::Request(std::function<HttpRequestHandler_t*(CURL*)> request, HttpResponse* response)
{
	CURL* curl = NULL;
	curl = curl_easy_init();

	if (!curl)
	{
		// init failed
		return false;
	}

	HttpRequestHandler_t*	cb = request(curl);

	curl_easy_setopt(curl, CURLOPT_URL, cb->request->uri.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Http::callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, cb);

	curl_multi_add_handle(multi_handle, curl);

	return true;
}

bool Http::GET(const std::string& uri, HttpResponse* response)
{
	return Request([&](CURL* curl) -> HttpRequestHandler_t*
		{ 
			HttpRequestHandler_t* cb = new HttpRequestHandler_t{
				new HttpRequest{ curl, uri },
				(response) ? response : &m_defaultResponse };

			curl_easy_setopt(curl, CURLOPT_URL, cb->request->uri.c_str());

			return cb;
		},
		response);
}

bool Http::POST(const std::string& uri, const std::vector<std::string>& header, const std::string& body, HttpResponse* response)
{
	return Request([&](CURL* curl) -> HttpRequestHandler_t*
		{
			HttpRequestHandler_t*	cb = new HttpRequestHandler_t{
				new HttpRequest{ curl, uri, header, body },
				(response) ? response : &m_defaultResponse };

			curl_easy_setopt(curl, CURLOPT_URL, cb->request->uri.c_str());
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			struct curl_slist *h = NULL;

			for (const auto& st : cb->request->header)
			{
				h = curl_slist_append(h, st.c_str());
			}

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cb->request->body.c_str());

			return cb;
		},
		response);
}

size_t Http::callback(void *ptr, size_t size, size_t nmemb, HttpRequestHandler_t *cb)
{
	if (!ptr)
	{
		// callback error, should we log out a warning(?)
		std::cout << "Http callback error" << std::endl;

		return 0;
	}

	if (ptr && cb)
	{
		std::string st(static_cast<char*>(ptr));

		cb->response->OnResponse(std::move(st));

		// cleanup should be made after the callback(!)
		//curl_easy_cleanup(cb->request->curl);

		delete cb->request;
		delete cb;
	}

	return size*nmemb;
}

void HttpResponse::OnResponse(std::string&& response)
{
	// default response
	std::cout << "Http response: " << response << std::endl;
}


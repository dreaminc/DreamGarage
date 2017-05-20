#ifndef WEB_REQUEST_H_
#define WEB_REQUEST_H_

#include <Primitives/DObject.h>

// DREAM OS
// DreamOS/Cloud/WebRequest.h
// This is the core Web Request object that should be used across the engine
// TODO: Use this across the engine

#include <string>
#include <map>
#include <memory>

class WebRequestPostData;

class WebRequest : public DObject {
public:
	enum class Method {
		POST,
		GET,
		INVALID
	};

public:
	WebRequest();

	RESULT SetURL(const std::wstring& wstrURL);
	const std::wstring& GetURL();
	const std::wstring GetRequestMethodString();
	static const std::wstring GetRequestMethodString(WebRequest::Method requestMethod);

	std::shared_ptr<WebRequestPostData> GetPostData();
	RESULT SetPostData(std::shared_ptr<WebRequestPostData> pWebRequestPostData);

	WebRequest::Method GetRequestMethod();
	RESULT SetRequestMethod(WebRequest::Method requestMethod);

	RESULT SetRequestHeaders(const std::multimap<std::wstring, std::wstring> &requestHeaders);
	const std::multimap<std::wstring, std::wstring> &GetRequestHeaders();
	RESULT ClearRequestHeaders();

	// Set all together
	RESULT Set(const std::wstring& wstrURL,
			   WebRequest::Method requestMethod,
			   std::shared_ptr<WebRequestPostData> pWebRequestPostData,
			   const std::multimap<std::wstring, std::wstring>& m_headers);

private:
	std::wstring m_wstrURL;
	WebRequest::Method m_requestMethod;
	std::shared_ptr<WebRequestPostData> m_pWebRequestPostData = nullptr;
	std::multimap<std::wstring, std::wstring> m_requestHeaders;
};

#endif // ! WEB_REQUEST_H_
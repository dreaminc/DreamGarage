#include "WebRequest.h"

WebRequest::WebRequest() {
	// empty
}

RESULT WebRequest::SetURL(const std::wstring& wstrURL) {
	m_wstrURL = wstrURL;
	return R_PASS;
}

const std::wstring& WebRequest::GetURL() {
	return m_wstrURL;
}

const std::wstring WebRequest::GetRequestMethodString() {
	return GetRequestMethodString(m_requestMethod);
}

const std::wstring WebRequest::GetRequestMethodString(WebRequest::Method requestMethod) {
	switch (requestMethod) {
		case Method::GET: return L"GET"; break;
		case Method::POST: return L"POST"; break;

		case Method::INVALID:
		default: return L"INVALID"; break;
	}

	return L"INVALID";
}

std::shared_ptr<WebRequestPostData> WebRequest::GetPostData() {
	return std::shared_ptr<WebRequestPostData>(m_pWebRequestPostData);
}

RESULT WebRequest::SetPostData(std::shared_ptr<WebRequestPostData> pWebRequestPostData) {
	m_pWebRequestPostData = pWebRequestPostData;
	return R_PASS;
}

WebRequest::Method WebRequest::GetRequestMethod() {
	return m_requestMethod;
}

RESULT WebRequest::SetRequestMethod(WebRequest::Method requestMethod) {
	m_requestMethod = requestMethod;
	return R_PASS;
}

RESULT WebRequest::ClearRequestHeaders() {
	m_requestHeaders.clear();
	return R_PASS;
}

RESULT WebRequest::SetRequestHeaders(const std::multimap<std::wstring, std::wstring> &requestHeaders) {
	RESULT r = R_PASS;

	CR(ClearRequestHeaders());
	m_requestHeaders = requestHeaders;

Error:
	return r;
}

const std::multimap<std::wstring, std::wstring>& WebRequest::GetRequestHeaders() {
	return m_requestHeaders;
}

// Set all together
RESULT WebRequest::Set(const std::wstring& wstrURL,
	WebRequest::Method requestMethod,
	std::shared_ptr<WebRequestPostData> pWebRequestPostData,
	const std::multimap<std::wstring, std::wstring>& requestHeaders)
{
	RESULT r = R_PASS;

	CR(SetURL(wstrURL));
	CR(SetRequestMethod(requestMethod));
	CR(SetPostData(pWebRequestPostData));
	CR(SetRequestHeaders(requestHeaders));

Error:
	return r;
}
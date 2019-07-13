#include "WebRequestPostData.h"
#include "WebRequestPostDataElement.h"

WebRequestPostData::WebRequestPostData() {
	// empty
}

RESULT WebRequestPostData::AddPostDataElement(std::wstring wstrValue) {
	RESULT r = R_PASS;

	CBM((FindPostDataElementByStringValue(wstrValue) == nullptr), "Value %S already present in POST data", wstrValue.c_str());

	{
		std::shared_ptr<WebRequestPostDataElement> pWebRequestPostDataElement = std::make_shared<WebRequestPostDataElement>(wstrValue);
		CN(pWebRequestPostDataElement);

		CR(AddPostDataElement(pWebRequestPostDataElement));
	}

Error:
	return r;
}

std::shared_ptr<WebRequestPostDataElement> WebRequestPostData::FindPostDataElementByStringValue(std::wstring wstrValue) {
	for (auto &pElement : m_postDataElements) {
		if (pElement->GetValue() == wstrValue) {
			return std::shared_ptr<WebRequestPostDataElement>(pElement);
		}
	}

	return nullptr;
}

RESULT WebRequestPostData::AddPostDataElement(std::shared_ptr<WebRequestPostDataElement> pPostDataElement) {
	RESULT r = R_PASS;

	CB((FindPostDataElement(pPostDataElement) == false));
	m_postDataElements.push_back(pPostDataElement);

Error:
	return r;
}

RESULT WebRequestPostData::RemovePostDataElement(std::shared_ptr<WebRequestPostDataElement> pPostDataElement) {
	for (auto it = m_postDataElements.begin(); it != m_postDataElements.end(); it++) {
		if ((*it) == pPostDataElement) {
			m_postDataElements.erase(it);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

bool WebRequestPostData::FindPostDataElement(std::shared_ptr<WebRequestPostDataElement> pPostDataElement) {
	for (auto &pElement : m_postDataElements) 
		if (pElement == pPostDataElement)
			return true;
	
	return false;
}

RESULT WebRequestPostData::Clear() {
	m_postDataElements.clear();
	return R_PASS;
}

size_t WebRequestPostData::GetElementCount() {
	return m_postDataElements.size();
}

std::vector<std::shared_ptr<WebRequestPostDataElement>> WebRequestPostData::GetElements() {
	return m_postDataElements;
}
#ifndef WEB_REQUEST_POST_DATA_H_
#define WEB_REQUEST_POST_DATA_H_

#include <Primitives/DObject.h>

// DREAM OS
// DreamOS/Cloud/WebRequestPostData.h
// The relevant WebRequest post data structure
// NOTE: This class is intentionally sparse -
// more functionality will fill this class over time as we need more and more capabilities 

#include <string>
#include <vector>
#include <memory>

class WebRequestPostDataElement;

class WebRequestPostData : public DObject {
public:
	WebRequestPostData();

	RESULT Clear();
	size_t GetElementCount();

	RESULT AddPostDataElement(std::wstring wstrValue);
	std::shared_ptr<WebRequestPostDataElement> FindPostDataElementByStringValue(std::wstring wstrValue);

	std::vector<std::shared_ptr<WebRequestPostDataElement>> GetElements();

private:
	RESULT AddPostDataElement(std::shared_ptr<WebRequestPostDataElement> pPostDataElement);
	RESULT RemovePostDataElement(std::shared_ptr<WebRequestPostDataElement> pPostDataElement);
	bool FindPostDataElement(std::shared_ptr<WebRequestPostDataElement> pPostDataElement);

private:
	std::vector<std::shared_ptr<WebRequestPostDataElement>> m_postDataElements;
};

#endif // ! WEB_REQUEST_H_
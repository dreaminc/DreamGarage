#ifndef WEB_REQUEST_POST_DATA_ELEMENT_H_
#define WEB_REQUEST_POST_DATA_ELEMENT_H_

#include <Primitives/DObject.h>

// DREAM OS
// DreamOS/Cloud/WebRequestPostData.h
// The relevant WebRequest post data structure

#include <string>
#include <vector>
#include <memory>

class WebRequestPostDataElement {
public:
	enum class Type {
		EMPTY,
		BYTES,
		STRING,
		FILE,
		INVALID
	};

public:
	WebRequestPostDataElement();
	//WebRequestPostDataElement(std::wstring wstrFilename);		// TODO: Plug this into the file arch
	WebRequestPostDataElement(uint8_t *pBuffer, size_t pBuffer_n);
	WebRequestPostDataElement(const std::wstring &wstrValue);

	std::wstring GetValue() { return m_wstrValue; }
	std::wstring GetFilename() { return m_wstrFilename; }

	uint8_t *GetBuffer() { return m_pBuffer; }
	size_t GetBufferSize() { return m_pBuffer_n; }

private:
	WebRequestPostDataElement::Type m_type;

	// File
	std::wstring m_wstrFilename;

	// Bytes
	uint8_t *m_pBuffer = nullptr;
	size_t m_pBuffer_n;

	// String based
	std::wstring m_wstrValue;
};

#endif	// !WEB_REQUEST_POST_DATA_ELEMENT_H_
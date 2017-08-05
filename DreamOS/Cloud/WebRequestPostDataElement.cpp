#include "WebRequestPostDataElement.h"

WebRequestPostDataElement::WebRequestPostDataElement() :
	m_type(WebRequestPostDataElement::Type::EMPTY)
{
	// empty
}

/*
WebRequestPostDataElement::WebRequestPostDataElement(std::wstring wstrFilename) :
	m_type(WebRequestPostDataElement::Type::FILE),
	m_wstrFilename(wstrFilename)
{
	// empty
}
*/

WebRequestPostDataElement::WebRequestPostDataElement(uint8_t *pBuffer, size_t pBuffer_n) :
	m_type(WebRequestPostDataElement::Type::BYTES),
	m_pBuffer_n(pBuffer_n),
	m_pBuffer(nullptr)
{
	// TODO: This isn't safe - this should be done through a static or factory method
	// This is an exception right now, so it's not being done - since the value pair
	// is going to be used most of the time
	m_pBuffer = (uint8_t *)malloc(m_pBuffer_n);
	memcpy(m_pBuffer, pBuffer, m_pBuffer_n);
}

WebRequestPostDataElement::WebRequestPostDataElement(const std::wstring &wstrValue) :
	m_type(WebRequestPostDataElement::Type::STRING),
	m_wstrValue(wstrValue)
{
	// empty
}
#include "CEFDOMNode.h"

CEFDOMNode::CEFDOMNode() :
	DOMNode(DOMNode::type::INVALID)
{
	// empty
}

CEFDOMNode::CEFDOMNode(CefRefPtr<CefDOMNode> pCEFDOMNode) :
	DOMNode(GetType(pCEFDOMNode->GetType()))
{
	if (pCEFDOMNode != nullptr) {
		m_fEditable = pCEFDOMNode->IsEditable();
		m_strElementTagName = pCEFDOMNode->GetElementTagName();
		m_strName = pCEFDOMNode->GetName();
		m_strValue = pCEFDOMNode->GetValue();
		m_strNodeTypeAttributeValue = pCEFDOMNode->GetElementAttribute("password");	// This may not be necessary?
	}
}

CEFDOMNode::CEFDOMNode(cef_dom_node_type_t cefDOMNodeType, std::string strName, std::string  strElementTagName, std::string strValue, bool fEditable, std::string strNodeTypeAttributeValue) :
	DOMNode(GetType(cefDOMNodeType)),
	m_strName(strName),
	m_strElementTagName(strElementTagName),
	m_strValue(strValue),
	m_fEditable(fEditable),
	m_strNodeTypeAttributeValue(strNodeTypeAttributeValue)
{
	// empty
}

CEFDOMNode::~CEFDOMNode() {
	// empty
}

std::string CEFDOMNode::GetElementTagName() {
	return m_strElementTagName;
}

std::string CEFDOMNode::GetName() {
	return m_strName;
}

std::string CEFDOMNode::GetValue() {
	return m_strValue;
}

DOMNode::type CEFDOMNode::GetType(cef_dom_node_type_t cefDomNodeType) {
	DOMNode::type domNodeType = DOMNode::type::INVALID;

	switch (cefDomNodeType) {
		case DOM_NODE_TYPE_UNSUPPORTED: domNodeType = DOMNode::type::UNSUPPORTED; break;
		case DOM_NODE_TYPE_ELEMENT: domNodeType = DOMNode::type::ELEMENT; break;
		case DOM_NODE_TYPE_ATTRIBUTE: domNodeType = DOMNode::type::ATTRIBUTE; break;
		case DOM_NODE_TYPE_TEXT: domNodeType = DOMNode::type::TEXT; break;
		case DOM_NODE_TYPE_CDATA_SECTION: domNodeType = DOMNode::type::CDATA_SECTION; break;
		case DOM_NODE_TYPE_PROCESSING_INSTRUCTIONS:domNodeType = DOMNode::type::PROCESSING_INSTRUCTIONS; break;
		case DOM_NODE_TYPE_COMMENT: domNodeType = DOMNode::type::COMMENT; break;
		case DOM_NODE_TYPE_DOCUMENT: domNodeType = DOMNode::type::DOCUMENT; break;
		case DOM_NODE_TYPE_DOCUMENT_TYPE: domNodeType = DOMNode::type::DOCUMENT_TYPE; break;
		case DOM_NODE_TYPE_DOCUMENT_FRAGMENT: domNodeType = DOMNode::type::DOCUMENT_FRAGMENT; break;
		default:domNodeType = DOMNode::type::INVALID; break;
	}

	return domNodeType;
}

bool CEFDOMNode::IsPassword() {
	return m_strNodeTypeAttributeValue == "password";
}
bool CEFDOMNode::IsEditable() {
	return m_fEditable;
}
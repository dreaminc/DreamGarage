#include "CEFDOMNode.h"

CEFDOMNode::CEFDOMNode(CefRefPtr<CefDOMNode> pCEFDOMNode) {
	if (pCEFDOMNode != nullptr) {
		m_fEditable = pCEFDOMNode->IsEditable();
		m_strElementTagName = pCEFDOMNode->GetElementTagName();
		m_strName = pCEFDOMNode->GetName();
		m_strValue = pCEFDOMNode->GetValue();
		m_type = GetType(pCEFDOMNode->GetType());
	}
}

CEFDOMNode::~CEFDOMNode() {
	// empty
}

std::string CEFDOMNode::GetElementTagName() const {
	return m_strElementTagName;
}

std::string CEFDOMNode::GetName() const {
	return m_strName;
}

std::string CEFDOMNode::GetValue() const {
	return m_strValue;
}

DOMNode::type CEFDOMNode::GetType() const {
	return m_type;
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

bool CEFDOMNode::IsEditable() const {
	return m_fEditable;
}
#ifndef CEF_DOM_NODE_H_
#define CEF_DOM_NODE_H_

#include "RESULT/EHM.h"

#include "include/cef_dom.h"
// DREAM OS
// DreamOS/WebBrowser/CEFBrowser/CEFDOMNode.h

#include <string>

#include "WebBrowser/DOMNode.h"

class CEFDOMNode : public DOMNode {
public:
	CEFDOMNode();
	CEFDOMNode(CefRefPtr<CefDOMNode> pCEFDOMNode);
	CEFDOMNode(cef_dom_node_type_t cefDOMNodeType, std::string strName, std::string  strElementTagName, std::string strValue, bool fEditable, std::string strNodeTypeAttributeValue);

	~CEFDOMNode();

public:
	virtual std::string GetElementTagName() override;
	virtual std::string GetName() override;
	virtual std::string GetValue() override;

	static DOMNode::type GetType(cef_dom_node_type_t cefDomNodeType);

	virtual bool IsPassword() override;
	virtual bool IsEditable() override;

private:
	bool m_fEditable = false;
	std::string m_strElementTagName = "";
	std::string m_strName = "";
	std::string m_strValue = "";
	std::string m_strNodeTypeAttributeValue = "";
};


#endif // CEF_DOM_NODE_H_
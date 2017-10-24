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
	CEFDOMNode(CefRefPtr<CefDOMNode> pCEFDOMNode);
	~CEFDOMNode();

public:
	virtual std::string GetElementTagName() const override;
	virtual std::string GetName() const override;
	virtual std::string GetValue() const override;

	virtual DOMNode::type GetType() const override;

	static DOMNode::type GetType(cef_dom_node_type_t cefDomNodeType);

	virtual bool IsEditable() const override;

private:
	bool m_fEditable = false;
	std::string m_strElementTagName = "";
	std::string m_strName = "";
	std::string m_strValue = "";
	DOMNode::type m_type = DOMNode::type::INVALID;
};


#endif // CEF_DOM_NODE_H_
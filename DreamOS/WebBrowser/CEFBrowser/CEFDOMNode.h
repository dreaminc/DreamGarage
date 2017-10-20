#ifndef CEF_DOM_NODE_H_
#define CEF_DOM_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/CEFBrowser/CEFDOMNode.h

#include <string>

#include "WebBrowser/DOMNode.h"

class CEFDOMNode : public DOMNode {
public:
	CEFDOMNode();
	~CEFDOMNode();

public:
	virtual std::string GetElementTagName() override;
	virtual std::string GetName() override;
	virtual std::string GetValue() override;

	virtual DOMNode::type GetType() override;

private:

};


#endif // CEF_DOM_NODE_H_
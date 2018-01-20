#ifndef DOM_NODE_H_
#define DOM_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/DOMNode.h
// A representative DOMNode class to be used within the Web Browser 
// classes 

#include <string>

class DOMNode {
public:
	enum class type {
		UNSUPPORTED,
		ELEMENT,
		ATTRIBUTE,
		TEXT,
		CDATA_SECTION,
		PROCESSING_INSTRUCTIONS,
		COMMENT,
		DOCUMENT,
		DOCUMENT_TYPE,
		DOCUMENT_FRAGMENT,
		INVALID
	};

public:
	DOMNode(DOMNode::type domNodeType) :
		m_type(domNodeType)
	{
		// empty
	}

	~DOMNode() {
		// empty
	}

public:
	virtual std::string GetElementTagName() = 0;
	virtual std::string GetName() = 0;
	virtual std::string GetValue() = 0;
	virtual bool IsEditable() = 0;
	virtual bool IsPassword() = 0;

	DOMNode::type GetType() {
		return m_type;
	}

private:
	type m_type;
};


#endif // DOM_NODE_H_
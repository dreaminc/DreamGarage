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
	virtual std::string GetElementTagName() = 0;
	virtual std::string GetName() = 0;
	virtual std::string GetValue() = 0;

	virtual DOMNode::type GetType() = 0;
};


#endif // DOM_NODE_H_
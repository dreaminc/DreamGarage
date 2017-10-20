#ifndef DOM_DOCUMENT_H_
#define DOM_DOCUMENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/DOMDocument.h
// A representative DOM document class

#include <memory>
#include <string>

class DOMNode;

class DOMDocument {
public:
	virtual std::shared_ptr<DOMNode> GetFocusedDOMNode() = 0;
	virtual std::shared_ptr<DOMNode> GetElementById(std::string strElementId) = 0;

	virtual std::string GetDocumentTitle() = 0;
	virtual std::string GetBaseURL() = 0;
	virtual std::string GetCompleteURL() = 0;
};


#endif // DOM_DOCUMENT_H_
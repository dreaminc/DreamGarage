#ifndef CEF_DOM_DOCUMENT_H_
#define CEF_DOM_DOCUMENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/DOMDocument.h
// A representative DOM document class

#include <memory>
#include <string>

#include "WebBrowser/DOMDocument.h"

class DOMNode;

class CEFDOMDocument : public DOMDocument {
public:
	CEFDOMDocument();
	~CEFDOMDocument();

public:
	virtual std::shared_ptr<DOMNode> GetFocusedDOMNode() override;
	virtual std::shared_ptr<DOMNode> GetElementById(std::string strElementId) override;

	virtual std::string GetDocumentTitle() override;
	virtual std::string GetBaseURL() override;
	virtual std::string GetCompleteURL() override;

private:

};


#endif // CEF_DOM_DOCUMENT_H_
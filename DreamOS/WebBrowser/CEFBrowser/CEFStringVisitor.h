#ifndef CEF_STRING_VISITOR_H
#define CEF_STRING_VISITOR_H

#include "RESULT/EHM.h"
#include "CEFStringVisitor.h"
#include <regex>
#include "include\cef_string_visitor.h"

///
// Implement this interface to receive string values asynchronously.
///
/*--cef(source=client)--*/
class CEFStringVisitor : 
	public CefStringVisitor 
{

public:
	///
	// Method that will be executed.
	///
	/*--cef(optional_param=string)--*/
	virtual void Visit(const CefString& string) override {
		std::string strHTMLSource = string;
		std::smatch regexMatch;
		std::regex e("<title>(.*?)</title>");

		//std::regex_match(strHTMLSource, regexMatch, e, std::regex_constants::match_default);
		std::regex_search(strHTMLSource, regexMatch, e);		
		std::string strTitle = regexMatch[1];

		pBrowserControllerObserver->SetTitle(strTitle);
	}

	RESULT SetBrowserControllerObserver(WebBrowserController::observer* pWebBrowserControllerObserver) {
		pBrowserControllerObserver = pWebBrowserControllerObserver;
		return R_PASS;
	}

private:
	WebBrowserController::observer* pBrowserControllerObserver = nullptr;
	IMPLEMENT_REFCOUNTING(CEFStringVisitor);
};

#endif  //CEF_STRING_VISITOR_H_

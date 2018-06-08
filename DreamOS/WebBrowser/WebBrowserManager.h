#ifndef WEB_BROWSER_MANAGER_H_
#define WEB_BROWSER_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/WebBrowserManager.h

#include <list>
#include <string>
#include <memory>

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

class WebBrowserController;
class CEFApp;

class WebBrowserManager : public valid {
public:
	WebBrowserManager();
	~WebBrowserManager();

	virtual RESULT Initialize() = 0;
	virtual RESULT Update() = 0;
	virtual RESULT Shutdown() { return R_NOT_IMPLEMENTED; }


	virtual std::shared_ptr<WebBrowserController> MakeNewBrowser(int width, int height, const std::string& strURL) = 0;

	std::shared_ptr<WebBrowserController> CreateNewBrowser(int width, int height, const std::string& strURL);
	std::shared_ptr<WebBrowserController> GetBrowser(const std::string& strID);
	
	RESULT RemoveBrowser(std::shared_ptr<WebBrowserController> pWebBrowserController);

protected:
	RESULT ClearAllBrowserControllers();

	//void SetKeyFocus(const std::string& id);
	//void OnKey(unsigned int scanCode, char16_t chr);

protected:
	std::list<std::shared_ptr<WebBrowserController>> m_webBrowserControllers;
};


#endif	// ! WEB_BROWSER_MANAGER_H_
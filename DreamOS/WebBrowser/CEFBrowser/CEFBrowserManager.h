#ifndef CEF_BROWSER_MANAGER_H_
#define CEF_BROWSER_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/CEFBrowser/CEFBrowserManager.h
// Base type for texture

#include <string>
#include "Primitives/composite.h"

class CEFBrowserController;
class CEFBrowserService;

class CEFBrowserManager {
public:
	RESULT Initialize(composite* pComposite);
	void Update();

	std::string CreateNewBrowser(unsigned int width, unsigned int height, const std::string& url);
	CEFBrowserController* GetBrowser(const std::string& strID);

	void SetKeyFocus(const std::string& id);
	void OnKey(unsigned int scanCode, char16_t chr);

private:
	composite*	m_pComposite;

	std::unique_ptr<CEFBrowserService> m_pCEFBrowserService = nullptr;

	struct BrowserObject {
		CEFBrowserController*		pCEFBrowserController;
		std::shared_ptr<texture>	pTexture;
		std::shared_ptr<quad>		pQuad;
	};

	// std::string indicates the id of the browser
	std::map<std::string, BrowserObject> m_Browsers;

	CEFBrowserController* m_browserInKeyFocus = nullptr;
};


#endif	// ! CEF_BROWSER_MANAGER_H_
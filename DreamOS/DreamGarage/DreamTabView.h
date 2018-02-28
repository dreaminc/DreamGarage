#ifndef DREAM_TAB_VIEW_H_
#define DREAM_TAB_VIEW_H_

#include "DreamApp.h"
#include "DreamOS.h"

class DreamUserControlArea;
class DreamBrowser;

class UIButton;
class UIView;
class quad;

#include <vector>
#include <map>

#define TAB_BORDER_WIDTH 0.2962f;
#define TAB_BORDER_HEIGHT 0.675269f;
#define TAB_WIDTH 0.2640f;
#define TAB_HEIGHT 0.148387f;

class DreamTabView : public DreamApp<DreamTabView> 
{
	friend class DreamAppManager;
	friend class DreamUserControlArea;

public:
	DreamTabView(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamTabView();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr);
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr);
	virtual RESULT Update(void *pContext = nullptr);
	virtual RESULT Shutdown(void *pContext = nullptr);

	RESULT InitializeWithParent(DreamUserControlArea *pParent);

protected:
	static DreamTabView* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:

	// called when a new piece of content is opened, 
	// all existing tabs shift down and a new tab is added at the top
	RESULT AddBrowser(std::shared_ptr<DreamBrowser> pBrowser);

	// called when close is pressed on the control bar,
	// the top tab is returned after the remaining tabs shift up 
	std::shared_ptr<DreamBrowser> RemoveBrowser();

	RESULT SelectTab(UIButton *pButtonContext, void *pContext);

	RESULT UpdateBrowserTexture(std::shared_ptr<DreamBrowser> pBrowser);

public:
	bool CanPressButton(int dirtyIndex);
	RESULT ClearMalletFlag(int index);

private:
	DreamUserControlArea* m_pParentApp = nullptr;

	std::shared_ptr<quad> m_pBackgroundQuad = nullptr;

	std::map<std::shared_ptr<DreamBrowser>, std::shared_ptr<UIButton>> m_appToTabMap;

	std::shared_ptr<UIView> m_pView = nullptr;
	std::vector<std::shared_ptr<UIButton>> m_tabButtons;
	std::vector<std::shared_ptr<DreamBrowser>> m_browsers;

private:
	const wchar_t *k_wszTabBackground = L"control-view-list-background.png";
	texture *m_pBackgroundTexture = nullptr;

	float m_borderWidth = TAB_BORDER_WIDTH;
	float m_borderHeight = TAB_BORDER_HEIGHT;
	float m_tabWidth = TAB_WIDTH;
	float m_tabHeight = TAB_HEIGHT;

	point m_ptMostRecent;

	bool m_fCanPressButton[2];
};

#endif // ! DREAM_TAB_VIEW_H_
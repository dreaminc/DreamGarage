#ifndef DREAM_TAB_VIEW_H_
#define DREAM_TAB_VIEW_H_

#include "DreamApp.h"
#include "DreamOS.h"

class DreamUserControlArea;
class DreamContentSource;
class DreamBrowser;

class UIFlatScrollView;
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

public:
	float GetBorderWidth();
	float GetBorderHeight();

	RESULT SetScrollFlag(bool fCanScroll, int index);

protected:
	static DreamTabView* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:

	// called when a new piece of content is opened, 
	// all existing tabs shift down and a new tab is added at the top
	RESULT AddContent(std::shared_ptr<DreamContentSource> pContent);

	// called when close is pressed on the control bar,
	// the top tab is returned after the remaining tabs shift up 
	std::shared_ptr<DreamContentSource> RemoveContent();

	RESULT SelectTab(UIButton *pButtonContext, void *pContext);
	RESULT SelectByContent(std::shared_ptr<DreamContentSource> pContent);

	RESULT UpdateContentTexture(std::shared_ptr<DreamContentSource> pContent);

//	std::vector<std::shared_ptr<UIButton>> GetTabButtons();

private:
	std::shared_ptr<UIButton> CreateTab();

// Animations
public:
	RESULT Hide();
	RESULT Show();

private:
	RESULT HideTab(UIButton *pTabButton);
	RESULT ShowTab(UIButton *pTabButton);

	RESULT TranslateTabDown(DimObj *pTabButton);
	RESULT TranslateTabUp(DimObj *pTabButton);
	
private:
	DreamUserControlArea* m_pParentApp = nullptr;

	std::shared_ptr<quad> m_pBackgroundQuad = nullptr;
	std::shared_ptr<quad> m_pRenderQuad = nullptr;

	std::map<std::shared_ptr<DreamContentSource>, std::shared_ptr<UIButton>> m_appToTabMap;

	std::shared_ptr<UIView> m_pView = nullptr;
	std::vector<std::shared_ptr<UIButton>> m_tabButtons;
	std::vector<std::shared_ptr<DreamContentSource>> m_sources;

	std::shared_ptr<UIButton> m_pTabPendingRemoval = nullptr;

	//TODO: hopefully temporary
	std::shared_ptr<UIView> m_pTabView = nullptr;

	std::shared_ptr<UIFlatScrollView> m_pScrollView = nullptr;

private:
	const wchar_t *k_wszTabBackground = L"control-view-list-background.png";
	texture *m_pBackgroundTexture = nullptr;

	float m_borderWidth = TAB_BORDER_WIDTH;
	float m_borderHeight = TAB_BORDER_HEIGHT;
	float m_tabWidth = TAB_WIDTH;
	float m_tabHeight = TAB_HEIGHT;

	bool m_fForceContentFocus = false;

	point m_ptMostRecent;

	// TODO: could be bugs based on this
	bool m_fIsAnimating = false;
};

#endif // ! DREAM_TAB_VIEW_H_
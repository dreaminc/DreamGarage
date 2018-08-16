#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "DreamApp.h"
#include "DreamAppHandle.h"

#include "UI/UIEvent.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "Cloud/Menu/MenuController.h"
#include "Cloud/Menu/MenuNode.h"

#include "Primitives/Subscriber.h"

#include "DreamUserApp.h"
#include "UI/UISpatialScrollView.h"

#include <functional>
#include <stack>
#include <queue>

class DreamUserControlArea;
class UIMallet;
class UIView;

class CloudController;
class EnvironmentControllerProxy;
class HTTPControllerProxy;
class UserControllerProxy;

class font;
class texture;
class UIButton;

class UIStageProgram;

#define KEYBOARD_OFFSET -0.07f
#define SHOW_MENU_HEIGHT -0.5f
#define SHOW_MENU_DEPTH 1.4f
#define MENU_ANIMATION_DURATION 0.1f

#define ACTUATION_DEPTH 0.055f

enum class MenuState {
	NONE,
	ANIMATING
};

enum class MenuLevel {
	ROOT,
	OPEN,
	INVALID
};

class DreamUIBar :	public DreamApp<DreamUIBar>, 
					public DreamUserObserver,
					public MenuController::observer, 
					public UISpatialScrollViewObserver,
					public Subscriber<UIEvent>
{

	friend class DreamAppManager;
	friend class DreamUserControlArea;
	friend class MultiContentTestSuite;

public:
	DreamUIBar(DreamOS *pDreamOS, void *pContext = nullptr);

	RESULT SetFont(const std::wstring& strFont);

	~DreamUIBar();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual DreamAppHandle* GetAppHandle() override;

	// ScrollViewObserver
	virtual RESULT GetNextPageItems() override;

	// Animation Callbacks
	RESULT UpdateMenu(void *pContext);

	// Animations
	RESULT HideApp();
	RESULT ShowApp();

	RESULT SelectMenuItem(UIButton *pPushButton = nullptr, std::function<RESULT(void*)> fnStartCallback = nullptr, std::function<RESULT(void*)> fnEndCallback = nullptr);

	RESULT HandleTouchStart(UIButton* pButtonContext, void* pContext);
	RESULT HandleTouchMove(void* pContext);
	RESULT HandleTouchEnd(void* pContext);

	RESULT MakeMenuItems();
	RESULT RequestMenuItemTexture();
	RESULT ProcessDownloadMenuItemTexture();

	RESULT PopPath();
	RESULT ResetAppComposite();
	RESULT ShowMenuLevel(MenuLevel menuLevel, bool fResetComposite = true);
	RESULT HandleEvent(UserObserverEventType type);
	texture *GetOverlayTexture(HAND_TYPE type);

	RESULT RequestIconFile(std::shared_ptr<MenuNode> pMenuNode);

	RESULT HandleSelect(UIButton* pButtonContext, void* pContext);

	RESULT HandleOnFileResponse(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext);

	RESULT SetMenuStateAnimated(void *pContext);
	RESULT ClearMenuState(void* pContext);
	RESULT ClearMenuWaitingFlag();

	RESULT RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback);

	std::map<InteractionEventType, std::function<RESULT(void*)>> m_callbacks;


// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);
	std::vector<std::string> GetStringHeaders();

// UIEvent
	RESULT Notify(UIEvent *pEvent);

	RESULT SetUIStageProgram(UIStageProgram *pUIStageProgram);
	RESULT InitializeWithParent(DreamUserControlArea *pParentApp);
	RESULT UpdateWidth(float totalWidth);
	bool IsEmpty();

protected:
	static DreamUIBar* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:

	std::shared_ptr<UIView> m_pView = nullptr; // not used for anything yet, but would be used for other UI elements
	std::shared_ptr<UISpatialScrollView> m_pScrollView = nullptr;

	//Cloud member variables
	CloudController *m_pCloudController = nullptr;
	MenuControllerProxy *m_pMenuControllerProxy = nullptr;
	EnvironmentControllerProxy *m_pEnvironmentControllerProxy = nullptr;
	HTTPControllerProxy *m_pHTTPControllerProxy = nullptr;
	UserControllerProxy *m_pUserControllerProxy = nullptr;

	int m_concurrentRequestLimit = 1;
	int m_loadedMenuItems = 0;
	std::shared_ptr<MenuNode> m_pMenuNode = nullptr;
	std::vector<std::pair<MenuNode*, std::shared_ptr<std::vector<uint8_t>>>> m_downloadQueue;
	
	std::queue<std::shared_ptr<MenuNode>> m_requestQueue;
	bool m_fRequestTexture = false;

	std::stack<std::shared_ptr<MenuNode>> m_pathStack = {};
	std::shared_ptr<MenuNode> m_pRootMenuNode; 
	std::shared_ptr<MenuNode> m_pOpenMenuNode; 

	std::string m_strIconTitle = "icon_title";

	std::shared_ptr<texture> m_pDefaultThumbnail = nullptr;
	std::shared_ptr<texture> m_pDefaultIcon = nullptr;
	std::shared_ptr<texture> m_pMenuIcon = nullptr;
	std::shared_ptr<texture> m_pOpenIcon = nullptr;
	std::shared_ptr<texture> m_pMenuItemBg = nullptr;
	texture* m_pOverlayLeft = nullptr;
	texture* m_pOverlayRight = nullptr;
	texture* m_pPendingIconTexture = nullptr;
	std::shared_ptr<std::vector<uint8_t>> m_pPendingIconTextureBuffer;

	std::shared_ptr<font> m_pFont;

	quaternion m_qMenuOrientation;
	point m_ptMenuShowOffset = point(0.0f, SHOW_MENU_HEIGHT, SHOW_MENU_DEPTH);
	float m_menuHeight = MENU_HEIGHT;
	float m_keyboardOffset = KEYBOARD_OFFSET;
	float m_menuDepth = MENU_DEPTH;
	float m_animationDuration = MENU_ANIMATION_DURATION;
	float m_actuationDepth = ACTUATION_DEPTH;

	bool m_fWaitingForMenuResponse = false;
	bool m_fAddNewMenuItems = false;

	MenuState m_menuState = MenuState::NONE;

	UIStageProgram *m_pUIStageProgram = nullptr;

	UID m_keyboardUID;
	UID m_browserUID;
	UID m_userUID;

	DreamUserHandle *m_pUserHandle = nullptr;
	UIKeyboardHandle *m_pKeyboardHandle = nullptr;

	DreamUserControlArea *m_pParentApp = nullptr;
};


#endif // ! DREAM_UI_BAR_H_

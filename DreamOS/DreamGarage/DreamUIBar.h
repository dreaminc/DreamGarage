#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "DreamApp.h"

#include "UI/UIEvent.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "Cloud/Menu/MenuController.h"
#include "Cloud/Menu/MenuNode.h"

#include "Primitives/Subscriber.h"

#include <functional>
#include <stack>
#include <queue>

class UIScrollView;
class UIMallet;
class UIView;

class CloudController;
class EnvironmentControllerProxy;
class HTTPControllerProxy;
class UserControllerProxy;

class font;
class texture;
class UIButton;

enum class MenuState {
	NONE,
	ANIMATING
};

class DreamUIBar :	public DreamApp<DreamUIBar>, 
					public MenuController::observer, 
					public Subscriber<UIEvent>
{

	friend class DreamAppManager;

public:
	DreamUIBar(DreamOS *pDreamOS, void *pContext = nullptr);

	RESULT SetFont(const std::wstring& strFont);

	~DreamUIBar();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	// Animation Callbacks
	RESULT UpdateMenu(void *pContext);

	// Animations
	RESULT HideMenu(std::function<RESULT(void*)> fnStartCallback = nullptr);
	RESULT ShowMenu(std::function<RESULT(void*)> fnStartCallback = nullptr, std::function<RESULT(void*)> fnEndCallback = nullptr);
	RESULT SelectMenuItem(UIButton *pPushButton = nullptr, std::function<RESULT(void*)> fnStartCallback = nullptr, std::function<RESULT(void*)> fnEndCallback = nullptr);

	RESULT HandleTouchStart(void* pContext);
	RESULT HandleTouchMove(void* pContext);
	RESULT HandleTouchEnd(void* pContext);

	RESULT HandleMenuUp(void* pContext);
	RESULT HandleSelect(void* pContext);

	RESULT HandleOnFileResponse(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext);

	RESULT SetMenuStateAnimated(void *pContext);
	RESULT ClearMenuState(void* pContext);

	RESULT RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback);

	std::map<InteractionEventType, std::function<RESULT(void*)>> m_callbacks;

// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);
	std::vector<std::string> GetStringHeaders();

// UIEvent
	RESULT Notify(UIEvent *pEvent);

protected:
	static DreamUIBar* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:

	std::shared_ptr<UIView> m_pView; // not used for anything yet, but would be used for other UI elements
	std::shared_ptr<UIScrollView> m_pScrollView;

	//TODO: Mallets should probably become a system app, like keyboard
	UIMallet *m_pLeftMallet;
	UIMallet *m_pRightMallet;

	//Cloud member variables
	CloudController *m_pCloudController = nullptr;
	MenuControllerProxy *m_pMenuControllerProxy = nullptr;
	EnvironmentControllerProxy *m_pEnvironmentControllerProxy = nullptr;
	HTTPControllerProxy *m_pHTTPControllerProxy = nullptr;
	UserControllerProxy *m_pUserControllerProxy = nullptr;

	std::shared_ptr<MenuNode> m_pMenuNode = nullptr;
	std::vector<std::pair<std::string, std::shared_ptr<std::vector<uint8_t>>>> m_downloadQueue;

	std::stack<std::shared_ptr<MenuNode>> m_pathStack = {};

	std::shared_ptr<texture> m_pDefaultThumbnail;
	std::shared_ptr<texture> m_pDefaultIcon;
	std::shared_ptr<texture> m_pShareIcon;
	std::shared_ptr<texture> m_pMenuItemBg;

	std::shared_ptr<font> m_pFont;

	quaternion m_qMenuOrientation;
	point m_ptMenuShowOffset;

	MenuState m_menuState = MenuState::NONE;
};


#endif // ! DREAM_UI_BAR_H_
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

class UIStageProgram;

#define KEYBOARD_OFFSET -0.07f
#define SHOW_MENU_HEIGHT -0.5f
#define SHOW_MENU_DEPTH 1.4f
#define MENU_ANIMATION_DURATION 0.1f

#define ACTUATION_DEPTH 0.055f

//Projection clipping values
//TODO: optimize these values to reduce error,
// once scrolling snap animation is determined
#define CLIPPING_OFFSET -0.6f
#define PROJECTION_WIDTH 0.575f
#define PROJECTION_HEIGHT 0.25f
#define PROJECTION_NEAR 0.0f
#define PROJECTION_FAR 5.0f
#define PROJECTION_ANGLE 30.0f

enum class MenuState {
	NONE,
	ANIMATING
};

class DreamUIBarHandle : public DreamAppHandle, public DreamUserObserver {
public:
	RESULT SendShowRootMenu();

public:
	virtual RESULT HandleEvent(UserObserverEventType type) = 0;
	virtual texture *GetOverlayTexture(HAND_TYPE type) = 0;

private:
	virtual RESULT ShowRootMenu() = 0;
};

class DreamUIBar :	public DreamApp<DreamUIBar>, 
					public DreamUIBarHandle,
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

	virtual DreamAppHandle* GetAppHandle() override;

	// Animation Callbacks
	RESULT UpdateMenu(void *pContext);

	RESULT UpdateBrowser(std::string strScope, std::string strPath);

	// Animations
	RESULT HideApp();
	RESULT ShowApp();

	RESULT ShowControlView();
	RESULT SendURLToBrowser();

	RESULT SelectMenuItem(UIButton *pPushButton = nullptr, std::function<RESULT(void*)> fnStartCallback = nullptr, std::function<RESULT(void*)> fnEndCallback = nullptr);

	RESULT HandleTouchStart(UIButton* pButtonContext, void* pContext);
	RESULT HandleTouchMove(void* pContext);
	RESULT HandleTouchEnd(void* pContext);

	RESULT PopPath();
	RESULT RequestMenu();
	RESULT ResetAppComposite();
	virtual RESULT ShowRootMenu() override;
	virtual RESULT HandleEvent(UserObserverEventType type) override;
	virtual texture *GetOverlayTexture(HAND_TYPE type) override;

	RESULT RequestIconFile(std::shared_ptr<MenuNode> pMenuNode);

	RESULT HandleSelect(UIButton* pButtonContext, void* pContext);

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

	RESULT SetUIStageProgram(UIStageProgram *pUIStageProgram);

protected:
	static DreamUIBar* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:

	std::shared_ptr<UIView> m_pView; // not used for anything yet, but would be used for other UI elements
	std::shared_ptr<UIScrollView> m_pScrollView;

	//Cloud member variables
	CloudController *m_pCloudController = nullptr;
	MenuControllerProxy *m_pMenuControllerProxy = nullptr;
	EnvironmentControllerProxy *m_pEnvironmentControllerProxy = nullptr;
	HTTPControllerProxy *m_pHTTPControllerProxy = nullptr;
	UserControllerProxy *m_pUserControllerProxy = nullptr;

	std::shared_ptr<MenuNode> m_pMenuNode = nullptr;
	std::vector<std::pair<std::string, std::shared_ptr<std::vector<uint8_t>>>> m_downloadQueue;

	std::stack<std::shared_ptr<MenuNode>> m_pathStack = {};

	std::shared_ptr<texture> m_pDefaultThumbnail = nullptr;
	std::shared_ptr<texture> m_pDefaultIcon = nullptr;
	std::shared_ptr<texture> m_pShareIcon = nullptr;
	std::shared_ptr<texture> m_pMenuItemBg = nullptr;
	texture* m_pOverlayLeft = nullptr;
	texture* m_pOverlayRight = nullptr;
	texture* m_pPendingIconTexture = nullptr;

	std::shared_ptr<font> m_pFont;

	quaternion m_qMenuOrientation;
	point m_ptMenuShowOffset = point(0.0f, SHOW_MENU_HEIGHT, SHOW_MENU_DEPTH);
	float m_menuHeight = MENU_HEIGHT;
	float m_keyboardOffset = KEYBOARD_OFFSET;
	float m_menuDepth = MENU_DEPTH;
	float m_animationDuration = MENU_ANIMATION_DURATION;
	float m_actuationDepth = ACTUATION_DEPTH;

	float m_projectionWidth = PROJECTION_WIDTH;
	float m_projectionHeight = PROJECTION_HEIGHT;
	float m_projectionNearPlane = PROJECTION_NEAR;
	float m_projectionFarPlane = PROJECTION_FAR;
	float m_projectionAngle = PROJECTION_ANGLE;

	MenuState m_menuState = MenuState::NONE;

	UIStageProgram *m_pUIStageProgram = nullptr;

	UID m_keyboardUID;
	UID m_browserUID;
	UID m_userUID;

	DreamUserHandle *m_pUserHandle = nullptr;
	UIKeyboardHandle *m_pKeyboardHandle = nullptr;
};


#endif // ! DREAM_UI_BAR_H_

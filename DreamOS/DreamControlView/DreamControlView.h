#ifndef DREAM_CONTROL_VIEW_H_
#define DREAM_CONTROL_VIEW_H_

#include "RESULT/EHM.h"
#include "DreamApp.h"
#include "DreamAppHandle.h"

#include "Sense/SenseController.h"
#include "InteractionEngine/InteractionObjectEvent.h"
#include "WebBrowser/WebBrowserController.h"
#include "DreamUserApp.h"
#include "UIControlBar.h"

#include "Primitives/Subscriber.h"
#include <functional>
#include <stack>

#define BROWSER_SCROLL_CONSTANT 10.0f

#define VIEW_WIDTH 0.60f // This is 1080p scaled down (2000x) - may want to use browser aspect ratio though
#define VIEW_HEIGHT (VIEW_WIDTH * 9.0f / 16.0f) //0.3375f
#define VIEW_ANGLE 32.0f
#define VIEW_POS_DEPTH 0.1f	
#define VIEW_POS_HEIGHT -0.2f

#define TYPING_ANGLE (M_PI / 2.0f)

#define KEYBOARD_ANIMATION_DURATION_SECONDS 0.1f

#define SQUARED_DRAG_THRESHOLD 0.001f;

class quad; 
class sphere;
class UIView;
class UIMallet;
class UIButton;
class texture;
class DreamBrowserHandle;

class DreamControlViewHandle : public DreamAppHandle, public DreamUserObserver {
public:
	RESULT SetControlViewTexture(std::shared_ptr<texture> pBrowserTexture);
	RESULT SendContentType(std::string strContentType);
	RESULT ShowApp();
	RESULT HideApp();
	RESULT DismissApp();
	RESULT SendURLtoBrowser();
	RESULT SendBrowserScopeAndPath(std::string strScope, std::string strPath);
	bool IsAppVisible();
	RESULT SendURLText(std::string strURL);

public:
	//User Observer
	virtual RESULT HandleEvent(UserObserverEventType type) = 0;
	virtual texture *GetOverlayTexture(HAND_TYPE type) = 0;

	virtual RESULT HandleKeyboardUp(std::string strTextField, point ptTextBox) = 0;

private:
	virtual RESULT SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) = 0;
	virtual RESULT SetContentType(std::string strContentType) = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Hide() = 0;
	virtual RESULT Dismiss() = 0;
	virtual RESULT SendURL() = 0;
	virtual RESULT SetBrowserScopeAndPath(std::string strScope, std::string strPath) = 0;
	virtual bool IsVisible() = 0;
	virtual RESULT SetURLText(std::string strURL) = 0;
};

class DreamControlView : public DreamApp<DreamControlView>, 
						 public DreamControlViewHandle,
						 public Subscriber<InteractionObjectEvent>,
						 public Subscriber<SenseControllerEvent>, 
						 public ControlBarObserver {
	friend class DreamAppManager;

public:
	DreamControlView(DreamOS *pDreamOS, void *pContext = nullptr);

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual RESULT SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) override;
	virtual RESULT SetContentType(std::string strContentType) override;

	virtual DreamAppHandle* GetAppHandle() override;

	virtual RESULT Notify(InteractionObjectEvent *pInteractionEvent) override;
	virtual RESULT Notify(SenseControllerEvent *pEvent) override;

	virtual RESULT HandleEvent(UserObserverEventType type) override;
	virtual texture *GetOverlayTexture(HAND_TYPE type);

	virtual RESULT HandleKeyboardUp(std::string strTextField, point ptTextBox) override;
	virtual RESULT HandleKeyboardDown();
	virtual RESULT SendURL() override;
	virtual RESULT SetBrowserScopeAndPath(std::string strScope, std::string strPath) override;

	RESULT ResetAppComposite();

private:
	RESULT UpdateWithMallet(UIMallet *pMallet, bool &fMalletDitry, bool &fControlBarDirty, bool &fMouseDown, HAND_TYPE handType);

	RESULT ShowKeyboard();
	RESULT HideKeyboard();

protected:
	static DreamControlView *SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Animations
private:

	virtual RESULT Show() override;
	virtual RESULT Hide() override;
	RESULT ShowView();
	RESULT HideView();
	virtual RESULT Dismiss() override;

	virtual bool IsVisible() override;

	//	manually checks the objects that could be animating,
	//	to avoid problems with animations and updates
	bool IsAnimating();

// ControlBarObserver 
private:
	bool CanPressButton(UIButton* pButtonContext);
	RESULT SetIsMinimizedFlag(bool fIsMinimized);

	virtual RESULT HandleClosePressed(UIButton* pButtonContext, void* pContext) override;
	virtual RESULT HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) override;
	virtual RESULT HandleBackPressed(UIButton* pButtonContext, void* pContext) override;
	virtual RESULT HandleForwardPressed(UIButton* pButtonContext, void* pContext) override;
	virtual RESULT HandleURLPressed(UIButton* pButtonContext, void* pContext) override;

	virtual RESULT SetURLText(std::string strURL) override;

// View Context
public:
	std::shared_ptr<quad> GetViewQuad();
	RESULT SetKeyboardAnimationDuration(float animationDuration);
	WebBrowserPoint GetRelativePointofContact(point ptContact);

public:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

	//TODO: potentially move these into user app or dream app
	const wchar_t *k_wszOculusOverlayLeft = L"left-controller-overlay-active.png";
	const wchar_t *k_wszOculusOverlayRight = L"right-controller-overlay-active.png";
	const wchar_t *k_wszViveOverlayLeft = L"vive-controller-overlay-left-active.png";
	const wchar_t *k_wszViveOverlayRight = L"vive-controller-overlay-right-active.png";

private:
	std::shared_ptr<UIView> m_pView = nullptr;
	std::shared_ptr<quad> m_pViewQuad = nullptr;
	std::shared_ptr<texture> m_pViewTexture = nullptr;
	texture* m_pLoadingScreenTexture = nullptr;
	std::shared_ptr<UIControlBar> m_pControlBar = nullptr;

	std::string m_strURL = "";

	texture* m_pOverlayLeft;
	texture* m_pOverlayRight;

	DreamBrowserHandle* m_pBrowserHandle = nullptr;
	DreamUserHandle *m_pUserHandle = nullptr;
	UIKeyboardHandle *m_pKeyboardHandle = nullptr;

	UID m_browserUID;
	UID m_userUID;	

	bool m_fMouseDown[2];
	point m_ptClick;

	// true while the keyboard is shown for sharing a new URL
	bool m_fIsShareURL = false;
	bool m_fIsMinimized = false;

	//TODO: the physics in the keyboard surface uses dirty with the mallets to determine whether a hit 
	//		should be registered.  This doesn't work correctly when there are multiple surfaces
	//		being used at the same time
	dirty m_fMalletDirty[2];
	dirty m_fCanPressButton[2];

	float m_dragThresholdSquared = SQUARED_DRAG_THRESHOLD;

	float m_hiddenScale; 
	float m_visibleScale;
	float m_keyboardAnimationDuration;	// In seconds (direct plug into PushAnimationItem)

	WebBrowserPoint m_ptLMalletPointing;
	WebBrowserPoint m_ptRMalletPointing;
	WebBrowserPoint m_ptLastEvent;
	point m_ptHiddenPosition;
	point m_ptVisiblePosition;	
	quaternion m_qViewQuadOrientation;
	std::string m_strText;

	BarType m_currentControlBarType;
};

#endif // ! DREAM_CONTROL_VIEW_H_

#ifndef DREAM_CONTROL_VIEW_H_
#define DREAM_CONTROL_VIEW_H_

#include "RESULT/EHM.h"
#include "DreamApp.h"
#include "DreamAppHandle.h"

#include "Sense/SenseController.h"
#include "InteractionEngine/InteractionObjectEvent.h"
#include "WebBrowser/WebBrowserController.h"
#include "DreamUserApp.h"

#include "DreamGarage/UICommon.h"

#include "DreamUserControlArea/DreamUserControlArea.h"

#include "Primitives/Subscriber.h"
#include <functional>
#include <stack>

#define BROWSER_SCROLL_CONSTANT 10.0f

#define VIEW_WIDTH 0.60f // This is 1080p scaled down (2000x) - may want to use browser aspect ratio though
#define VIEW_HEIGHT (VIEW_WIDTH * 9.0f / 16.0f) //0.3375f

#define TYPING_ANGLE (42.0f * M_PI / 180.0f)
#define KEYBOARD_ANIMATION_DURATION_SECONDS 0.1f
#define SQUARED_DRAG_THRESHOLD 0.001f;

class quad; 
class sphere;
class UIView;
class UIMallet;
class UIButton;
class texture;

class DreamControlView : public DreamApp<DreamControlView>, 
						 public DreamUserObserver {
	friend class DreamAppManager;
	friend class DreamUserControlArea;
	friend class DreamSettingsApp;
	friend class DreamFormApp;

public:
	DreamControlView(DreamOS *pDreamOS, void *pContext = nullptr);

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamControlView *SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	RESULT InitializeWithUserApp(std::shared_ptr<DreamUserApp> pParent);
	
// DreamAppHandle
public:
	RESULT SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture);

	virtual RESULT HandleEvent(UserObserverEventType type) override;
	virtual texture *GetOverlayTexture(HAND_TYPE type);

	RESULT HandleKeyboardUp();
	virtual RESULT HandleKeyboardDown();

	RESULT ResetAppComposite();

private:
	RESULT ShowKeyboard();
	RESULT HideKeyboard();


// Animations
public:
	RESULT Show();
	RESULT Hide();

private:
	RESULT ShowView();
	RESULT HideView();
	RESULT Dismiss();

	bool IsVisible();

	//	manually checks the objects that could be animating,
	//	to avoid problems with animations and updates
	bool IsAnimating();

	RESULT SetURLText(std::string strURL);

// View Context
public:
	std::shared_ptr<quad> GetViewQuad();
	std::shared_ptr<UISurface> GetViewSurface();

	float GetBackgroundWidth();
	RESULT SetKeyboardAnimationDuration(float animationDuration);
	point GetLastEvent();

public:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

	//TODO: potentially move these into user app or dream app
	const wchar_t *k_wszOculusOverlayLeft = L"left-controller-overlay-active.png";
	const wchar_t *k_wszOculusOverlayRight = L"right-controller-overlay-active.png";
	const wchar_t *k_wszViveOverlayLeft = L"vive-controller-overlay-left-active.png";
	const wchar_t *k_wszViveOverlayRight = L"vive-controller-overlay-right-active.png";

private:
	
	std::shared_ptr<UIView> m_pView = nullptr;

	std::shared_ptr<UISurface> m_pUISurface = nullptr;
	std::shared_ptr<quad> m_pViewQuad = nullptr;
	std::shared_ptr<texture> m_pViewTexture = nullptr;

	std::shared_ptr<quad> m_pViewBackground = nullptr;
	texture* m_pBackgroundTexture = nullptr;

	texture* m_pLoadingScreenTexture = nullptr;

	std::string m_strURL = "";

	texture* m_pOverlayLeft;
	texture* m_pOverlayRight;

	bool m_fMouseDown[2];
	bool m_fMouseDrag = false;
	point m_ptClick;

	bool m_fIsMinimized = false;

	//TODO: the physics in the keyboard surface uses dirty with the mallets to determine whether a hit 
	//		should be registered.  This doesn't work correctly when there are multiple surfaces
	//		being used at the same time
	dirty m_fMalletDirty[2];

	float m_dragThresholdSquared = SQUARED_DRAG_THRESHOLD;

	float m_borderWidth = BORDER_WIDTH;
	float m_borderHeight = BORDER_HEIGHT;

	float m_hiddenScale; 
	float m_visibleScale;
	float m_keyboardAnimationDuration;	// In seconds (direct plug into PushAnimationItem)

	point m_ptHiddenPosition;
	point m_ptVisiblePosition;	
	quaternion m_qViewQuadOrientation;
	std::string m_strText;
};

#endif // ! DREAM_CONTROL_VIEW_H_

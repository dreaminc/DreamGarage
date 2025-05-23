#ifndef DREAM_CONTROL_VIEW_H_
#define DREAM_CONTROL_VIEW_H_

#include "core/ehm/EHM.h"

// Dream UI
// dos/src/ui/DreamControlView/UIControlView.h
// TODO: Review this, is this more specific

#include <functional>
#include <stack>

#include "core/types/Subscriber.h"

#include "ui/UICommon.h"
#include "ui/UISurface.h"

#include "os/app/DreamApp.h"
#include "os/app/DreamAppHandle.h"

#include "sense/SenseController.h"

#include "modules/InteractionEngine/InteractionObjectEvent.h"

// TOOD: Should this be here if this is a general UI lib component
#include "WebBrowser/WebBrowserController.h"

// TODO: Obviously a problem
#include "apps/DreamUserApp/DreamUserApp.h"
#include "apps/DreamUserControlAreaApp/DreamUserControlAreaApp.h"

// TODO: Param time
#define BROWSER_SCROLL_CONSTANT 10.0f

#define VIEW_WIDTH 0.60f // This is 1080p scaled down (2000x) - may want to use browser aspect ratio though
#define VIEW_HEIGHT (VIEW_WIDTH * 9.0f / 16.0f) //0.3375f

#define TYPING_ANGLE (42.0f * M_PI / 180.0f)
#define KEYBOARD_ANIMATION_DURATION_SECONDS 0.1f
#define SQUARED_DRAG_THRESHOLD 0.001f;

#define LOCK_PX_WIDTH 97.0f
#define BACKGROUND_PX_WIDTH 1823.0f
#define ADDRESS_PX_HEIGHT 120.0f

class quad; 
class sphere;
class UIView;
class UIButton;
class texture;
class font;
class text;

enum class ContentType {
	FORM,
	DEFAULT,
	INVALID
	// TODO: potentially different layouts for numpad or logins
};

class UIControlView : public UISurface, 
						 public DreamUserObserver {
	friend class DreamUserControlAreaApp;
	friend class MultiContentTestSuite;
	friend class DreamSettingsApp;
	friend class DreamFormApp;

public:
	UIControlView(HALImp *pHALImp, DreamOS *pDreamOS);

public:
	RESULT Initialize();
	RESULT Update();
	
public:
	RESULT InitializeAddressBar(float width);

public:
	virtual RESULT HandleEvent(UserObserverEventType type) override;
	virtual texture *GetOverlayTexture(HAND_TYPE type);

// Animations
public:
	RESULT Show();
	RESULT Hide();

	RESULT HandleKeyboardUp(ContentType type = ContentType::DEFAULT);
	RESULT HandleKeyboardDown();

	RESULT FlipViewUp();
	RESULT FlipViewDown();

private:
	RESULT ShowView();
	RESULT HideView();

	bool IsVisible();

	//	manually checks the objects that could be animating,
	//	to avoid problems with animations and updates
	bool IsAnimating();

// View Context
public:
	float GetBackgroundWidth();
	RESULT SetKeyboardAnimationDuration(float animationDuration);
	RESULT SetViewQuadTexture(texture* pBrowserTexture);

public:
	RESULT SetURLText(std::string strURL);
	RESULT SetSchemeText(std::string strScheme);
	RESULT SetURLSecurity(bool fSecure);

public:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

	//TODO: potentially move these into user app or dream app
	const wchar_t *k_wszOculusOverlayLeft = L"left-controller-overlay-active.png";
	const wchar_t *k_wszOculusOverlayRight = L"right-controller-overlay-active.png";
	const wchar_t *k_wszViveOverlayLeft = L"vive-controller-overlay-left-active.png";
	const wchar_t *k_wszViveOverlayRight = L"vive-controller-overlay-right-active.png";

private:
	std::wstring k_wstrAddressSecure = L"texture/control-view/address-bar-secure.png";
	std::wstring k_wstrAddressInsecure = L"texture/control-view/address-bar-insecure.png";
	std::wstring k_wstrAddressBackground = L"texture/control-view/address-bar-url-background.png";
	
private:
	std::shared_ptr<texture> m_pViewTexture = nullptr;

	std::shared_ptr<quad> m_pViewBackground = nullptr;
	texture* m_pBackgroundTexture = nullptr;

	texture* m_pLoadingScreenTexture = nullptr;

	texture* m_pOverlayLeft = nullptr;
	texture* m_pOverlayRight = nullptr;

	texture *m_pAddressSecureTexture = nullptr;
	texture *m_pAddressInsecureTexture = nullptr;
	texture *m_pAddressBackgroundTexture = nullptr;

	std::shared_ptr<UIView> m_pAddressBar = nullptr;
	std::shared_ptr<quad> m_pAddressSecurityQuad = nullptr;
	std::shared_ptr<quad> m_pAddressBackgroundQuad = nullptr;

	std::shared_ptr<font> m_pFont = nullptr;
	std::shared_ptr<text> m_pAddressText = nullptr;
	std::string m_strCurrentURL;
	std::string m_strCurrentScheme;

	bool m_fMouseDown[2] = {false, false};
	bool m_fMouseDrag = false;
	point m_ptClick;

	bool m_fIsMinimized = false;
	bool m_fUpdateAddressBarText = false;

	//TODO: the physics in the keyboard surface uses dirty with the mallets to determine whether a hit 
	//		should be registered.  This doesn't work correctly when there are multiple surfaces
	//		being used at the same time
	dirty m_fMalletDirty[2];

	float m_dragThresholdSquared = SQUARED_DRAG_THRESHOLD;

	float m_borderWidth = BORDER_WIDTH;
	float m_borderHeight = BORDER_HEIGHT;

	float m_hiddenScale = 0.2f;
	float m_visibleScale = 1.0f;
	float m_keyboardAnimationDuration = KEYBOARD_ANIMATION_DURATION_SECONDS;	// In seconds (direct plug into PushAnimationItem)

	point m_ptHiddenPosition;
	point m_ptVisiblePosition;	
	quaternion m_qViewQuadOrientation;
	std::string m_strText;
};

#endif // ! DREAM_CONTROL_VIEW_H_

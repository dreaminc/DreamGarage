#ifndef DREAM_CONTROL_VIEW_H_
#define DREAM_CONTROL_VIEW_H_

#include "RESULT/EHM.h"
#include "DreamApp.h"
#include "DreamAppHandle.h"

#include "Sense/SenseController.h"
#include "InteractionEngine/InteractionObjectEvent.h"
#include "WebBrowser/WebBrowserController.h"
#include "DreamUserApp.h"

#include "Primitives/Subscriber.h"
#include <functional>
#include <stack>

#define BROWSER_SCROLL_CONSTANT 10.0f

#define CONTROL_VIEWQUAD_WIDTH 0.60f // This is 1080p scaled down (2000x) - may want to use browser aspect ratio though
#define CONTROL_VIEWQUAD_HEIGHT 0.3375f
#define CONTROL_VIEWQUAD_ANGLE 32.0f
#define CONTROL_VIEW_DEPTH -0.4f	// This could be calculated off of MENU_DEPTH as well.
#define CONTROL_VIEW_HEIGHT -0.2f
#define TYPING_ROTATION (M_PI / 2.0f)

#define KEYBOARD_ANIMATION_DURATION_SECONDS 0.1f

class quad; 
class sphere;
class UIView;
class UIMallet;
class UIScrollView;
class texture;
class DreamBrowserHandle;

class DreamControlViewHandle : public DreamAppHandle, public DreamUserObserver {
public:
	RESULT SetControlViewTexture(std::shared_ptr<texture> pBrowserTexture);
	RESULT ShowApp();
	RESULT HideApp();
	RESULT DismissApp();
	RESULT SendURLtoBrowser();
	bool IsAppVisible();

public:
	//User Observer
	virtual RESULT HandleEvent(UserObserverEventType type) = 0;
	virtual texture *GetOverlayTexture(HAND_TYPE type) = 0;

	virtual RESULT HandleKeyboardUp(std::string strTextField, point ptTextBox) = 0;

private:
	virtual RESULT SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Hide() = 0;
	virtual RESULT Dismiss() = 0;
	virtual RESULT SendURL() = 0;
	virtual bool IsVisible() = 0;
};

class DreamControlView : public DreamApp<DreamControlView>, 
						 public DreamControlViewHandle,
						 public Subscriber<InteractionObjectEvent>,
						 public Subscriber<SenseControllerEvent> {
	friend class DreamAppManager;

public:
	DreamControlView(DreamOS *pDreamOS, void *pContext = nullptr);

	enum class state {
		HIDDEN,
		HIDE,
		VISIBLE,
		SHOW,
		TYPING
	};

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual RESULT SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) override;
	virtual DreamAppHandle* GetAppHandle() override;

	virtual RESULT Notify(InteractionObjectEvent *pInteractionEvent) override;
	virtual RESULT Notify(SenseControllerEvent *pEvent) override;

	virtual RESULT HandleEvent(UserObserverEventType type) override;
	virtual texture *GetOverlayTexture(HAND_TYPE type);

	virtual RESULT HandleKeyboardUp(std::string strTextField, point ptTextBox) override;
	virtual RESULT HandleKeyboardDown();
	virtual RESULT SendURL() override;

protected:
	static DreamControlView *SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Animations
private:

	virtual RESULT Show() override;
	virtual RESULT Hide() override;
	virtual RESULT Dismiss() override;

	virtual bool IsVisible() override;

// View Context
public:
	std::shared_ptr<quad> GetViewQuad();
	RESULT SetViewState(DreamControlView::state viewState);
	RESULT SetKeyboardAnimationDuration(float animationDuration);
	WebBrowserPoint GetRelativePointofContact(point ptContact);

private:
	std::shared_ptr<quad> m_pViewQuad = nullptr;
	std::shared_ptr<texture> m_pViewTexture = nullptr;
	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;
	std::shared_ptr<UIView> m_pView = nullptr;

	std::string m_strURL = "";

	texture* m_pOverlayLeft;
	texture* m_pOverlayRight;

	DreamBrowserHandle* m_pBrowserHandle = nullptr;
	DreamUserHandle *m_pUserHandle = nullptr;
	UIKeyboardHandle *m_pKeyboardHandle = nullptr;

	UID m_browserUID;
	UID m_userUID;	

	DreamControlView::state m_viewState;

	bool m_fMouseUp;

	float m_hiddenScale; 
	float m_visibleScale;
	float m_keyboardAnimationDuration;	// In seconds (direct plug into PushAnimationItem)

	WebBrowserPoint m_ptLMalletPointing;
	WebBrowserPoint m_ptRMalletPointing;
	point m_ptHiddenPosition;
	point m_ptVisiblePosition;	
	quaternion m_qViewQuadOrientation;
};

#endif // ! DREAM_CONTROL_VIEW_H_

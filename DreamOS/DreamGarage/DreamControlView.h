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

#define CONTROL_VIEWQUAD_WIDTH 0.96f // This is 1080p scaled down (2000x) - may want to use browser aspect ratio though
#define CONTROL_VIEWQUAD_HEIGHT 0.54f
#define CONTROL_VIEWQUAD_ANGLE (M_PI / 3.0f)
#define CONTROL_VIEW_DEPTH 0.6f
#define CONTROL_VIEW_HEIGHT -0.20f
#define TYPING_ROTATION (M_PI / 2.0f)

#define KEYBOARD_ANIMATION_DURATION_SECONDS 0.1f
class text;
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
	bool IsAppVisible();

public:
	virtual RESULT HandleEvent(UserObserverEventType type) = 0;
	virtual RESULT HandleKeyboardUp(std::string strTextField, point ptTextBox) = 0;

private:
	virtual RESULT SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Hide() = 0;
	virtual bool IsVisible() = 0;
};

class DreamControlView : public DreamApp<DreamControlView>, 
						 public DreamControlViewHandle,
						 public Subscriber<InteractionObjectEvent>,
						 public Subscriber<SenseControllerEvent> {
	friend class DreamAppManager;

public:
	DreamControlView(DreamOS *pDreamOS, void *pContext = nullptr);

	enum class State {
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
	virtual RESULT HandleKeyboardUp(std::string strTextField, point ptTextBox) override;
	virtual RESULT HandleKeyboardDown();

protected:
	static DreamControlView *SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Animations
private:

	virtual RESULT Show() override;
	virtual RESULT Hide() override;

	virtual bool IsVisible() override;

// View Context
public:
	//When update is called the screen texture is updated to the texture of this object
	RESULT SetSharedViewContext();
	std::shared_ptr<quad> GetViewQuad();
	RESULT SetViewState(State state);
	RESULT SetKeyboardAnimationDuration(float animationDuration);
	WebBrowserPoint GetRelativePointofContact(point ptContact);

private:
	std::shared_ptr<quad> m_pViewQuad;
	vector m_vNormal;
	std::shared_ptr<texture> m_pViewTexture;
	std::shared_ptr<UIView> m_pView;
	std::shared_ptr<UIScrollView> m_pScrollView;

	std::string m_strURL = "";

	DreamBrowserHandle* m_pBrowserHandle = nullptr;
	DreamUserHandle *m_pUserHandle = nullptr;
	UIKeyboardHandle *m_pKeyboardHandle = nullptr;

	UID m_browserUID;
	UID m_userUID;
	UID m_keyboardUID;

	State m_viewState;

	float m_hiddenScale; 
	float m_visibleScale;
	float m_keyboardAnimationDuration;	// In seconds (direct plug into PushAnimationItem)

	point m_ptHiddenPosition;
	point m_ptVisiblePosition;
	std::shared_ptr<text> pTextBoxText;
	quaternion m_qViewQuadOrientation;
};

#endif // ! DREAM_CONTROL_VIEW_H_

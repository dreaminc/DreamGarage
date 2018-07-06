#ifndef DREAM_USER_CONTROL_AREA_H_
#define DREAM_USER_CONTROL_AREA_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamGarage/DreamControlBar.h"
#include "DreamGarage/DreamBrowser.h"
#include "Primitives/Subscriber.h"

#include <vector>
#include <map>

class DreamUserApp;
class DreamControlView;
class DreamUIBar;
class DreamTabView;
class DreamContentSource;
class DreamDesktopApp;

class CEFBrowserManager;
struct WebBrowserPoint;
class EnvironmentAsset;

class AudioPacket;

struct InteractionObjectEvent;

class UIStageProgram;

class quad;

#define VIEW_POS_DEPTH 0.1f	
#define VIEW_POS_HEIGHT -0.2f

#define ANIMATION_SCALE 0.1f

#define TITLE_DESKTOP "Desktop"
#define TITLE_WEBSITE "website"
 
class DreamUserControlArea : public DreamApp<DreamUserControlArea>, 
	public Subscriber<InteractionObjectEvent>,
	public Subscriber<UIEvent>,
	public DreamBrowserObserver {

	friend class DreamAppManager;
	friend class MultiContentTestSuite;
	friend class DreamOSTestSuite;
	friend class DreamUIBar;

public:
	DreamUserControlArea(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamUserControlArea();

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamUserControlArea* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Expose size variables
public:
	// Sizes relative to control view width
	float GetSpacingSize();
	float GetBaseWidth();
	float GetBaseHeight();
	//point GetBaseCenter();
	float GetViewAngle();


	// 
	RESULT SetViewHeight(float height);
	// Set Base width and update everything else
	RESULT ScaleViewWidth(float scale);
	float GetViewScale();

	// absolute positions
	point GetCenter();
	float GetCenterOffset();
	float GetTotalWidth();
	float GetTotalHeight();

// Animations
public:
	RESULT Show();
	RESULT Hide();

// DreamControlView
public:
	int GetWidth();
	int GetHeight();

	RESULT OnClick(point ptContact, bool fMouseDown);
	RESULT OnMouseMove(point mousePoint);
	RESULT OnKeyPress(char chkey, bool fKeyDown);
	RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint);

	//TODO: Set Scope/Path should be removed once DreamUIBar follows "open" implementation
	RESULT SetScope(std::string strScope);
	RESULT SetPath(std::string strPath);
	RESULT RequestOpenAsset(std::string strScope, std::string strPath, std::string strTitle);
	RESULT CreateBrowserSource();
	RESULT SetActiveBrowserURI();
	RESULT HideWebsiteTyping();

// DreamControlBar
public:
	RESULT HandleControlBarEvent(ControlEventType type);

	bool CanPressButton(UIButton *pButtonContext);

// DreamTabView
public:
	std::shared_ptr<DreamContentSource> GetActiveSource();
	RESULT SetActiveSource(std::shared_ptr<DreamContentSource> pNewContent);

// DreamBrowser
public:

	virtual RESULT UpdateContentSourceTexture(std::shared_ptr<texture> pTexture, DreamContentSource* pContext) override;
	virtual RESULT UpdateControlBarText(std::string& strTitle) override;
	virtual RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override;

	//TODO: present keyboard from browser::OnNodeFocusChanged
	virtual RESULT HandleNodeFocusChanged(std::string strInitial) override;
	bool IsContentVisible();

	virtual RESULT HandleDreamFormSuccess() override;
	virtual RESULT HandleCanTabNext(bool fCanNext) override;
	virtual RESULT HandleCanTabPrevious(bool fCanPrevious) override;

// Dream Desktop
public:
	RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth);

// DreamGarage compatability (temp?)
public:
	RESULT ResetAppComposite();
	RESULT AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT SetUIProgramNode(UIStageProgram *pUIProgramNode);

	RESULT ShutdownSource();
	RESULT CloseActiveAsset();
	RESULT OnReceiveAsset();

// Animations
public:
	float GetAnimationDuration();
	float GetAnimationScale();

	// should be changed once there is a flexible implementation of animation canceling
	bool IsAnimating();
	RESULT SetIsAnimating(bool fIsAnimating);

private:
	RESULT ShowControlView();
	WebBrowserPoint GetRelativePointofContact(point ptContact);

public:
	virtual RESULT Notify(InteractionObjectEvent *pSubscriberEvent) override;
	virtual RESULT Notify(UIEvent *pUIEvent) override;

// child applications
public:
	RESULT SetDreamUserApp(std::shared_ptr<DreamUserApp> pDreamUserApp);

private:

	// positioning helper
	std::shared_ptr<DreamUserApp> m_pDreamUserApp;

	// App used for opening content
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;

	// Apps in control area
	std::shared_ptr<DreamControlBar> m_pControlBar;
	std::shared_ptr<DreamControlView> m_pControlView;
	std::shared_ptr<DreamTabView> m_pDreamTabView;

	//TODO: potentially a class Browser and Desktop extend that implements
	// the control view events, ContactAtPoint, ScrollByDiff, etc.
	// ControlViewObserver?
	std::shared_ptr<DreamContentSource> m_pActiveSource;

	//TODO: list of objects that relate to the right bar
	//std::vector<std::shared_ptr<DreamApp>> m_openApps;

	// string saved from KEY_DOWN interaction events
	// this is used for opening a URL, key presses are sent directly to the active browser
	std::string m_strURL;

// logic
private:
	bool m_fHasOpenApp;
	bool m_fFromMenu = false;
	bool m_fCanPressButton[2];

	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;

	bool m_fKeyboardUp = false;

// layout variables
private:
	float m_centerOffset;

	std::shared_ptr<DreamDesktopApp> m_pDreamDesktop = nullptr;

	float m_animationScale = ANIMATION_SCALE;

	UIStageProgram *m_pUIStageProgram = nullptr;

	// certainly temporary
	bool m_fIsAnimating = false;

	std::string m_strDesktopTitle = TITLE_DESKTOP;
	std::string m_strWebsiteTitle = TITLE_WEBSITE;
};

#endif // ! DREAM_USER_CONTROL_AREA_H_

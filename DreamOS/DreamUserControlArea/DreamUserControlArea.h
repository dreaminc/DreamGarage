#ifndef DREAM_USER_CONTROL_AREA_H_
#define DREAM_USER_CONTROL_AREA_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamGarage/UserAreaControls.h"
#include "DreamGarage/DreamBrowser.h"
#include "Primitives/Subscriber.h"
#include "DreamVCam.h"

#include <vector>
#include <map>

class DreamUserApp;
class UIControlView;
class DreamUIBar;
class UITabView;
class DreamContentSource;
class DreamDesktopApp;

class CEFBrowserManager;
struct WebBrowserPoint;
class EnvironmentAsset;
class EnvironmentShare;

class AudioPacket;

struct InteractionObjectEvent;

class UIStageProgram;

class quad;

#define VIEW_POS_DEPTH 0.1f	
#define VIEW_POS_HEIGHT -0.2f

#define ANIMATION_SCALE 0.1f

#define SCOPE_DESKTOP "MenuProviderScope.DesktopMenuProvider"
#define SCOPE_WEBSITE "MenuProviderScope.WebsiteMenuProvider"
#define SCOPE_CAMERA "MenuProviderScope.CameraMenuProvider"

#define CAMERA_CONTENT_CONTROL_TYPE "ContentControlType.Camera"
 
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

	// absolute positions
	point GetCenter();
	float GetCenterOffset();
	float GetTotalWidth();
	float GetTotalHeight();

// Animations
public:
	RESULT Show();
	RESULT Hide();

	RESULT ShowDesktopKeyboard();

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
	RESULT CreateBrowserSource(std::string strScope);
	RESULT HideWebsiteTyping();

	RESULT OnCameraInMotion();
	RESULT OnCameraAtRest();

// DreamControlBar
public:
	bool CanPressButton(UIButton *pButtonContext);
	RESULT Open();
	RESULT Minimize();
	RESULT Maximize();

// DreamTabView
public:
	std::shared_ptr<DreamContentSource> GetActiveSource();
	std::shared_ptr<DreamContentSource> GetActiveCameraSource();

	RESULT SetActiveSource(std::shared_ptr<DreamContentSource> pNewContent);

// DreamBrowser
public:

	virtual RESULT UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) override;
	virtual RESULT UpdateControlBarText(std::string& strTitle) override;
	virtual RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT UpdateURLBarSecurity(bool fSecure) override;
	virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override;

	//TODO: present keyboard from browser::OnNodeFocusChanged
	virtual RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) override;
	virtual RESULT HandleIsInputFocused(bool fIsFocused, DreamContentSource *pContext) override;
	virtual RESULT HandleLoadEnd() override;
	bool IsContentVisible();

	virtual RESULT HandleDreamFormSuccess() override;
	virtual RESULT HandleDreamFormCancel() override { return R_NOT_IMPLEMENTED; };
	virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) override { return R_NOT_IMPLEMENTED; };
	virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) override { return R_NOT_IMPLEMENTED; };

	virtual RESULT HandleCanTabNext(bool fCanNext) override;
	virtual RESULT HandleCanTabPrevious(bool fCanPrevious) override;

	virtual std::string GetCertificateErrorURL() override;
	virtual std::string GetLoadErrorURL() override;

// DreamVCam
public:
	RESULT OnVirtualCameraCaptured();
	RESULT OnVirtualCameraReleased();

	RESULT OnVirtualCameraSettings(point ptPosition, quaternion qOrientation);

	RESULT SetVirtualCameraSource(DreamVCam::SourceType sourceType);
	RESULT MuteVirtualCamera(bool fMute);
	RESULT ResetVirtualCamera();

// DreamGarage compatability (temp?)
public:
	RESULT ResetAppComposite();
	RESULT AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT PendEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT AddEnvironmentCameraAsset();
	RESULT PendCameraEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT SetUIProgramNode(UIStageProgram *pUIProgramNode);

	RESULT OnDreamFormSuccess();
	RESULT ShutdownSource();
	RESULT ShutdownAllSources();

	RESULT CloseActiveAsset();
	RESULT CloseCameraTab();
	RESULT OnReceiveAsset();

	RESULT SendFirstFrame();
	RESULT StartSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare);
	RESULT ForceStopSharing();
	RESULT HandleStopSending();

	std::shared_ptr<EnvironmentShare> GetCurrentScreenShare();
	bool IsSharingScreen();

	RESULT UpdateIsActive(bool fIsActive);
	RESULT HandleCameraClosed();

	RESULT SetCertificateErrorURL(std::string strURL);
	RESULT SetLoadErrorURL(std::string strURL);

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

	// DreamModule doesn't have access to these, so UserControlArea is acting as a passthrough, since is it the parent app
	RESULT BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage);
	virtual RESULT HandleDreamAppMessage(PeerConnection *pPeerConnection, DreamAppMessage *pDreamAppMessage) override;
	std::shared_ptr<DreamVCam> GetVCam();

private:

	// positioning helper
	std::shared_ptr<DreamUserApp> m_pDreamUserApp;

	// App used for opening content
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;

	// UI in control area
	std::shared_ptr<UIView> m_pView = nullptr;
	std::shared_ptr<UserAreaControls> m_pUserControls = nullptr;

	// TODO: move to UI
	// Apps in control area
	std::shared_ptr<UIControlView> m_pControlView = nullptr;
	std::shared_ptr<UITabView> m_pDreamTabView = nullptr;

	//TODO: potentially a class Browser and Desktop extend that implements
	// the control view events, ContactAtPoint, ScrollByDiff, etc.
	// ControlViewObserver?
	std::shared_ptr<DreamContentSource> m_pActiveSource = nullptr;

	std::shared_ptr<DreamContentSource> m_pActiveCameraSource = nullptr;
	//TODO: list of objects that relate to the right bar
	//std::vector<std::shared_ptr<DreamApp>> m_openApps;

// logic
private:
	bool m_fHasOpenApp;
	bool m_fFromMenu = false;
	bool m_fCanPressButton[2];

	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;

	bool m_fKeyboardUp = false;

	// saved url for certificate error form
	std::string m_strCertificateErrorURL;
	std::string m_strLoadErrorURL;

// layout variables
private:
	float m_centerOffset;

	std::shared_ptr<DreamDesktopApp> m_pDreamDesktop = nullptr;
	std::shared_ptr<DreamVCam> m_pDreamVCam = nullptr;

	std::shared_ptr<EnvironmentShare> m_pCurrentScreenShare = nullptr;

	float m_animationScale = ANIMATION_SCALE;

	UIStageProgram *m_pUIStageProgram = nullptr;

	// certainly temporary
	bool m_fIsAnimating = false;

	std::string m_strDesktopScope = SCOPE_DESKTOP;
	std::string m_strWebsiteScope = SCOPE_WEBSITE;
	std::string m_strCameraScope = SCOPE_CAMERA;

	bool m_fUpdateDreamUIBar = false;
	bool m_fPendDreamFormSuccess = false;

	std::shared_ptr<EnvironmentAsset> m_pPendingEnvironmentAsset = nullptr;
	std::shared_ptr<EnvironmentAsset> m_pPendingEnvironmentCameraAsset = nullptr;
};

#endif // ! DREAM_USER_CONTROL_AREA_H_

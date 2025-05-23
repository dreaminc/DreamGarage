#ifndef DREAM_USER_APP_H_
#define DREAM_USER_APP_H_

#include "core/ehm/EHM.h"

// Dream User App
// dos/src/app/DreamUserApp/DreamUserApp.h

// The Dream User app is the app to contain all of the 
// relevant user information (and assets)

#include "memory"                      // for shared_ptr
#include "xstring"                     // for wstring
#include <map>

#include "os/app/DreamApp.h"

#include "core/types/Subscriber.h"

#include "core/primitives/point.h"

#include "core/hand/HandType.h"

#include "ui/UICommon.h"

class DreamOS;
class composite;

struct InteractionObjectEvent;
struct HysteresisEvent;

class HysteresisObject;
class quad; 
class texture;
class hand;
class DimRay;
class VirtualObj;
class user;
class quaternion;

class CEFBrowserManager;
class CameraNode;

// TODO: All this into a configuration file
#define MENU_HEIGHT -0.16f
#define MENU_DEPTH 0.0f
#define MENU_DEPTH_MIN 0.5f
#define MENU_DEPTH_MAX 0.7f

#define GAZE_OVERLAY_MS 800.0 //1250.0

#define OVERLAY_ASPECT_RATIO (332.0f / 671.0f)

// default user settings
#define MAIN_DIAGONAL 0.70f

#define VIEW_ANGLE 32.0f

#define ANIMATION_DURATION_SECONDS 0.175f

#define MESSAGE_QUAD_WIDTH 2.40f
#define MESSAGE_QUAD_HEIGHT (MESSAGE_QUAD_WIDTH * 9.0f / 16.0f)

enum class UserObserverEventType {
	BACK,
	DISMISS,
	KB_ENTER,
	INVALID
};

class DreamUserObserver {
public:
	virtual RESULT HandleEvent(UserObserverEventType type) = 0;
	virtual texture* GetOverlayTexture(HAND_TYPE type);
};

class DreamUserApp : public DreamApp<DreamUserApp>, 
					public Subscriber<InteractionObjectEvent>,
					public Subscriber<HysteresisEvent> {
	friend class DreamAppManager;
	friend class MultiContentTestSuite;
	friend class DreamUserControlAreaApp;
	friend class DreamSettingsApp;
	friend class DreamFormApp;

public:
	DreamUserApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamUserApp();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamUserApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	virtual RESULT Notify(InteractionObjectEvent *mEvent);

	hand *GetHand(HAND_TYPE type);
	RESULT SetHand(hand* pHand);
	RESULT ClearHands();

	RESULT CreateHapticImpulse(VirtualObj *pEventObj);

	RESULT HandleUserObserverEvent(UserObserverEventType type);
	
public:
	// used to set seating position created in DreamGarage and DreamEnvironmentApp
	RESULT GetAppBasisPosition(point& ptOrigin);
	RESULT GetAppBasisOrientation(quaternion& qOrigin) ;

	RESULT SetAppCompositeOrientation(quaternion qOrientation);
	RESULT SetAppCompositePosition(point ptPosition);

	// used for app transitioning logic, helps with what happens when menu is pressed or 
	// DreamFormSuccess is fired
	RESULT SetHasOpenApp(bool fHasOpenApp);
	RESULT SetEventApp(DreamUserObserver *pEventApp);
	RESULT SetPreviousApp(DreamUserObserver* pObserver) ;

	RESULT ResetAppComposite();

	RESULT SetSeatingPosition(int seatPosition);

	// Pointing
public:
	virtual RESULT Notify(HysteresisEvent *mEvent);
private:

	// Current Hysteresis Event (ON/OFF)
	bool m_fLeftSphereOn = false;
	bool m_fRightSphereOn = false;

	bool m_fLeftSphereInteracting = false;
	bool m_fRightSphereInteracting = false;

	point m_ptLeftPointer;
	point m_ptRightPointer;

	VirtualObj *m_pLeftInteractionObject = nullptr;
	VirtualObj *m_pRightInteractionObject = nullptr;

	HysteresisObject *m_pPointingArea = nullptr;

private:
	RESULT UpdateCompositeWithHands(float yPos);

//protected:
public:
	RESULT ToggleUserModel();

	RESULT UpdateHand(HAND_TYPE type);
	RESULT UpdateOverlayTexture(HAND_TYPE type);
	RESULT UpdateOverlayTextures();

	RESULT UpdateHysteresisObject();

	RESULT UpdateLabelOrientation(CameraNode *pCamera);

// user settings
public:
	float GetPXWidth();
	float GetPXHeight();
	float GetBaseWidth();
	float GetBaseHeight();
	float GetViewAngle();
	float GetAnimationDuration();
	float GetSpacingSize();

	std::shared_ptr<CEFBrowserManager> GetBrowserManager();

// Startup Message Quad
public:
	enum class StartupMessage {
		WELCOME,
		SIGN_IN,
		UPDATE_REQUIRED,
		INVALID_REFRESH_TOKEN,
		INTERNET_REQUIRED,
		INVALID
	};

public:
	RESULT SetStartupMessageType(StartupMessage messageType);

	RESULT ShowMessageQuad();
	RESULT HideMessageQuad();

private:
	std::wstring k_wstrUpdateRequired = L"LaunchQuad/launch-update-required.png"; 
	std::wstring k_wstrWelcome = L"LaunchQuad/launch-welcome.png"; 
	std::wstring k_wstrSignIn = L"LaunchQuad/launch-sign-in.png"; 
	std::wstring k_wstrInternetRequired = L"LaunchQuad/launch-internet-required.png"; 

private:
	std::map<StartupMessage, std::wstring> m_textureStringFromStartupMessage = {
		{ StartupMessage::UPDATE_REQUIRED, k_wstrUpdateRequired },
		{ StartupMessage::WELCOME, k_wstrWelcome },
		{ StartupMessage::SIGN_IN, k_wstrSignIn },
		{ StartupMessage::INVALID_REFRESH_TOKEN, k_wstrSignIn },
		{ StartupMessage::INTERNET_REQUIRED, k_wstrInternetRequired }
	};

private:
	std::shared_ptr<composite> m_pMessageComposite = nullptr;
	std::shared_ptr<quad> m_pMessageQuad = nullptr;
	std::shared_ptr<quad> m_pMessageQuadBackground = nullptr;

	bool m_fShowLaunchQuad = false;
	StartupMessage m_currentLaunchMessage;

	float m_messageQuadHeight = MESSAGE_QUAD_HEIGHT;
	float m_messageQuadWidth = MESSAGE_QUAD_WIDTH;

	float m_messageBackgroundWidth = BORDER_WIDTH;
	float m_messageBackgroundHeight = BORDER_HEIGHT;


private:
	//user *m_pUserModel = nullptr;
	std::shared_ptr<DimRay> m_pOrientationRay = nullptr;
	
	std::shared_ptr<user> m_pUserModel = nullptr;
	hand* m_pLeftHand = nullptr;
	hand* m_pRightHand = nullptr;

	std::shared_ptr<composite> m_pPhantomRightHand = nullptr;
	std::shared_ptr<composite> m_pPhantomLeftHand = nullptr;

	// apps position themselves with this when they are presented
	VirtualObj *m_pAppBasis = nullptr;

	bool m_fHasOpenApp = false;
	// current app that should receive events from the user
	DreamUserObserver* m_pEventApp = nullptr;
	DreamUserObserver* m_pPreviousApp = nullptr;

	bool m_fSendLeftPointerMessage = false;
	bool m_fSendRightPointerMessage = false;

	// reflection of the member 
	bool m_fStreaming = false;

public:
	struct UserSettings {
		float m_spacingSize = SPACING_SIZE;
		float m_pxWidth = BROWSER_WIDTH;
		float m_pxHeight = BROWSER_HEIGHT;

		float m_diagonalSize = MAIN_DIAGONAL;
		float m_viewAngle = VIEW_ANGLE;

		float m_aspectRatio;
		float m_baseWidth;
		float m_baseHeight;

		float m_animationDuration = ANIMATION_DURATION_SECONDS;
	};

private:
	float m_menuDepth = MENU_DEPTH;
	float m_menuHeight = MENU_HEIGHT;

	UserSettings *m_userSettings;

	double m_msGazeOverlayDelay = GAZE_OVERLAY_MS;
	double m_msGazeStart;

	bool m_fCollisionLeft = false;
	bool m_fCollisionRight = false;
	bool m_fIsSharing = false;

	bool m_fHeadsetAndHandsTracked = false;

	texture *m_pTextureDefaultGazeLeft = nullptr;
	texture *m_pTextureDefaultGazeRight = nullptr;

	std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;
};

#endif // ! DREAM_USER_APP_H_
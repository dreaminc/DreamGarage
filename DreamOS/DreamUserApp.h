#ifndef DREAM_USER_APP_H_
#define DREAM_USER_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamUserApp.h
// The Dream User app is the app to contain all of the 
// relevant user information (and assets)

#include "Primitives/Subscriber.h"
#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "Primitives/HandType.h"

#include <map>
#include <vector>
#include <stack>

struct InteractionObjectEvent;

class volume;
class quad; 
class texture;
class hand;
class UIMallet;
class DimRay;
class VirtualObj;
class UIKeyboard;
class UIKeyboardHandle;
class DimObj;

#define MENU_HEIGHT -0.16f
#define MENU_DEPTH 0.0f
#define MENU_DEPTH_MIN 0.5f
#define MENU_DEPTH_MAX 0.7f

#define GAZE_OVERLAY_MS 800.0 //1250.0

#define OVERLAY_ASPECT_RATIO (332.0f / 671.0f)

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

class DreamUserHandle : public DreamAppHandle {
public:
	//TODO: this is unsafe, since the mallets can be used later, 
	// this function could return a handle to a mallet
	UIMallet *RequestMallet(HAND_TYPE type);
	RESULT RequestHapticImpulse(VirtualObj *pEventObj);

	RESULT RequestAppBasisPosition(point& ptOrigin);
	RESULT RequestAppBasisOrientation(quaternion& qOrigin);

	RESULT SendPopFocusStack();
	RESULT SendPushFocusStack(DreamUserObserver* pObserver);
	RESULT SendClearFocusStack();

	RESULT SendKBEnterEvent();
	RESULT SendUserObserverEvent(UserObserverEventType type);
	UIKeyboardHandle *RequestKeyboard();
	RESULT SendReleaseKeyboard();

	RESULT RequestStreamingState(bool& fStreaming);
	RESULT SendStreamingState(bool fStreaming);
	RESULT SendPreserveSharingState(bool fIsSharing);

	RESULT SendStopSharing();

	RESULT RequestResetAppComposite();

private:
	virtual UIMallet *GetMallet(HAND_TYPE type) = 0;
	virtual RESULT CreateHapticImpulse(VirtualObj *pEventObj) = 0;

	virtual RESULT GetAppBasisPosition(point& ptOrigin) = 0;
	virtual RESULT GetAppBasisOrientation(quaternion& qOrigin) = 0;

	virtual RESULT PopFocusStack() = 0;
	virtual RESULT PushFocusStack(DreamUserObserver* pObserver) = 0;
	virtual RESULT ClearFocusStack() = 0;

	virtual RESULT HandleKBEnterEvent() = 0;
	virtual RESULT HandleUserObserverEvent(UserObserverEventType type) = 0;
	virtual UIKeyboardHandle *GetKeyboard() = 0;
	virtual RESULT ReleaseKeyboard() = 0;

	virtual RESULT GetStreamingState(bool& fStreaming) = 0;
	virtual RESULT SetStreamingState(bool fStreaming) = 0;
	virtual RESULT PreserveSharingState(bool fIsSharing) = 0;

	virtual RESULT StopSharing() = 0;

	virtual RESULT ResetAppComposite() = 0;

};

class DreamUserApp : public DreamApp<DreamUserApp>, public DreamUserHandle, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;
	friend class MultiContentTestSuite;
	friend class DreamUserControlArea;

public:
	DreamUserApp(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual DreamAppHandle *GetAppHandle() override;
	virtual unsigned int GetHandleLimit() override;

protected:
	static DreamUserApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:

public:
	virtual RESULT Notify(InteractionObjectEvent *mEvent) override;

	RESULT SetHand(hand* pHand);

	virtual UIMallet *GetMallet(HAND_TYPE type) override;
	hand *GetHand(HAND_TYPE type);
	virtual RESULT CreateHapticImpulse(VirtualObj *pEventObj) override;

	virtual RESULT GetAppBasisPosition(point& ptOrigin) override;
	virtual RESULT GetAppBasisOrientation(quaternion& qOrigin) override;

	virtual RESULT PopFocusStack() override;
	virtual RESULT PushFocusStack(DreamUserObserver* pObserver) override;
	virtual RESULT ClearFocusStack() override;
	RESULT OnFocusStackEmpty(DreamUserObserver *pLastApp);

	virtual RESULT HandleKBEnterEvent() override;
	virtual RESULT HandleUserObserverEvent(UserObserverEventType type) override;
	virtual UIKeyboardHandle *GetKeyboard() override;
	virtual RESULT ReleaseKeyboard() override;

	virtual RESULT GetStreamingState(bool& fStreaming) override;
	virtual RESULT SetStreamingState(bool fStreaming) override;
	virtual RESULT PreserveSharingState(bool fIsSharing) override;

	virtual RESULT StopSharing() override;

	virtual RESULT ResetAppComposite() override;

	RESULT SetHasOpenApp(bool fHasOpenApp);
	RESULT SetEventApp(DreamUserObserver *pEventApp);

//protected:
public:

	RESULT UpdateHand(HAND_TYPE type);
	RESULT UpdateOverlayTexture(HAND_TYPE type);
	RESULT UpdateOverlayTextures();

	RESULT UpdateCompositeWithCameraLook(float depth, float yPos);
	RESULT UpdateCompositeWithHands(float yPos);

private:
	//user *m_pUserModel = nullptr;
	std::shared_ptr<DimRay> m_pOrientationRay = nullptr;
	
	hand* m_pLeftHand = nullptr;
	hand* m_pRightHand = nullptr;

	UIMallet* m_pLeftMallet = nullptr;
	UIMallet* m_pRightMallet = nullptr;

	// apps position themselves with this when they are presented
	VirtualObj *m_pAppBasis = nullptr;

	bool m_fHasOpenApp = false;
	// current app that should receive events from the user
	DreamUserObserver* m_pEventApp = nullptr;

	UIKeyboardHandle *m_pKeyboardHandle = nullptr;

	// reflection of the member 
	bool m_fStreaming = false;

private:
	float m_menuDepth = MENU_DEPTH;
	float m_menuHeight = MENU_HEIGHT;

	double m_msGazeOverlayDelay = GAZE_OVERLAY_MS;
	double m_msGazeStart;

	bool m_fCollisionLeft = false;
	bool m_fCollisionRight = false;
	bool m_fIsSharing = false;

	texture *m_pTextureDefaultGazeLeft = nullptr;
	texture *m_pTextureDefaultGazeRight = nullptr;
};

#endif // ! DREAM_USER_APP_H_
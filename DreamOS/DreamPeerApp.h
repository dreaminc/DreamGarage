#ifndef DREAM_PEER_H_
#define DREAM_PEER_H_

#include "RESULT/EHM.h"

// DREAM PEER
// DreamOS/DreamPeer.h
// Dream Peer holds context for a given dream peer
// and keeps track of state

//#include "Primitives/DObject.h"
#include "Primitives/Subscriber.h"

#include "DreamApp.h"

#include "Primitives/point.h"
#include "Primitives/quaternion.h"

#include "Primitives/hand.h"

#define NAMETAG_BORDER 0.1f
#define NAMETAG_HEIGHT 0.05f
#define NAME_LINE_HEIGHT .12f
#define USERNAME_ANIMATION_DURATION 0.3f

#define BASE_LABEL_WIDTH 0.4f
#define LABEL_HEIGHT (BASE_LABEL_WIDTH) * (80.0f / 332.0f)
#define LABEL_PHOTO_WIDTH (LABEL_HEIGHT) // photo is square
#define LABEL_GAP_WIDTH (BASE_LABEL_WIDTH) * (20.0f / 332.0f)

class User;
class PeerConnection;
class composite;
class UIView;
class DreamOS;
class user;
class text;
class font;
class SpatialSoundObject;
class AudioDataMessage;

struct InteractionObjectEvent;

class WebRTCPeerConnectionProxy;

class DreamPeerApp : public DreamApp<DreamPeerApp> {
	friend class DreamAppManager;

public:
	enum class state : uint16_t {
		UNINITIALIZED,
		INITIALIZED,
		PENDING,
		ESTABLISHED,
		DISCONNECTED,
		INVALID
	};

	struct PeerConnectionState {
		unsigned fDataChannel : 1;
		unsigned fAudioChannel : 1;

		unsigned fSentHandshakeRequest : 1;
		unsigned fReceivedHandshakeAck : 1;
		unsigned fReceivedHandshakeRequest : 1;
		unsigned fSentHandshakeRequestACK : 1;

		unsigned fReceivedAndRespondedToHandshake : 1;

		// Hang detection
		uint32_t rxStayAliveCounter;
		uint32_t txStayAliveAckCounter;
	};

public:
	class DreamPeerAppObserver {
	public:
		virtual RESULT OnDreamPeerStateChange(DreamPeerApp* pDreamPeer) = 0;
	};

	RESULT RegisterDreamPeerObserver(DreamPeerAppObserver* pDreamPeerObserver);

private:
	DreamPeerAppObserver* m_pDreamPeerObserver = nullptr;

public:

	DreamPeerApp(DreamOS *pDOS, void *pContext = nullptr);
	//DreamPeerApp::DreamPeerApp(DreamOS *pDOS, PeerConnection *pPeerConnection, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamPeerApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:

	// This needs to be called before InitializeUserNameLabel
	// because it uses the width of the rendered text object
	RESULT InitializeUserNameText();

	RESULT InitializeUserNameLabel();

public:
	RESULT ShowUserNameField();
	RESULT HideUserNameField();

public:
	RESULT OnDataChannel();
	RESULT OnAudioChannel();

	RESULT SentHandshakeRequest();
	RESULT ReceivedHandshakeACK();
	RESULT SentHandshakeACK();
	RESULT ReceivedHandshakeRequest();

	RESULT UpdatePeerHandshakeState();
	bool IsPeerReady();
	bool IsDataChannel();

	DreamPeerApp::state GetState();

	// Not the most eloquent, revisit in the future
	bool IsHandshakeRequestHung();
	bool IsHandshakeRequestAckHung();

	long GetPeerUserID();

	PeerConnection *GetPeerConnection();
	RESULT SetPeerConnection(PeerConnection *pPeerConnection);

	RESULT PendProfilePhotoDownload();
	RESULT OnProfilePhotoDownload(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext);
	RESULT UpdateProfilePhoto();

	WebRTCPeerConnectionProxy *GetWebRTCPeerConnectionProxy();

	std::shared_ptr<user> GetUserModel();
	RESULT AssignUserModel(user* pUserModel);

	RESULT ReleaseUserModel();

	RESULT SetVisible(bool fVisibile = true);
	bool IsVisible();
	bool IsUserNameVisible();
	RESULT SetPosition(const point& ptPosition);
	RESULT SetOrientation(const quaternion& qOrientation);
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT UpdateMouth(float mouthScale);
	RESULT RotateByDeg(float degX, float degY, float degZ);
	
	RESULT HandleUserAudioDataMessage(AudioDataMessage *pAudioDataMessage);

	RESULT SetUsernameAnimationDuration(float animationDuration);

	RESULT SetUserLabelPosition(point ptPosition);
	RESULT SetUserLabelOrientation(quaternion qOrientation);

	bool HasProfilePhoto();

private:
	RESULT SetState(DreamPeerApp::state peerState);

private:
	std::wstring k_wstrLeft = L"UserLabel/user-label-background-left.png";
	std::wstring k_wstrMiddle = L"UserLabel/user-label-background-middle.png";
	std::wstring k_wstrRight = L"UserLabel/user-label-background-right.png";

	// Used instead of the left texture if the user does not have a profile picture
	std::wstring k_wstrLeftEmpty = L"UserLabel/user-label-background-left-empty.png";

	// Used for photo while download is pending
	std::wstring k_wstrPhoto = L"UserLabel/user-label-background-photo-temp.png";

private:
	long m_peerUserID = -1;
	std::string m_strScreenName;
	long m_avatarModelId = -1;
	std::string m_strProfilePhotoURL;

	DreamOS *m_pDOS = nullptr;
	
	PeerConnection *m_pPeerConnection = nullptr;
	
	DreamPeerApp::state m_state = DreamPeerApp::state::UNINITIALIZED;

	std::shared_ptr<SpatialSoundObject> m_pSpatialSoundObject = nullptr;
	std::shared_ptr<user> m_pUserModel = nullptr;
	bool m_fPendingAssignedUserModel = false;
	bool m_fVisible = false;

	sphere *m_pSphere = nullptr;

	// appear and disappear duration in seconds (direct plug into PushAnimation)
	float m_userNameAnimationDuration = USERNAME_ANIMATION_DURATION;

	color m_hiddenColor = color(1.0f, 1.0f, 1.0f, 0.0f);
	color m_backgroundColor = color(1.0f, 1.0f, 1.0f, 0.75f);
	color m_visibleColor = color(1.0f, 1.0f, 1.0f, 1.0f);

	std::shared_ptr<std::vector<uint8_t>> m_pPendingPhotoTextureBuffer;

private:
	std::shared_ptr<composite> m_pUIObjectComposite = nullptr;
	std::shared_ptr<composite> m_pUserLabelComposite = nullptr;
	std::shared_ptr<text> m_pTextUserName = nullptr;
	std::shared_ptr<font> m_pFont = nullptr;

	std::shared_ptr<quad> m_pPhotoQuad = nullptr;
	std::shared_ptr<quad> m_pLeftGap = nullptr;
	std::shared_ptr<quad> m_pNameBackground = nullptr;
	std::shared_ptr<quad> m_pRightGap = nullptr;
	std::shared_ptr<texture> m_pTextBoxTexture = nullptr;

private:
	PeerConnectionState m_peerConnectionState = {0};
};

#endif // ~ DREAM_PEER_H_
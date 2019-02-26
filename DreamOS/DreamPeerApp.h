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
class CameraNode;

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

	std::shared_ptr<composite> GetUserLabelComposite();
	RESULT SetUserLabelPosition(point ptPosition);
	RESULT SetUserLabelOrientation(quaternion qOrientation);
	RESULT UpdateLabelOrientation(camera *pCamera);

private:
	RESULT SetState(DreamPeerApp::state peerState);

private:
	long m_peerUserID = -1;
	std::string m_strScreenName;
	std::string m_strInitials;
	long m_avatarModelId = -1;
	std::string m_strProfilePhotoURL;

	DreamOS *m_pDOS = nullptr;
	
	PeerConnection *m_pPeerConnection = nullptr;
	
	DreamPeerApp::state m_state = DreamPeerApp::state::UNINITIALIZED;

	std::shared_ptr<SpatialSoundObject> m_pSpatialSoundObject = nullptr;
	std::shared_ptr<user> m_pUserModel = nullptr;
	bool m_fPendingAssignedUserModel = false;
	bool m_fVisible = false;

private:
	PeerConnectionState m_peerConnectionState = {0};
};

#endif // ~ DREAM_PEER_H_
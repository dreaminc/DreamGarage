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
#define NAMETAG_HEIGHT 0.3f
#define NAME_LINE_HEIGHT .12f

class User;
class PeerConnection;
class composite;
class UIView;
class DreamOS;
class user;
class text;
class font;

struct InteractionObjectEvent;

class WebRTCPeerConnectionProxy;

class DreamPeerApp : public DreamApp<DreamPeerApp>, public Subscriber<InteractionObjectEvent> {
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

	DreamPeerApp::DreamPeerApp(DreamOS *pDOS, void *pContext = nullptr);
	//DreamPeerApp::DreamPeerApp(DreamOS *pDOS, PeerConnection *pPeerConnection, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamPeerApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	virtual RESULT Notify(InteractionObjectEvent *mEvent) override;
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

	DreamPeerApp::state GetState();

	long GetPeerUserID();

	PeerConnection *GetPeerConnection();
	RESULT SetPeerConnection(PeerConnection *pPeerConnection);

	WebRTCPeerConnectionProxy *GetWebRTCPeerConnectionProxy();

	std::shared_ptr<user> GetUserModel();
	RESULT AssignUserModel(user* pUserModel);

	RESULT ReleaseUserModel();

	RESULT SetVisible(bool fVisibile = true);
	RESULT SetPosition(const point& ptPosition);
	RESULT SetOrientation(const quaternion& qOrientation);
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT UpdateMouth(float mouthScale);
	RESULT RotateByDeg(float degX, float degY, float degZ);
		
private:
	RESULT SetState(DreamPeerApp::state peerState);

private:
	long m_peerUserID = -1;
	std::string m_strScreenName;

	DreamOS *m_pDOS = nullptr;
	
	PeerConnection *m_pPeerConnection = nullptr;
	
	DreamPeerApp::state m_state = DreamPeerApp::state::UNINITIALIZED;

	std::shared_ptr<user> m_pUserModel = nullptr;
	bool m_fPendingAssignedUserMode = false;
	bool m_fGazeInteraction = false;

	sphere *m_pSphere = nullptr;

	std::shared_ptr<volume> m_pPhantomVolume = nullptr;
	std::shared_ptr<DimRay> m_pOrientationRay = nullptr;
	
	double m_msTimeGazeStart;
	double m_msTimeUserNameDelay = 1250;

	color m_hiddenColor = color(1.0f, 1.0f, 1.0f, 0.0f);
	color m_backgroundColor = color(1.0f, 1.0f, 1.0f, 0.5f);
	color m_visibleColor = color(1.0f, 1.0f, 1.0f, 1.0f);

	std::shared_ptr<composite> m_pNameComposite = nullptr;
	std::shared_ptr<text> m_pTextUserName = nullptr;
	std::shared_ptr<font> m_pFont = nullptr;

	std::shared_ptr<quad> m_pNameBackground = nullptr;
	std::shared_ptr<texture> m_pTextBoxTexture = nullptr;

private:
	PeerConnectionState m_peerConnectionState = {0};
};

#endif // ~ DREAM_PEER_H_
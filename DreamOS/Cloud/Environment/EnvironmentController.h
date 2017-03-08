#ifndef ENVIRONMENT_CONTROLLER_H_
#define ENVIRONMENT_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/EnvironmentController.h
// The Environment Controller

#include <string>
#include <memory>

#include "Cloud/Controller.h"

#include "Environment.h"
#include "EnvironmentPeer.h"
#include "PeerConnectionController.h"
#include "Cloud/Menu/MenuController.h"

#include "Cloud/User/User.h"
#include "Cloud/User/TwilioNTSInformation.h"

class Websocket;
class CloudMessage;

// TODO: This is actually a UserController - so change the name of object and file
class EnvironmentController : public Controller, public PeerConnectionController::PeerConnectionControllerObserver {
	friend class MenuController;
public:
	enum class state {
		UNINITIALIZED,
		SOCKET_INITIALIZED,
		SOCKET_CONNECTING,
		SOCKET_CONNECTED,
		CREATING_ENVIRONMENT_USER,
		ENVIRONMENT_USER_CREATED,
		ENVIRONMENT_PEER_LIST_REQUESTED,
		ENVIRONMENT_PEER_LIST_RECEIVED,
		ENVIRONMENT_CONNECTED_AND_READY,

		// Peer Connection
		// TODO: Move to Peer Connection?
		SET_SDP_OFFER,
		SET_SDP_ANSWER,
		SET_OFFER_CANDIDATES,
		SET_ANSWER_CANDIDATES,

		// Menu
		MENU_API_REQUEST,

		INVALID
	};

public:
	enum class EnvironmentMethod {
		CONNECT_SOCKET,
		INVALID
	};

public:
	class EnvironmentControllerObserver {
	public:
		virtual RESULT OnPeersUpdate(long index) = 0;
		virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
		virtual RESULT OnAudioData(long peerConnectionID,
			const void* audio_data,
			int bits_per_sample,
			int sample_rate,
			size_t number_of_channels,
			size_t number_of_frames) = 0;
		virtual long GetUserID() = 0;
	};

	RESULT RegisterEnvironmentControllerObserver(EnvironmentControllerObserver* pEnvironmentControllerObserver);

public:
	EnvironmentController(Controller* pParentController, long environmentID);
	~EnvironmentController();

public:
	RESULT Initialize();

	RESULT ConnectToEnvironmentSocket(User user, long environmentID);
	RESULT CreateEnvironmentUser(User user);	// TODO: This is deprecated
	RESULT GetEnvironmentPeerList(User user);

	// TODO: New Server Integration
	nlohmann::json CreateEnvironmentMessage(User user, PeerConnection *pPeerConnection, std::string strMethod);
	RESULT SetSDPOffer(User user, PeerConnection *pPeerConnection);
	RESULT SetSDPAnswer(User user, PeerConnection *pPeerConnection);
	RESULT SetOfferCandidates(User user, PeerConnection *pPeerConnection);
	RESULT SetAnswerCandidates(User user, PeerConnection *pPeerConnection);

	RESULT UpdateEnvironmentUser();
	RESULT PrintEnvironmentPeerList();

	long GetUserID();

	// TODO: Temporary 
	//RESULT InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel);

protected:
	RESULT SendEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudRequest, state newState);

private:
	RESULT SendEnvironmentSocketData(const std::string& strData, state newState);
	RESULT InitializeWebsocket(std::string& strURI);

	void HandleWebsocketMessage(const std::string& strMessage);
	void HandleWebsocketConnectionOpen();
	void HandleWebsocketConnectionClose();
	void HandleWebsocketConnectionFailed();

	RESULT ClearPeerList();
	RESULT AddNewPeer(long userID, long environmentID, const std::string& strSDPOffer);
	RESULT UpdatePeer(long userID, long environmentID, const std::string& strSDPOffer);
	bool FindPeerByUserID(long userID);
	EnvironmentPeer *GetPeerByUserID(long userID);
	
	std::string GetMethodURI(EnvironmentMethod userMethod);

	// PeerConnectionControllerObserver
	virtual RESULT OnPeersUpdate(long index) override;
	virtual RESULT OnDataChannelStringMessage(long peerUserID, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnAudioData(long peerConnectionID,
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames) override;
	virtual RESULT OnSDPOfferSuccess(PeerConnection *pPeerConnection) override;
	virtual RESULT OnSDPAnswerSuccess(PeerConnection *pPeerConnection) override;
	virtual RESULT OnICECandidatesGatheringDone(PeerConnection *pPeerConnection) override;

public:
	long GetEnvironmentID() { return m_environment.GetEnvironmentID(); }
	RESULT SetEnvironmentID(long environmentID) { return m_environment.SetEnvironmentID(environmentID); }
	bool IsUserIDConnected(long peerUserID);

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT BroadcastDataChannelStringMessage(std::string& strMessage);
	RESULT BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT SetUser(User currentUser);
	RESULT SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation);

	bool IsEnvironmentSocketConnected();

	// Menu Controller
	MenuControllerProxy* GetMenuControllerProxy();

public:
	EnvironmentController::state GetState() {
		return m_state;
	}

private:
	bool m_fConnected;
	EnvironmentController::state m_state;

	bool m_fPendingMessage;
	uint64_t m_pendingMessageID;

	Environment m_environment;
	std::unique_ptr<Websocket> m_pEnvironmentWebsocket;

	std::vector<EnvironmentPeer> m_environmentPeers;

	std::unique_ptr<PeerConnectionController> m_pPeerConnectionController;
	std::unique_ptr<MenuController> m_pMenuController;

	EnvironmentControllerObserver *m_pEnvironmentControllerObserver;
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_
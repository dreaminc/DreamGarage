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

class User;
class Websocket;


// TODO: This is actually a UserController - so change the name of object and file
class EnvironmentController : public Controller{
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
		INVALID
	};

public:
	enum class EnvironmentMethod {
		CONNECT_SOCKET,
		INVALID
	};
public:
	EnvironmentController(Controller* pParentController, long environmentID);
	~EnvironmentController();

public:
	RESULT ConnectToEnvironmentSocket(User user);
	RESULT CreateEnvironmentUser(User user);	
	RESULT GetEnvironmentPeerList(User user);

	RESULT UpdateEnvironmentUser();
	RESULT PrintEnvironmentPeerList();

private:
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
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_
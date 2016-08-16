#ifndef ENVIRONMENT_CONTROLLER_H_
#define ENVIRONMENT_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/EnvironmentController.h
// The Environment Controller

#include <string>
#include <memory>

#include "Environment.h"
#include "EnvironmentPeer.h"

class User;
class Websocket;


// TODO: This is actually a UserController - so change the name of object and file
class EnvironmentController {
public:
	enum class state {
		SOCKET_UNINITIALIZED,
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
	EnvironmentController(long environmentID);
	~EnvironmentController();

public:
	RESULT ConnectToEnvironmentSocket(User user);
	RESULT CreateEnvironmentUser(User user);	
	RESULT GetEnvironmentPeerList(User user);

private:
	RESULT InitializeWebsocket(std::string& strURI);

	void HandleWebsocketMessage(const std::string& strMessage);
	void HandleWebsocketConnectionOpen();
	void HandleWebsocketConnectionClose();
	void HandleWebsocketConnectionFailed();

	RESULT ClearPeerList();
	RESULT AddNewPeer(long userID, long environmentID, const std::string& strSDPOffer);
	bool FindPeerByUserID(long userID);
	RESULT PrintEnvironmentPeerList();

private:
	bool m_fConnected;
	state m_state;

	bool m_fPendingMessage;
	uint64_t m_pendingMessageID;

	Environment m_environment;
	std::unique_ptr<Websocket> m_pEnvironmentWebsocket;

	std::vector<EnvironmentPeer> m_environmentPeers;
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_
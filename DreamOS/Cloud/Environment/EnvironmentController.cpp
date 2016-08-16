#include "EnvironmentController.h"
#include "Cloud/User/User.h"

#include "Cloud/Websockets/Websocket.h"
#include "json.hpp"

#include "Primitives/Types/UID.h"

EnvironmentController::EnvironmentController(long environmentID) :
	m_fConnected(false),
	m_fPendingMessage(false),
	m_pEnvironmentWebsocket(nullptr),
	m_pendingMessageID(0),
	m_state(state::SOCKET_UNINITIALIZED)
{
	m_environment = Environment(environmentID);
}

EnvironmentController::~EnvironmentController() {
	if (m_pEnvironmentWebsocket != nullptr) {
		m_pEnvironmentWebsocket->Stop();
	}
}

RESULT EnvironmentController::InitializeWebsocket(std::string& strURI) {
	RESULT r = R_PASS;

	//m_pEnvironmentWebsocket = std::unique_ptr<Websocket>(new Websocket(strURI));

	m_pEnvironmentWebsocket = std::unique_ptr<Websocket>(
		new Websocket(strURI, std::bind(&EnvironmentController::HandleWebsocketMessage, this, ::_1),
							  std::bind(&EnvironmentController::HandleWebsocketConnectionOpen, this),
							  std::bind(&EnvironmentController::HandleWebsocketConnectionClose, this),
							  std::bind(&EnvironmentController::HandleWebsocketConnectionFailed, this)));
	CN(m_pEnvironmentWebsocket);

	m_state = state::SOCKET_INITIALIZED;

Error:
	return r;
}

// TODO: FIX THIS ITS REALLY BAD (for testing)
static User s_user;

RESULT EnvironmentController::ConnectToEnvironmentSocket(User user) {
	RESULT r = R_PASS;

	m_environment = Environment(user.GetDefaultEnvironmentID());

	// TODO: Not hard coded!
	if (m_pEnvironmentWebsocket == nullptr) {
		std::string strURI = "ws://localhost:8000/environment/" + std::to_string(m_environment.GetEnvironmentID()) + "/";
		CR(InitializeWebsocket(strURI));
	}

	s_user = user;
	m_pEnvironmentWebsocket->SetToken(user.GetToken());

	m_state = state::SOCKET_CONNECTING;
	CRM(m_pEnvironmentWebsocket->Start(), "Failed to start environment websocket");

	m_fConnected = true;

Error:
	return r;
}

RESULT EnvironmentController::CreateEnvironmentUser(User user) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	long environmentID = user.GetDefaultEnvironmentID();

	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");
	
	// Set up the JSON data
	m_pendingMessageID = UID().GetID();
	jsonData["id"] = m_pendingMessageID;
	jsonData["token"] = user.GetToken();
	jsonData["method"] = "environmentuser.create";
	jsonData["params"] = {
		{"sdp_offer", "{'foo': 'bar2'}"},
		{"user", user.GetUserID() },
		{"environment", environmentID}
	};
	jsonData["version"] = user.GetVersion().GetString(false);

	strData = jsonData.dump();
	DEBUG_LINEOUT("Create Environment User JSON: %s", strData.c_str());

	m_fPendingMessage = true;
	m_state = state::CREATING_ENVIRONMENT_USER;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

RESULT EnvironmentController::GetEnvironmentPeerList(User user) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	long environmentID = user.GetDefaultEnvironmentID();

	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	m_pendingMessageID = UID().GetID();
	jsonData["id"] = m_pendingMessageID;
	jsonData["token"] = user.GetToken();
	jsonData["method"] = "environmentuser.list";
	jsonData["params"] = "";
	jsonData["version"] = user.GetVersion().GetString(false);

	strData = jsonData.dump();
	DEBUG_LINEOUT("Get Environment User List JSON: %s", strData.c_str());

	m_fPendingMessage = true;
	m_state = state::ENVIRONMENT_PEER_LIST_REQUESTED;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

RESULT EnvironmentController::PrintEnvironmentPeerList() {
	DEBUG_LINEOUT("%d Peers Environment: %d", m_environmentPeers.size(), m_environment.GetEnvironmentID());
	for (auto &peer : m_environmentPeers)
		peer.PrintEnvironmentPeer();

	return R_PASS;
}

bool EnvironmentController::FindPeerByUserID(long userID) {
	for (auto &peer : m_environmentPeers) 
		if (peer.GetUserID() == userID)
			return true;
	
	return false;
}

RESULT EnvironmentController::ClearPeerList() {
	m_environmentPeers.clear();
	return R_PASS;
}

RESULT EnvironmentController::AddNewPeer(long userID, long environmentID, const std::string& strSDPOffer) {
	RESULT r = R_PASS;

	CBM((FindPeerByUserID(userID) == false), "User %d already in environment peer list", userID);
	CBM((s_user.GetUserID() != userID), "User ID %d is self ID - not allowed", userID);
	
	{
		EnvironmentPeer environmentPeer(userID, environmentID, strSDPOffer);
		m_environmentPeers.push_back(environmentPeer);
	}

Error:
	return r;
}

void EnvironmentController::HandleWebsocketMessage(const std::string& strMessage) {
	DEBUG_LINEOUT("HandleWebsocketMessage");

	nlohmann::json jsonCloudResponse = nlohmann::json::parse(strMessage);
	
	long id = jsonCloudResponse["/id"_json_pointer].get<long>();
	long statusCode = jsonCloudResponse["/meta/status_code"_json_pointer].get<long>();

	DEBUG_LINEOUT("HandleWebsocketMessage id:%d statuscode: %d pending:%d pending id:%d state: 0x%x", 
		id, statusCode, m_fPendingMessage, m_pendingMessageID, m_state);

	if (m_fPendingMessage && (m_pendingMessageID == id)) {
		m_fPendingMessage = false;
		m_pendingMessageID = 0;

		switch (m_state) {
			case state::CREATING_ENVIRONMENT_USER: {
				// Environment User Created 
				DEBUG_LINEOUT("Environment User Created Successfully");
				m_state = state::ENVIRONMENT_USER_CREATED;
				GetEnvironmentPeerList(s_user); // TODO: s_user is dumb
			} break;

			case state::ENVIRONMENT_PEER_LIST_REQUESTED: {
				// Peer list received 
				DEBUG_LINEOUT("Environment Peer List Received");
				m_state = state::ENVIRONMENT_PEER_LIST_RECEIVED;
				ClearPeerList();

				// Save environment peer list
				nlohmann::json jsonPeerList = jsonCloudResponse["/data"_json_pointer];

				if (jsonPeerList.size() > 0) {
					for (nlohmann::json::iterator it = jsonPeerList.begin(); it != jsonPeerList.end(); ++it) {
						nlohmann::json jsonPeer = (*it);
						long userID = jsonPeer["/user"_json_pointer].get<long>();
						long environmentID = jsonPeer["/environment"_json_pointer].get<long>();
						std::string strSDPOffer = jsonPeer["/sdp_offer"_json_pointer].get<std::string>();

						AddNewPeer(userID, environmentID, strSDPOffer);
					}
				}

				PrintEnvironmentPeerList();

				DEBUG_LINEOUT("Environment connected and ready");
				m_state = state::ENVIRONMENT_CONNECTED_AND_READY;

			} break;

			case state::ENVIRONMENT_CONNECTED_AND_READY: {
				// TODO: Handle the message here - depending on the type
			} break;
		}
	}
	else {
		// Error
	}

	// Handle message



}

void EnvironmentController::HandleWebsocketConnectionOpen() {
	DEBUG_LINEOUT("HandleWebsocketConnectionOpen");

	m_state = state::SOCKET_CONNECTED;

	// Try to Create Environment User
	// Create Environment User
	// TODO: This is dumb
	CreateEnvironmentUser(s_user);
}

void EnvironmentController::HandleWebsocketConnectionClose() {
	DEBUG_LINEOUT("HandleWebsocketConnectionClose");
}

void EnvironmentController::HandleWebsocketConnectionFailed() {
	DEBUG_LINEOUT("HandleWebsocketConnectionFailed");
}
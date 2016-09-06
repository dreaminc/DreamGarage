#include "Cloud/CloudController.h"
#include "EnvironmentController.h"
#include "Cloud/User/User.h"

#include "Cloud/Websockets/Websocket.h"
#include "Sandbox/CommandLineManager.h"
#include "json.hpp"

#include "PeerConnection.h"

#include "Primitives/Types/UID.h"
#include "Primitives/Types/guid.h"

EnvironmentController::EnvironmentController(Controller* pParentController, long environmentID) :
	Controller(pParentController),
	m_fConnected(false),
	m_fPendingMessage(false),
	m_pEnvironmentWebsocket(nullptr),
	m_pendingMessageID(0),
	m_state(state::UNINITIALIZED)
{
	m_environment = Environment(environmentID);

	m_pPeerConnectionController = std::unique_ptr<PeerConnectionController>(new PeerConnectionController(this));
}

EnvironmentController::~EnvironmentController() {
	if (m_pEnvironmentWebsocket != nullptr) {
		m_pEnvironmentWebsocket->Stop();
	}
}

RESULT EnvironmentController::Initialize() {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->Initialize());
	CR(m_pPeerConnectionController->RegisterPeerConnectionControllerObserver(this));

Error:
	return r;
}

RESULT EnvironmentController::RegisterEnvironmentControllerObserver(EnvironmentControllerObserver* pEnvironmentControllerObserver) {
	RESULT r = R_PASS;

	CNM((pEnvironmentControllerObserver), "Observer cannot be nullptr");
	CBM((m_pEnvironmentControllerObserver == nullptr), "Can't overwrite environment observer");
	m_pEnvironmentControllerObserver = pEnvironmentControllerObserver;

Error:
	return r;
}

std::string EnvironmentController::GetMethodURI(EnvironmentMethod userMethod) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strURI = "";
	int port = std::stoi(pCommandLineManager->GetParameterValue("port"));
	std::string strIP = pCommandLineManager->GetParameterValue("ip");

	switch (userMethod) {
		case EnvironmentMethod::CONNECT_SOCKET: {
			strURI = "ws://" + strIP + ":" + std::to_string(port) + "/environment/";
		} break;
	}

	return strURI;
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

	std::string strURI = GetMethodURI(EnvironmentMethod::CONNECT_SOCKET);
	strURI += std::to_string(m_environment.GetEnvironmentID()); 
	strURI += "/";

	DEBUG_LINEOUT("Connceting to environment socket URL: %s", strURI.c_str());

	// TODO: Not hard coded!
	if (m_pEnvironmentWebsocket == nullptr) {
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

// TODO: this might be dead code
RESULT EnvironmentController::CreateEnvironmentUser(User user) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	std::string strSDPOffer;
	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());
	guid guidMessage;

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");
	
	//strSDPOffer = pParentCloudController->GetSDPOfferString();

	// Set up the JSON data

	// TODO: change pending ID to guid instead
	//m_pendingMessageID = guidMessage.GetGUIDString();

	jsonData["id"] = guidMessage.GetGUIDString();
	jsonData["token"] = user.GetToken();
	jsonData["method"] = "environmentuser.create";
	jsonData["params"] = {
		//{"sdp_offer", "{'foo': 'bar2'}"},
		{ "sdp_offer", strSDPOffer },
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

// TODO: Move to PeerConnection for PeerConnection related calls?
nlohmann::json EnvironmentController::CreateEnvironmentMessage(User user, PeerConnection *pPeerConnection, std::string strMethod) {
	nlohmann::json jsonData;

	// Set up the JSON data
	guid guidMessage = guid();
	std::string strGUID = guidMessage.GetGUIDString();

	jsonData["id"] = guidMessage.GetGUIDString();
	jsonData["token"] = user.GetToken();
	//jsonData["type"] = "response";
	jsonData["type"] = "request";

	//jsonData["method"] = std::string("environmentuser") + strMethod;
	jsonData["method"] = strMethod;

	jsonData["payload"] = nlohmann::json::object();
	if (pPeerConnection != nullptr) {
		jsonData["payload"]["peer_connection"] = pPeerConnection->GetPeerConnectionJSON();
	}

	jsonData["version"] = user.GetVersion().GetString(false);

	return jsonData;
}

RESULT EnvironmentController::SetSDPOffer(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	
	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_offer");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set SDP Offer JSON: %s", strData.c_str());

	m_fPendingMessage = true;
	m_state = state::SET_SDP_OFFER;

	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

RESULT EnvironmentController::SetSDPAnswer(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;

	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_answer");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set SDP Offer JSON: %s", strData.c_str());

	m_fPendingMessage = true;
	m_state = state::SET_SDP_OFFER;

	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

RESULT EnvironmentController::SetOfferCandidates(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;

	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_offer_candidates");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set Offer Candidates JSON: %s", strData.c_str());

	m_fPendingMessage = true;
	m_state = state::SET_OFFER_CANDIDATES;

	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

// TODO: Lots of duplicated code 
RESULT EnvironmentController::SetAnswerCandidates(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;

	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_answer_candidates");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set Answer Candidates JSON: %s", strData.c_str());

	m_fPendingMessage = true;
	m_state = state::SET_ANSWER_CANDIDATES;

	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

// TODO: Right now this is no different than create user... 
RESULT EnvironmentController::UpdateEnvironmentUser() {
	RESULT r = R_PASS;

	CR(CreateEnvironmentUser(s_user));

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
	DEBUG_LINEOUT("%d Peers Environment: %d", (int)(m_environmentPeers.size()), (int)(m_environment.GetEnvironmentID()));
	for (auto &peer : m_environmentPeers)
		peer.PrintEnvironmentPeer();

	return R_PASS;
}

EnvironmentPeer *EnvironmentController::GetPeerByUserID(long userID) {
	for (auto &peer : m_environmentPeers)
		if (peer.GetUserID() == userID)
			return &(peer);

	return nullptr;
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

RESULT EnvironmentController::UpdatePeer(long userID, long environmentID, const std::string& strSDPOffer) {
	RESULT r = R_PASS;

	EnvironmentPeer *pPeer = GetPeerByUserID(userID);
	CNM(pPeer, "User ID %d not found in peer list", userID);

	CR(pPeer->UpdateSDPOffer(strSDPOffer));

	DEBUG_LINEOUT("Peer %d updated", userID);

Error:
	return r;
}

RESULT EnvironmentController::AddNewPeer(long userID, long environmentID, const std::string& strSDPOffer) {
	RESULT r = R_PASS;

	CBM((s_user.GetUserID() != userID), "User ID %d is self ID - not allowed", userID);
	
	{
		EnvironmentPeer environmentPeer(userID, environmentID, strSDPOffer);
		m_environmentPeers.push_back(environmentPeer);
	}

Error:
	return r;
}

std::vector<std::string> TokenizeString(std::string str, char cDelim) {
	std::istringstream strStream(str);
	std::vector<std::string> strTokens;
	std::string strToken;

	while (std::getline(strStream, strToken, cDelim)) {
		if (!strToken.empty()) {
			strTokens.push_back(strToken);
		}
	}

	return strTokens;
}

bool EnvironmentController::IsUserIDConnected(long peerUserID) {
	return m_pPeerConnectionController->IsUserIDConnected(peerUserID);
}

/*
// TODO: This is temp
RESULT EnvironmentController::InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->InitializeNewPeerConnection(fCreateOffer, fAddDataChannel));

Error:
	return r;
}
*/

RESULT EnvironmentController::OnSDPOfferSuccess(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	// TODO: Fix the s_user bullshit

	CBM((pPeerConnection->GetOfferUserID() == s_user.GetUserID()), "User ID mismatch offer user ID of peer connection");
	CR(SetSDPOffer(s_user, pPeerConnection));

	// TOOD: based on pPeerConnection vs username answer or answer

Error:
	return r;
}

RESULT EnvironmentController::OnSDPAnswerSuccess(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	// TODO: Fix the s_user bullshit
	CBM((pPeerConnection->GetAnswerUserID() == s_user.GetUserID()), "User ID mismatch answer user ID of peer connection");
	CR(SetSDPAnswer(s_user, pPeerConnection));

	// TOOD: based on pPeerConnection vs username answer or answer

Error:
	return r;
}

RESULT EnvironmentController::OnICECandidatesGatheringDone(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	if (pPeerConnection->GetOfferUserID() == s_user.GetUserID()) {
		CR(SetOfferCandidates(s_user, pPeerConnection));
	}
	else if (pPeerConnection->GetAnswerUserID() == s_user.GetUserID()) {
		CR(SetAnswerCandidates(s_user, pPeerConnection));
	}


Error:
	return r;
}

void EnvironmentController::HandleWebsocketMessage(const std::string& strMessage) {
	DEBUG_LINEOUT("HandleWebsocketMessage");

	nlohmann::json jsonCloudMessage = nlohmann::json::parse(strMessage);
	
	std::string strGUID = jsonCloudMessage["/id"_json_pointer].get<std::string>();
	std::string strType = jsonCloudMessage["/type"_json_pointer].get<std::string>();
	std::string strMethod = jsonCloudMessage["/method"_json_pointer].get<std::string>();
	
	std::vector<std::string> strTokens = TokenizeString(strMethod, '.');

	// Determine who to handle this
	if (strTokens[0] == "peer_connection") {
		nlohmann::json jsonPayload = jsonCloudMessage["/payload"_json_pointer];
		strMethod = strTokens[1];

		if (strType == "request") {
			m_pPeerConnectionController->HandleEnvironmentSocketRequest(strMethod, jsonPayload);
		}
		else if (strType == "response") {
			m_pPeerConnectionController->HandleEnvironmentSocketResponse(strMethod, jsonPayload);
		}
	}

	/*
	DEBUG_LINEOUT("HandleWebsocketMessage id:%d statuscode: %d pending:%d pending id:%d state: 0x%x", 
		(int)(id), (int)(statusCode), m_fPendingMessage, (int)(m_pendingMessageID), m_state);

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

			
		}
	}
	else {
		CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());
		if (pParentCloudController != nullptr) {
			switch (m_state) {
				case state::ENVIRONMENT_CONNECTED_AND_READY: {
					// TODO: Message type not implemented
					DEBUG_LINEOUT("New Environment Peer Connected");
					
					/*
					m_state = state::ENVIRONMENT_CONNECTED_AND_READY;
					GetEnvironmentPeerList(s_user); // TODO: s_user is dumb
					//*//*

					/*
					nlohmann::json jsonPeer = jsonCloudResponse["/data"_json_pointer];

					long userID = jsonPeer["/user"_json_pointer].get<long>();
					long environmentID = jsonPeer["/environment"_json_pointer].get<long>();

					std::string strSDPOffer = jsonPeer["/sdp_offer"_json_pointer].get<std::string>();

					if (FindPeerByUserID(userID)) {
						UpdatePeer(userID, environmentID, strSDPOffer);
					}
					else {
						AddNewPeer(userID, environmentID, strSDPOffer);
					}

					PrintEnvironmentPeerList();


					pParentCloudController->CreateSDPOfferAnswer(strSDPOffer);

					/*
					// Attempt to connect here
					std::string strType = jsonPeer["sdp_offer"]["type"].get<std::string>();
					if (strType == "offer") {
						pParentCloudController->CreateSDPOfferAnswer(strSDPOffer);
					}
					else if (strType == "answer") {
						// This is the update to the SDP info - so it's an answer and has the candidates
						// TODO: ICE Candidates / or change the name to handle SDP offer updates
						pParentCloudController->CreateSDPOfferAnswer(strSDPOffer);
					}
					//*//*
				} break;
			}
		}
		else {
			DEBUG_LINEOUT("Cannot handle message, cloud controller not found");
		}
	}
	*/
}

void EnvironmentController::HandleWebsocketConnectionOpen() {
	DEBUG_LINEOUT("HandleWebsocketConnectionOpen");

	m_state = state::SOCKET_CONNECTED;

	// Try to Create Environment User
	// Create Environment User
	// TODO: This is dumb
	//CreateEnvironmentUser(s_user);
}

void EnvironmentController::HandleWebsocketConnectionClose() {
	DEBUG_LINEOUT("HandleWebsocketConnectionClose");
}

void EnvironmentController::HandleWebsocketConnectionFailed() {
	DEBUG_LINEOUT("HandleWebsocketConnectionFailed");
}

RESULT EnvironmentController::SendDataChannelStringMessage(int peerID, std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->SendDataChannelStringMessage(peerID, strMessage));

Error:
	return r;
}

RESULT EnvironmentController::SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->SendDataChannelMessage(peerID, pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT EnvironmentController::OnDataChannelStringMessage(const std::string& strDataChannelMessage) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnDataChannelStringMessage(strDataChannelMessage));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));
	}

Error:
	return r;
}
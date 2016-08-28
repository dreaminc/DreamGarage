#include "PeerConnectionController.h"
#include "PeerConnection.h"

#include "Sandbox/CommandLineManager.h"

std::string PeerConnectionController::GetMethodURI(PeerConnectionMethod method) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	
	std::string strURI = "";
	
	int port = std::stoi(pCommandLineManager->GetParameterValue("port"));
	std::string strIP = pCommandLineManager->GetParameterValue("ip");

	// TODO:
	switch (method) {
		case PeerConnectionMethod::INVALID:
		default: {
			//strURI = "ws://" + strIP + ":" + std::to_string(port) + "/environment/";
			strURI = "ws://" + strIP + ":" + std::to_string(port);
		} break;
	}

	return strURI;
}

PeerConnectionController::PeerConnectionController(Controller* pParentController) :
	Controller(pParentController),
	m_pWebRTCImp(nullptr)
{
	// empty
}

RESULT PeerConnectionController::Initialize() {
	RESULT r = R_PASS;

	m_pWebRTCImp = std::make_unique<WebRTCImp>(GetCloudController());
	CN(m_pWebRTCImp);

	CR(m_pWebRTCImp->Initialize());
	CR(m_pWebRTCImp->RegisterObserver(this));

Error:
	return r;
}

RESULT PeerConnectionController::InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	CN(m_pWebRTCImp);
	CRM(m_pWebRTCImp->InitializePeerConnection(fCreateOffer, fAddDataChannel), "Failed to initialize WebRTC Peer Connection");

Error:
	return r;
}

PeerConnectionController::~PeerConnectionController() {
	// empty
}

RESULT PeerConnectionController::ClearPeerConnections() {
	m_peerConnections.clear();
	return R_PASS;
}

bool PeerConnectionController::FindPeerConnectionByUserID(long userID) {
	if (GetPeerConnectionByUserID(userID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByUserID(long userID) {
	for (auto &peerConnection : m_peerConnections) {
		if (userID == peerConnection.GetUserID()) {
			return &(peerConnection);
		}
	}

	return nullptr;
}

bool PeerConnectionController::FindPeerConnectionByPeerUserID(long peerUserID) {
	if (GetPeerConnectionByPeerUserID(peerUserID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByPeerUserID(long peerUserID) {
	for (auto &peerConnection : m_peerConnections) {
		if (peerUserID == peerConnection.GetPeerUserID()) {
			return &(peerConnection);
		}
	}

	return nullptr;
}

bool PeerConnectionController::FindPeerConnectionByID(long peerConnectionID) {
	if (GetPeerConnectionByID(peerConnectionID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByID(long peerConnectionID) {
	for (auto &peerConnection : m_peerConnections) {
		if (peerConnectionID == peerConnection.GetPeerConnectionID()) {
			return &(peerConnection);
		}
	}

	return nullptr;
}

PeerConnection* PeerConnectionController::CreateNewPeerConnection(long peerConnectionID, long userID, long peerUserID) {
	//RESULT r = R_PASS;
	PeerConnection *pPeerConnection = nullptr;

	if ((pPeerConnection = GetPeerConnectionByID(peerConnectionID)) != nullptr) {
		DEBUG_LINEOUT("Peer Connection %d already exists", peerConnectionID);
		return pPeerConnection;
	}

	if ((pPeerConnection = GetPeerConnectionByPeerUserID(peerUserID)) != nullptr) {
		DEBUG_LINEOUT("Peer conncetion to peer %d already exists", peerUserID);
		return pPeerConnection;
	}

	PeerConnection peerConnection(userID, peerUserID, peerConnectionID);
	m_peerConnections.push_back(peerConnection);

	pPeerConnection = &(m_peerConnections.back());

//Error:
	return pPeerConnection;
}

RESULT PeerConnectionController::HandleEnvironmentSocketMessage(std::string strMethod, nlohmann::json jsonPayload) {
	RESULT r = R_PASS;

	// Either create a new peer connection or pull the one that we have
	nlohmann::json jsonPeerConnection = jsonPayload["/peer_connection"_json_pointer];
	long peerConnectionID = jsonPeerConnection["/id"_json_pointer].get<long>();

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	
	// The below will create it if it doesn't exist

	if (strMethod == "create_offer") {
		nlohmann::json jsonOfferSocketConnection = jsonPayload["/offer_socket_connection"_json_pointer];
		long userID = jsonOfferSocketConnection["/user"_json_pointer].get<long>();

		nlohmann::json jsonAnswerSocketConnection = jsonPayload["/answer_socket_connection"_json_pointer];
		long peerUserId = jsonAnswerSocketConnection["/user"_json_pointer].get<long>();

		CBM((pPeerConnection == nullptr), "Peer Connection %d already exists", peerConnectionID);
		pPeerConnection = CreateNewPeerConnection(peerConnectionID, userID, peerUserId);

		// Initialize SDP Peer Connection and Offer
		CN(m_pWebRTCImp);
		m_pWebRTCImp->InitializePeerConnection(true);
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnPeerConnectionInitialized() {
	RESULT r = R_PASS;

	// Get the string and send to server
	int a = 5;

//Error:
	return r;
}

RESULT PeerConnectionController::OnICECandidatesGatheringDone() {
	RESULT r = R_NOT_IMPLEMENTED;

	// TODO:

//Error:
	return r;
}

RESULT PeerConnectionController::OnDataChannelStringMessage(const std::string& strDataChannelMessage) {
	RESULT r = R_NOT_IMPLEMENTED;

	// TODO:

//Error:
	return r;
}

RESULT PeerConnectionController::OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_NOT_IMPLEMENTED;

	// TODO:

//Error:
	return r;
}
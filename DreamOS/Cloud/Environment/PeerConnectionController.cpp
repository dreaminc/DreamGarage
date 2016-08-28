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

RESULT PeerConnectionController::RegisterPeerConnectionControllerObserver(PeerConnectionControllerObserver* pPeerConnectionControllerObserver) {
	RESULT r = R_PASS;

	CNM((pPeerConnectionControllerObserver), "Observer cannot be nullptr");
	CBM((m_pPeerConnectionControllerObserver == nullptr), "Can't overwrite peer connection observer");
	m_pPeerConnectionControllerObserver = pPeerConnectionControllerObserver;

Error:
	return r;
}

PeerConnectionController::PeerConnectionController(Controller* pParentController) :
	Controller(pParentController),
	m_pWebRTCImp(nullptr),
	m_pPeerConnectionCurrentHandshake(nullptr)
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

bool PeerConnectionController::FindPeerConnectionByOfferUserID(long userID) {
	if (GetPeerConnectionByOfferUserID(userID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByOfferUserID(long userID) {
	for (auto &peerConnection : m_peerConnections) {
		if (userID == peerConnection.GetOfferUserID()) {
			return &(peerConnection);
		}
	}

	return nullptr;
}

bool PeerConnectionController::FindPeerConnectionByAnswerUserID(long peerUserID) {
	if (GetPeerConnectionByAnswerUserID(peerUserID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByAnswerUserID(long peerUserID) {
	for (auto &peerConnection : m_peerConnections) {
		if (peerUserID == peerConnection.GetAnswerUserID()) {
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

	if ((pPeerConnection = GetPeerConnectionByAnswerUserID(peerUserID)) != nullptr) {
		DEBUG_LINEOUT("Peer conncetion to peer %d already exists", peerUserID);
		return pPeerConnection;
	}

	PeerConnection peerConnection(userID, peerUserID, peerConnectionID);
	m_peerConnections.push_back(peerConnection);

	pPeerConnection = &(m_peerConnections.back());

//Error:
	return pPeerConnection;
}

PeerConnection* PeerConnectionController::CreateNewPeerConnection(nlohmann::json jsonPeerConnection, nlohmann::json jsonOfferSocketConnection, nlohmann::json jsonAnswerSocketConnection) {
	
	PeerConnection peerConnection(jsonPeerConnection, jsonOfferSocketConnection, jsonAnswerSocketConnection);
	PeerConnection *pPeerConnection = nullptr;

	if ((pPeerConnection = GetPeerConnectionByID(peerConnection.GetPeerConnectionID())) != nullptr) {
		DEBUG_LINEOUT("Peer Connection %d already exists", peerConnection.GetPeerConnectionID());
		return pPeerConnection;
	}

	if ((pPeerConnection = GetPeerConnectionByAnswerUserID(peerConnection.GetAnswerUserID())) != nullptr) {
		DEBUG_LINEOUT("Peer conncetion to peer %d already exists", peerConnection.GetAnswerUserID());
		return pPeerConnection;
	}

	m_peerConnections.push_back(peerConnection);
	pPeerConnection = &(m_peerConnections.back());

	//Error:
	return pPeerConnection;
}

RESULT PeerConnectionController::HandleEnvironmentSocketRequest(std::string strMethod, nlohmann::json jsonPayload) {
	RESULT r = R_PASS;

	// Either create a new peer connection or pull the one that we have
	nlohmann::json jsonPeerConnection = jsonPayload["/peer_connection"_json_pointer];
	long peerConnectionID = jsonPeerConnection["/id"_json_pointer].get<long>();

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);

	if (m_pPeerConnectionCurrentHandshake != nullptr) {
		CBM((m_pPeerConnectionCurrentHandshake->GetPeerConnectionID() == peerConnectionID), "Can't negotiate multiple peers at same time");
	}
	
	// The below will create it if it doesn't exist

	if (strMethod == "create_offer") {
		nlohmann::json jsonOfferSocketConnection = jsonPayload["/offer_socket_connection"_json_pointer];
		long userID = jsonOfferSocketConnection["/user"_json_pointer].get<long>();

		nlohmann::json jsonAnswerSocketConnection = jsonPayload["/answer_socket_connection"_json_pointer];
		long peerUserId = jsonAnswerSocketConnection["/user"_json_pointer].get<long>();

		long offerEnvironmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();
		long answerEnvironmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();

		// TODO: Make sure they match

		CBM((pPeerConnection == nullptr), "Peer Connection %d already exists", peerConnectionID);
		//pPeerConnection = CreateNewPeerConnection(peerConnectionID, userID, peerUserId);
		pPeerConnection = CreateNewPeerConnection(jsonPeerConnection, jsonOfferSocketConnection, jsonAnswerSocketConnection);

		m_pPeerConnectionCurrentHandshake = pPeerConnection;

		// Initialize SDP Peer Connection and Offer
		CN(m_pWebRTCImp);
		m_pWebRTCImp->InitializePeerConnection(true);
	}
	else if (strMethod == "create_answer") {
		// TODO: Reproduction of code above - move to function
		nlohmann::json jsonOfferSocketConnection = jsonPayload["/offer_socket_connection"_json_pointer];
		long offerUserID = jsonOfferSocketConnection["/user"_json_pointer].get<long>();

		nlohmann::json jsonAnswerSocketConnection = jsonPayload["/answer_socket_connection"_json_pointer];
		long answerUserId = jsonAnswerSocketConnection["/user"_json_pointer].get<long>();

		long offerEnvironmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();
		long answerEnvironmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();

		// TODO: Make sure they match

		CBM((pPeerConnection == nullptr), "Peer Connection %d already exists", peerConnectionID);
		//pPeerConnection = CreateNewPeerConnection(peerConnectionID, userID, peerUserId);
		pPeerConnection = CreateNewPeerConnection(jsonPeerConnection, jsonOfferSocketConnection, jsonAnswerSocketConnection);

		m_pPeerConnectionCurrentHandshake = pPeerConnection;

		std::string strSDPOffer = pPeerConnection->GetSDPOffer();

		// Initialize SDP Peer Connection Offer and Create Answer
		CN(m_pWebRTCImp);
		CR(m_pWebRTCImp->InitializePeerConnection(false));
		CR(m_pWebRTCImp->CreateSDPOfferAnswer(strSDPOffer));
	}
	else {
		// TODO:
		int a = 5;
	}

Error:
	return r;
}

RESULT PeerConnectionController::HandleEnvironmentSocketResponse(std::string strMethod, nlohmann::json jsonPayload) {
	RESULT r = R_PASS;

	// TODO: Validate state?

//Error:
	return r;
}

RESULT PeerConnectionController::OnSDPOfferSuccess() {
	RESULT r = R_PASS;

	// Create SDP offer response
	// TODO: Multi-peer will need to implement state hold for ID or object
	//CR(m_pPeerConnectionCurrentHandshake->SetSDPOffer(m_pWebRTCImp->GetSDPOfferString()));
	CR(m_pPeerConnectionCurrentHandshake->SetSDPOffer(m_pWebRTCImp->GetSDPString()));

	if (m_pPeerConnectionControllerObserver != nullptr) {
		m_pPeerConnectionControllerObserver->OnSDPOfferSuccess(m_pPeerConnectionCurrentHandshake);
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnSDPAnswerSuccess() {
	RESULT r = R_PASS;

	// Create SDP offer response
	// TODO: Multi-peer will need to implement state hold for ID or object
	//CR(m_pPeerConnectionCurrentHandshake->SetSDPAnswer(m_pWebRTCImp->GetSDPOfferString()));
	CR(m_pPeerConnectionCurrentHandshake->SetSDPAnswer(m_pWebRTCImp->GetSDPString()));

	if (m_pPeerConnectionControllerObserver != nullptr) {
		m_pPeerConnectionControllerObserver->OnSDPAnswerSuccess(m_pPeerConnectionCurrentHandshake);
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnICECandidatesGatheringDone() {
	RESULT r = R_NOT_IMPLEMENTED;

	//CR(m_pPeerConnectionCurrentHandshake->SetSDPOffer(m_pWebRTCImp->GetSDPOfferString()));
	// TODO: Add ICE Candidates to the peer connection

	if (m_pWebRTCImp->IsOfferer()) {
		CR(m_pPeerConnectionCurrentHandshake->SetOfferCandidates(m_pWebRTCImp->GetCandidates()));
	}
	else {
		CR(m_pPeerConnectionCurrentHandshake->SetAnswerCandidates(m_pWebRTCImp->GetCandidates()));
	}

	if (m_pPeerConnectionControllerObserver != nullptr) {
		m_pPeerConnectionControllerObserver->OnICECandidatesGatheringDone(m_pPeerConnectionCurrentHandshake);
	}


Error:
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
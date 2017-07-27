#include "Logger/Logger.h"
#include "easylogging++.h"

#include "PeerConnectionController.h"
#include "PeerConnection.h"

#include "Sandbox/CommandLineManager.h"

std::string PeerConnectionController::GetMethodURI(PeerConnectionMethod method) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	
	std::string strURI = "";
	std::string ip = pCommandLineManager->GetParameterValue("ws.ip");

	// TODO:
	switch (method) {
		case PeerConnectionMethod::INVALID:
		default: {
			//strURI = "ws://" + strIP + ":" + std::to_string(port) + "/environment/";
			strURI = ip;
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
	m_pWebRTCImp(nullptr)
	//DEADBEEF: m_pPeerConnectionCurrentHandshake(nullptr)
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

/*
RESULT PeerConnectionController::InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	CN(m_pWebRTCImp);
	CRM(m_pWebRTCImp->InitializePeerConnection(fCreateOffer, fAddDataChannel), "Failed to initialize WebRTC Peer Connection");

Error:
	return r;
}
*/

PeerConnectionController::~PeerConnectionController() {
	// empty
}

RESULT PeerConnectionController::ClearPeerConnections() {
	m_peerConnections.clear();
	return R_PASS;
}

bool PeerConnectionController::FindPeerConnectionByOfferUserID(long offerUserID) {
	if (GetPeerConnectionByOfferUserID(offerUserID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByOfferUserID(long offerUserID) {
	for (auto &peerConnection : m_peerConnections) {
		if (offerUserID == peerConnection.GetOfferUserID()) {
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

bool PeerConnectionController::FindPeerConnectionByAnswerUserID(long answerUserID) {
	if (GetPeerConnectionByAnswerUserID(answerUserID) == nullptr)
		return false;
	else
		return true;
}

PeerConnection *PeerConnectionController::GetPeerConnectionByAnswerUserID(long answerUserID) {
	for (auto &peerConnection : m_peerConnections) {
		if (answerUserID == peerConnection.GetAnswerUserID()) {
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

bool PeerConnectionController::IsUserIDConnected(long peerUserID) {
	// TODO: Fix this so it works with user IDs
	//PeerConnection *pPeerConnection = GetPeerConnectionByAnswerUserID(peerUserID);
	
	//if (pPeerConnection != nullptr && pPeerConnection->IsWebRTCConnected()) {
	if (m_peerConnections.size() != 0 && (*(m_peerConnections.begin())).IsWebRTCConnected()) {
		return true;
	}
	else {
		return false;
	}
}

// DEADBEEF:?
/*
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

	PeerConnection peerConnection(userID, userID, peerUserID, peerConnectionID);
	m_peerConnections.push_back(peerConnection);

	pPeerConnection = &(m_peerConnections.back());

//Error:
	return pPeerConnection;
}
*/

PeerConnection* PeerConnectionController::CreateNewPeerConnection(long userID, nlohmann::json jsonPeerConnection, nlohmann::json jsonOfferSocketConnection, nlohmann::json jsonAnswerSocketConnection) {
	
	PeerConnection peerConnection(userID, jsonPeerConnection, jsonOfferSocketConnection, jsonAnswerSocketConnection);
	PeerConnection *pPeerConnection = nullptr;

	if ((pPeerConnection = GetPeerConnectionByID(peerConnection.GetPeerConnectionID())) != nullptr) {
		LOG(INFO) << "(cloud) creating a peer already found by connection";
		DEBUG_LINEOUT("Peer Connection %d already exists", peerConnection.GetPeerConnectionID());
		return pPeerConnection;
	}

	m_peerConnections.push_back(peerConnection);
	pPeerConnection = &(m_peerConnections.back());

	//Error:
	return pPeerConnection;
}

RESULT PeerConnectionController::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	long position = (fOfferor) ? pPeerConnection->GetOfferorPosition() : pPeerConnection->GetAnswererPosition();
	
	LOG(INFO) << "myUserID" << (fOfferor ? "(Offeror)" : "(Answerer)") << "=" << userID << " peerUserID=" << peerUserID << " position=" << position;
	DEBUG_LINEOUT("%s: myUserID: %d peerUserID: %d, position: %d", (fOfferor ? "Offeror" : "Answerer"), userID, peerUserID, position);

	if (m_pPeerConnectionControllerObserver != nullptr) {
		//CR(m_pPeerConnectionControllerObserver->OnNewPeerConnection(position - 1));
		CR(m_pPeerConnectionControllerObserver->OnNewPeerConnection(userID, peerUserID, fOfferor, pPeerConnection));
	}

Error:
	return r;
}

RESULT PeerConnectionController::HandleEnvironmentSocketRequest(std::string strMethod, nlohmann::json jsonPayload) {
	RESULT r = R_PASS;

	// Either create a new peer connection or pull the one that we have
	nlohmann::json jsonPeerConnection = jsonPayload["/peer_connection"_json_pointer];
	long peerConnectionID = jsonPeerConnection["/id"_json_pointer].get<long>();

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	long userID = GetUserID();

	/*
	// DEADBEEF: No longer true
	if (m_pPeerConnectionCurrentHandshake != nullptr) {
		CBM((m_pPeerConnectionCurrentHandshake->GetPeerConnectionID() == peerConnectionID), "Can't negotiate multiple peers at same time");
	}
	*/
	
	// The below will create it if it doesn't exist

	nlohmann::json jsonOfferSocketConnection = jsonPayload["/offer_socket_connection"_json_pointer];
	long offerUserID = jsonOfferSocketConnection["/user"_json_pointer].get<long>();

	nlohmann::json jsonAnswerSocketConnection = jsonPayload["/answer_socket_connection"_json_pointer];
	long answerUserId = jsonAnswerSocketConnection["/user"_json_pointer].get<long>();

	long offerEnvironmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();
	long answerEnvironmentID = jsonAnswerSocketConnection["/environment"_json_pointer].get<long>();
	
	// TODO: Make sure they match

	CBM((userID != -1), "User does not seem to be signed in");

	if (strMethod == "create_offer") {
		if (userID != offerUserID) {
			LOG(ERROR) << "(cloud) requested offer for wrong user. payload=" << jsonPayload;
			return R_FAIL;
		}

		LOG(INFO) << "(cloud) creating a new peer (id=" << peerConnectionID << ") between user ids (" << userID << "[offeror,self]->" << answerUserId << "[answerer])";

		CBM((pPeerConnection == nullptr), "Peer Connection %d already exists", peerConnectionID);
		pPeerConnection = CreateNewPeerConnection(userID, jsonPeerConnection, jsonOfferSocketConnection, jsonAnswerSocketConnection);
		// DEADBEEF: No longer true
		//m_pPeerConnectionCurrentHandshake = pPeerConnection;

		// Initialize SDP Peer Connection and Offer
		CN(m_pWebRTCImp);
		//m_pWebRTCImp->InitializePeerConnection(true);
		m_pWebRTCImp->InitializeNewPeerConnection(peerConnectionID, true);

		CR(OnNewPeerConnection(userID, pPeerConnection->GetPeerUserID(), true, pPeerConnection));
	}
	else if (strMethod == "set_offer") {
		LOG(ERROR) << "(cloud) set offer should not be a request";
		CNM((pPeerConnection), "Peer Connection %d doesn't exist", peerConnectionID);

		// DEADBEEF: No longer true
		//CBM((m_pPeerConnectionCurrentHandshake == pPeerConnection), "Peer connection mis matches current handshake connection");

		//pPeerConnection->UpdatePeerConnectionFromJSON(jsonPeerConnection);
	}
	else if (strMethod == "create_answer") {
		if (userID != answerUserId) {
			LOG(ERROR) << "(cloud) requested answer for wrong user. payload=" << jsonPayload;
			return R_FAIL;
		}

		LOG(INFO) << "(cloud) creating a new peer (id=" << peerConnectionID << ") between user ids (" << offerUserID << "[offeror]->" << answerUserId << "[answerer,self])";

		CBM((pPeerConnection == nullptr), "Peer Connection %d already exists", peerConnectionID);
		pPeerConnection = CreateNewPeerConnection(userID, jsonPeerConnection, jsonOfferSocketConnection, jsonAnswerSocketConnection);
		// DEADBEEF: No longer true
		//m_pPeerConnectionCurrentHandshake = pPeerConnection;	

		std::string strSDPOffer = pPeerConnection->GetSDPOffer();

		// Initialize SDP Peer Connection Offer and Create Answer
		CN(m_pWebRTCImp);
		//CR(m_pWebRTCImp->InitializePeerConnection(false));
		m_pWebRTCImp->InitializeNewPeerConnection(peerConnectionID, false);
		CR(m_pWebRTCImp->CreateSDPOfferAnswer(peerConnectionID, strSDPOffer));

		CR(OnNewPeerConnection(userID, pPeerConnection->GetPeerUserID(), false, pPeerConnection));
	}
	else if (strMethod == "set_offer_candidates") {
		CNM((pPeerConnection), "Peer Connection %d doesn't exist", peerConnectionID);
		
		// DEADBEEF: No longer true
		//CBM((m_pPeerConnectionCurrentHandshake == pPeerConnection), "Peer connection mis matches current handshake connection");
		
		pPeerConnection->UpdatePeerConnectionFromJSON(jsonPeerConnection);

		// Initialize SDP Peer Connection Offer and Create Answer
		CN(m_pWebRTCImp);
		//CR(m_pWebRTCImp->CreateSDPOfferAnswer(strSDPOffer));

		//CR(m_pWebRTCImp->AddOfferCandidates(pPeerConnection));

		// TODO: Add Candidates
		///*
		if ((m_pWebRTCImp->IsOfferer(peerConnectionID) == false) /*&& m_pPeerConnectionCurrentHandshake->IsWebRTCConnectionStable()*/) {
			CBM((pPeerConnection->GetOfferCandidates().size() > 0), "Can't add answer candidates since there are none");
			CRM(m_pWebRTCImp->AddOfferCandidates(pPeerConnection), "Failed to add Peer Connection answer candidates");
		}
		//*/
	}
	else if (strMethod == "set_answer") {
		CNM((pPeerConnection), "Peer Connection %d doesn't exist", peerConnectionID);

		// DEADBEEF:
		//CBM((m_pPeerConnectionCurrentHandshake == pPeerConnection), "Peer connection mis matches current handshake connection");

		pPeerConnection->UpdatePeerConnectionFromJSON(jsonPeerConnection);

		// TODO: This is a bit of a hack - but setting the answer description here from the Answer SDP 
		// will ultimately signal the WebRTC connection to be complete	
		CN(m_pWebRTCImp);
		CR(m_pWebRTCImp->SetSDPAnswer(peerConnectionID, pPeerConnection->GetSDPAnswer()));
		//CR(m_pWebRTCImp->SetSDPAnswer(pPeerConnection->GetSDPAnswer()));
	

		// We don't have a guarantee that the WebRTC connection is stable at this point

		// Initialize SDP Peer Connection Offer and Create Answer
		//CN(m_pWebRTCImp);

		// We can do this now - since we are guaranteed to already have our local SDP 
		//CR(m_pWebRTCImp->AddAnswerCandidates(pPeerConnection));

		//CR(m_pWebRTCImp->CreateSDPOfferAnswer(strSDPOffer));

		// TODO: Add Candidates
		// At this point the whole thing is complete
		//pPeerConnection->Print();

		//RESULT WebRTCConductor::AddIceCandidate(ICECandidate iceCandidate) {
	}
	else if (strMethod == "set_answer_candidates") {
		CNM((pPeerConnection), "Peer Connection %d doesn't exist", peerConnectionID);

		// DEADBEEF:
		//CBM((m_pPeerConnectionCurrentHandshake == pPeerConnection), "Peer connection mis matches current handshake connection");

		pPeerConnection->UpdatePeerConnectionFromJSON(jsonPeerConnection);

		// TODO: This is a bit of a hack - but setting the answer description here from the Answer SDP 
		// will ultimately signal the WebRTC connection to be complete
		CN(m_pWebRTCImp);
		//CR(m_pWebRTCImp->SetSDPAnswer(pPeerConnection->GetSDPAnswer()));

		///*
		if ((m_pWebRTCImp->IsOfferer(peerConnectionID) == true) /*&& m_pPeerConnectionCurrentHandshake->IsWebRTCConnectionStable()*/) {
			CBM((pPeerConnection->GetAnswerCandidates().size() > 0), "Can't add answer candidates since there are none");
			CRM(m_pWebRTCImp->AddAnswerCandidates(pPeerConnection), "Failed to add Peer Connection answer candidates");
		}
		//*/

		// We don't have a guarantee that the WebRTC connection is stable at this point

		// Initialize SDP Peer Connection Offer and Create Answer
		//CN(m_pWebRTCImp);

		// We can do this now - since we are guaranteed to already have our local SDP 
		//CR(m_pWebRTCImp->AddAnswerCandidates(pPeerConnection));

		//CR(m_pWebRTCImp->CreateSDPOfferAnswer(strSDPOffer));

		// TODO: Add Candidates
		// At this point the whole thing is complete
		//pPeerConnection->Print();
		
		//RESULT WebRTCConductor::AddIceCandidate(ICECandidate iceCandidate) {
	}
	else
	{
		LOG(ERROR) << "(cloud) method unknown";
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnWebRTCConnectionStable(long peerConnectionID) {
	RESULT r = R_PASS;
	
	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	CR(pPeerConnection->SetWebRTCConnectionStable());

	/*
	CNM(m_pPeerConnectionCurrentHandshake, "WebRTC Connection stable without current peer connection ERROR!");
	m_pPeerConnectionCurrentHandshake->SetWebRTCConnectionStable();
	*/

	// If we're the offerer, we add the answer candidates - if we're the answerer we add the offers candidates
	// At this point the WebRTC connection is stable so we're guaranteed to have the remote description
	// TODO: This could in theory be done when we have the remote description - so could be optimized 
	/*
	if (m_pWebRTCImp->IsOfferer()) {
		CBM((m_pPeerConnectionCurrentHandshake->GetAnswerCandidates().size() > 0), "Can't add answer candidates since there are none");
		CRM(m_pWebRTCImp->AddAnswerCandidates(m_pPeerConnectionCurrentHandshake), "Failed to add Peer Connection answer candidates");
	}
	else {
		CBM((m_pPeerConnectionCurrentHandshake->GetOfferCandidates().size() > 0), "Can't add answer candidates since there are none");
		CRM(m_pWebRTCImp->AddOfferCandidates(m_pPeerConnectionCurrentHandshake), "Failed to add Peer Connection answer candidates");
	}
	//*/

Error:
	return r;
}

RESULT PeerConnectionController::HandleEnvironmentSocketResponse(std::string strMethod, nlohmann::json jsonPayload) {
	RESULT r = R_PASS;

	// TODO: Validate state?

//Error:
	return r;
}

RESULT PeerConnectionController::OnWebRTCConnectionClosed(long peerConnectionID) {
	RESULT r = R_PASS;

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	DEBUG_LINEOUT("Peer Connection %d Closed", peerConnectionID);

	// TODO: Remove peer connection from list here

Error:
	return r;
}

// TODO: This might not be needed
// Handshake moderation might get in the way
RESULT PeerConnectionController::OnSDPOfferSuccess(long peerConnectionID) {
	RESULT r = R_PASS;

	// Create SDP offer response
	// TODO: Multi-peer will need to implement state hold for ID or object
	//CR(m_pPeerConnectionCurrentHandshake->SetSDPOffer(m_pWebRTCImp->GetSDPOfferString()));
	//long peerConnectionID = m_pPeerConnectionCurrentHandshake->GetPeerConnectionID();
	
	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	CR(pPeerConnection->SetSDPOffer(m_pWebRTCImp->GetLocalSDPString(peerConnectionID)));

	if (m_pPeerConnectionControllerObserver != nullptr) {
		m_pPeerConnectionControllerObserver->OnSDPOfferSuccess(pPeerConnection);
	}

Error:
	return r;
}

// TODO: This might not be needed
// Handshake moderation might get in the way
RESULT PeerConnectionController::OnSDPAnswerSuccess(long peerConnectionID) {
	RESULT r = R_PASS;

	// Create SDP offer response
	// TODO: Multi-peer will need to implement state hold for ID or object
	//CR(m_pPeerConnectionCurrentHandshake->SetSDPAnswer(m_pWebRTCImp->GetSDPOfferString()));

	//long peerConnectionID = m_pPeerConnectionCurrentHandshake->GetPeerConnectionID();
	//CR(m_pPeerConnectionCurrentHandshake->SetSDPAnswer(m_pWebRTCImp->GetRemoteSDPString(peerConnectionID)));

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	//CR(pPeerConnection->SetSDPAnswer(m_pWebRTCImp->GetRemoteSDPString(peerConnectionID)));
	CR(pPeerConnection->SetSDPAnswer(m_pWebRTCImp->GetLocalSDPString(peerConnectionID)));

	if (m_pPeerConnectionControllerObserver != nullptr) {
		m_pPeerConnectionControllerObserver->OnSDPAnswerSuccess(pPeerConnection);
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnICECandidatesGatheringDone(long peerConnectionID) {
	RESULT r = R_PASS;

	//CR(m_pPeerConnectionCurrentHandshake->SetSDPOffer(m_pWebRTCImp->GetSDPOfferString()));
	// TODO: Add ICE Candidates to the peer connection

	//long peerConnectionID = m_pPeerConnectionCurrentHandshake->GetPeerConnectionID();

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	if (m_pWebRTCImp->IsOfferer(peerConnectionID)) {
		CR(pPeerConnection->SetOfferCandidates(m_pWebRTCImp->GetCandidates(peerConnectionID)));
	}
	else {
		CR(pPeerConnection->SetAnswerCandidates(m_pWebRTCImp->GetCandidates(peerConnectionID)));
	}

	if (m_pPeerConnectionControllerObserver != nullptr) {
		m_pPeerConnectionControllerObserver->OnICECandidatesGatheringDone(pPeerConnection);
	}


Error:
	return r;
}

RESULT PeerConnectionController::OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) {
	RESULT r = R_PASS;

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	if (m_pPeerConnectionControllerObserver != nullptr) {
		CR(m_pPeerConnectionControllerObserver->OnDataChannelStringMessage(pPeerConnection->GetPeerUserID(), strDataChannelMessage));
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	if (m_pPeerConnectionControllerObserver != nullptr) {
		CR(m_pPeerConnectionControllerObserver->OnDataChannelMessage(pPeerConnection->GetPeerUserID(), pDataChannelBuffer, pDataChannelBuffer_n));
	}

Error:
	return r;
}

RESULT PeerConnectionController::OnAudioData(long peerConnectionID, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	RESULT r = R_PASS;

	PeerConnection *pPeerConnection = GetPeerConnectionByID(peerConnectionID);
	CNM(pPeerConnection, "Peer connection %d not found", peerConnectionID);

	if (m_pPeerConnectionControllerObserver != nullptr) {
		CR(m_pPeerConnectionControllerObserver->OnAudioData(pPeerConnection, pAudioData, bitsPerSample, samplingRate, channels, frames));
	}

Error:
	return r;
}

RESULT PeerConnectionController::SendDataChannelStringMessage(int peerID, std::string& strMessage) {
	RESULT r = R_PASS;
	
	PeerConnection *pPeerConnection = GetPeerConnectionByPeerUserID(peerID);
	CNM(pPeerConnection, "Peer connection to user %d not found", peerID);

	CN(m_pWebRTCImp);
	CR(m_pWebRTCImp->SendDataChannelStringMessage(pPeerConnection->GetPeerConnectionID(), strMessage));

Error:
	return r;
}

RESULT PeerConnectionController::SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	PeerConnection *pPeerConnection = GetPeerConnectionByPeerUserID(peerID);
	CNM(pPeerConnection, "Peer connection to user %d not found", peerID);

	CN(m_pWebRTCImp);
	CR(m_pWebRTCImp->SendDataChannelMessage(pPeerConnection->GetPeerConnectionID(), pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT PeerConnectionController::BroadcastDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pWebRTCImp);

	for (auto &pPeerConnection: m_peerConnections) {
		if (pPeerConnection.IsWebRTCConnectionStable()) {
			CR(m_pWebRTCImp->SendDataChannelStringMessage(pPeerConnection.GetPeerConnectionID(), strMessage));
		}
	}

Error:
	return r;
}

RESULT PeerConnectionController::BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pWebRTCImp);

	for (auto &pPeerConnection : m_peerConnections) {
		if (pPeerConnection.IsWebRTCConnectionStable()) {
			CR(m_pWebRTCImp->SendDataChannelMessage(pPeerConnection.GetPeerConnectionID(), pDataChannelBuffer, pDataChannelBuffer_n));
		}
	}

Error:
	return r;
}

long PeerConnectionController::GetUserID() {
	if (m_pPeerConnectionControllerObserver != nullptr) {
		return m_pPeerConnectionControllerObserver->GetUserID();
	}

	return -1;
}

RESULT PeerConnectionController::SetUser(User currentUser) {
	return m_pWebRTCImp->SetUser(currentUser);
}

RESULT PeerConnectionController::SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation) {
	return m_pWebRTCImp->SetTwilioNTSInformation(twilioNTSInformation);
}
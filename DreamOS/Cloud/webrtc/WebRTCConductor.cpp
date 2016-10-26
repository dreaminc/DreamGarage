#include "Logger/Logger.h"

#include "WebRTCConductor.h"

#include "WebRTCImp.h"

#include <memory>
#include <utility>
#include <vector>

#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/examples/peerconnection/client/defaults.h"

#include "webrtc/api/test/fakertccertificategenerator.h"
#include "webrtc/p2p/base/fakeportallocator.h"

#include "WebRTCPeerConnection.h"
#include "WebRTCICECandidate.h"
#include "Cloud/Environment/PeerConnection.h"

#include "Cloud/User/User.h"
#include "Cloud//User/TwilioNTSInformation.h"

WebRTCConductor::WebRTCConductor(WebRTCConductorObserver *pParetObserver) :
	m_pParentObserver(pParetObserver),
	m_pWebRTCPeerConnectionFactory(nullptr)
{
	if (m_pWebRTCPeerConnectionFactory.get() != nullptr) {
		m_pWebRTCPeerConnectionFactory.release();
	}

	ClearPeerConnections();
}

WebRTCConductor::~WebRTCConductor() {
	if (m_pWebRTCPeerConnectionFactory.get() != nullptr) {
		m_pWebRTCPeerConnectionFactory.release();
		m_pWebRTCPeerConnectionFactory = nullptr;
	}
}

RESULT WebRTCConductor::ClearPeerConnections() {
	m_webRTCPeerConnections.clear();
	return R_PASS;
}

rtc::scoped_refptr<WebRTCPeerConnection> WebRTCConductor::AddNewPeerConnection(long peerConnectionID) {
	RESULT r = R_PASS;
	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = nullptr;

	CBM((FindPeerConnectionByID(peerConnectionID) == false), "Peer Conncetion ID %d already present", peerConnectionID);
	
	//pWebRTCPeerConnection = std::make_shared<WebRTCPeerConnection>(this, peerConnectionID, m_pWebRTCPeerConnectionFactory);
	pWebRTCPeerConnection = rtc::scoped_refptr<WebRTCPeerConnection>(new rtc::RefCountedObject<WebRTCPeerConnection>(this, peerConnectionID, m_pWebRTCPeerConnectionFactory));
	
	CNM(pWebRTCPeerConnection, "Failed to allocate new peer connection");

	m_webRTCPeerConnections.push_back(pWebRTCPeerConnection);

// Success:
	return pWebRTCPeerConnection;

Error:
	if (pWebRTCPeerConnection != nullptr) {
		pWebRTCPeerConnection = nullptr;
	}

	return nullptr;
}

rtc::scoped_refptr<WebRTCPeerConnection> WebRTCConductor::GetPeerConnection(long peerConnectionID) {
	for (auto &pWebRTCPeerConnection : m_webRTCPeerConnections)
		if (pWebRTCPeerConnection->GetPeerConnectionID() == peerConnectionID)
			return pWebRTCPeerConnection;

	return nullptr;
}

bool WebRTCConductor::FindPeerConnectionByID(long peerConnectionID) {
	if (GetPeerConnection(peerConnectionID) == nullptr) 
		return false;
	else 
		return true;
}

rtc::scoped_refptr<WebRTCPeerConnection> WebRTCConductor::GetPeerConnectionByPeerUserID(long peerUserID) {
	for (auto &pWebRTCPeerConnection : m_webRTCPeerConnections)
		if (pWebRTCPeerConnection->GetPeerUserID() == peerUserID)
			return pWebRTCPeerConnection;

	return nullptr;
}

bool WebRTCConductor::FindPeerConnectionByPeerUserID(long peerUserID) {
	if (GetPeerConnectionByPeerUserID(peerUserID) == nullptr)
		return false;
	else
		return true;
}

bool WebRTCConductor::IsPeerConnectionInitialized(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->IsPeerConnectionInitialized();
	}

	return false;
}

bool WebRTCConductor::IsConnected(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	
	if (pWebRTCPeerConnection != nullptr) {
		// TODO: Switch to a robust connection check
		return pWebRTCPeerConnection->IsPeerConnectionInitialized();
	}

	return false;
}

bool WebRTCConductor::IsOfferer(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->IsOfferer();
	}

	return false;
}

bool WebRTCConductor::IsAnswerer(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->IsAnswerer();
	}

	return false;
}

std::list<WebRTCICECandidate> WebRTCConductor::GetICECandidates(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetICECandidates();
	}

	return std::list<WebRTCICECandidate>();
}

std::string WebRTCConductor::GetLocalSDPString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetLocalSDPString();
	}

	return std::string("");
}

std::string WebRTCConductor::GetRemoteSDPString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetRemoteSDPString();
	}

	return std::string("");
}

std::string WebRTCConductor::GetLocalSDPJSONString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetLocalSDPJSONString();
	}
	
	return std::string("");
}

std::string WebRTCConductor::GetRemoteSDPJSONString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetRemoteSDPJSONString();
	}

	return std::string("");
}

RESULT WebRTCConductor::CreateSDPOfferAnswer(long peerConnectionID, std::string strSDPOffer) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CN(pWebRTCPeerConnection);

	CR(pWebRTCPeerConnection->CreateSDPOfferAnswer(strSDPOffer));

Error:
	return r;
}

RESULT WebRTCConductor::SetSDPAnswer(long peerConnectionID, std::string strSDPAnswer) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CN(pWebRTCPeerConnection);

	CR(pWebRTCPeerConnection->SetSDPAnswer(strSDPAnswer));

Error:
	return r;
}

// TODO: Make sure PeerConnect is the answerer
RESULT WebRTCConductor::AddOfferCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(pPeerConnection->GetPeerConnectionID());

	if (pWebRTCPeerConnection != nullptr) {
		for (auto &iceCandidate : pPeerConnection->GetOfferCandidates()) {
			CR(pWebRTCPeerConnection->AddIceCandidate(iceCandidate));
		}
	}

Error:
	return r;
}

// TODO: Make sure PeerConnect is the offerer
RESULT WebRTCConductor::AddAnswerCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(pPeerConnection->GetPeerConnectionID());

	if (pWebRTCPeerConnection != nullptr) {
		for (auto &iceCandidate : pPeerConnection->GetAnswerCandidates()) {
			CR(pWebRTCPeerConnection->AddIceCandidate(iceCandidate));
		}
	}

Error:
	return r;
}

RESULT WebRTCConductor::Initialize() {
	RESULT r = R_PASS;

	CBM((m_pWebRTCPeerConnectionFactory == nullptr), "Peer Connection Factory already initialized");
	m_pWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory();
	CNM(m_pWebRTCPeerConnectionFactory.get(), "WebRTC Error Failed to initialize PeerConnectionFactory");

//Success:
	return r;

Error:
	if (m_pWebRTCPeerConnectionFactory.get() != nullptr) {
		m_pWebRTCPeerConnectionFactory.release();
	}

	return r;
}

RESULT WebRTCConductor::InitializeNewPeerConnection(long peerConnectionID, bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = AddNewPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Failed to add new peer connection %d", peerConnectionID);

	CRM(pWebRTCPeerConnection->InitializePeerConnection(fAddDataChannel), "Failed to initialize WebRTC Peer Connection");

	if (fCreateOffer) {
		CRM(pWebRTCPeerConnection->CreateOffer(), "Failed to create WebRTC Offer");
	}

Error:
	return r;
}

// WebRTCPeerConnectionObserver Interface
// TODO: implement these and pass back the right vars
RESULT WebRTCConductor::OnWebRTCConnectionStable(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnWebRTCConnectionStable(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnWebRTCConnectionClosed(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnWebRTCConnectionClosed(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnSDPOfferSuccess(long peerConnectionID) {		// TODO: Consolidate with below
	LOG(INFO) << "OnSDPOfferSuccess";

	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnSDPOfferSuccess(peerConnectionID);
	}

	return R_NOT_HANDLED;
}


RESULT WebRTCConductor::OnSDPAnswerSuccess(long peerConnectionID) {	// TODO: Consolidate with below
	LOG(INFO) << "OnSDPAnswerSuccess"; 
	
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnSDPAnswerSuccess(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnSDPSuccess(long peerConnectionID, bool fOffer) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SDP Success on peer connection ID %d %s", peerConnectionID, fOffer ? "offerer" : "answerer");

	LOG(INFO) << "SDP Success on peer connection ID " << peerConnectionID << " " << (fOffer ? "offerer" : "answerer");

//Error:
	return r;
}

RESULT WebRTCConductor::OnSDPFailure(long peerConnectionID, bool fOffer) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SDP Failure on peer connection ID %d %s", peerConnectionID, fOffer ? "offerer" : "answerer");

	//Error:
	return r;
}

RESULT WebRTCConductor::OnICECandidatesGatheringDone(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnICECandidatesGatheringDone(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnDataChannelStringMessage(peerConnectionID, strDataChannelMessage);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnDataChannelMessage(peerConnectionID, pDataChannelBuffer, pDataChannelBuffer_n);
	}

	return R_NOT_HANDLED;
}

User WebRTCConductor::GetUser() {
	return m_pParentObserver->GetUser();
}

TwilioNTSInformation WebRTCConductor::GetTwilioNTSInformation() {
	return m_pParentObserver->GetTwilioNTSInformation();
}
	
RESULT WebRTCConductor::OnAudioData(long peerConnectionID,
	const void* audio_data,
	int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames) {
		
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnAudioData(peerConnectionID, audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::SendDataChannelStringMessageByPeerUserID(long peerUserID, std::string& strMessage) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnectionByPeerUserID(peerUserID);
	CNM(pWebRTCPeerConnection, "Peer Connection for user ID %d not found", peerUserID);

	CR(pWebRTCPeerConnection->SendDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessageByPeerUserID(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnectionByPeerUserID(peerUserID);
	CNM(pWebRTCPeerConnection, "Peer Connection for user ID %d not found", peerUserID);

	CR(pWebRTCPeerConnection->SendDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

// Success:
	return r;

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->SendDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->SendDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}
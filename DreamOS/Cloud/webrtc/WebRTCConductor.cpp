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

RESULT WebRTCConductor::SendMessageToPeer(std::string* strMessage, int peerID) {
	RESULT r = R_PASS;

	LOG(INFO) << "SendMessageToPeer peerID=" << peerID << " msg=" << strMessage;

	// TODO: Add a queue here for messages, this is not correct

	CN(m_pWebRTCPeerConnection);
	CRM(m_pWebRTCClient->SendMessageToPeer(peerID, *strMessage), "Failed to send message to peer");

	// TODO: Disconnect server if failed to connect?
}


std::list<WebRTCICECandidate> WebRTCConductor::GetICECandidates(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
=======
void WebRTCConductor::SendMessage(const std::string& strJSONObject) {
	LOG(INFO) << "SendMessage " << strJSONObject;

	std::string* msg = new std::string(strJSONObject);
	m_pParentWebRTCImp->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, msg);
}

RESULT WebRTCConductor::PrintSDP() {
	DEBUG_LINEOUT("WebRTCConductor: SDP:");
	DEBUG_LINEOUT("%s", m_strSessionDescriptionProtocol.c_str());
	LOG(INFO) << "PrintSDP " << m_strSessionDescriptionProtocol.c_str();

	return R_PASS;
}
>>>>>>> development

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

<<<<<<< HEAD
	return std::string("");
}

std::string WebRTCConductor::GetLocalSDPJSONString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetLocalSDPJSONString();
	}
=======
	LOG(INFO) << "UIThreadCallback msgID=" << msgID;

//Error:
	return;
}

// OnSuccess called when PeerConnection established 
void WebRTCConductor::OnSuccess(webrtc::SessionDescriptionInterface* sessionDescription) {
	RESULT r = R_PASS;

	m_strSessionDescriptionType = sessionDescription->type();
	sessionDescription->ToString(&m_strSessionDescriptionProtocol);

	if (m_fOffer) {
		CR(m_pParentWebRTCImp->OnSDPOfferSuccess());
	}
	else {
		CR(m_pParentWebRTCImp->OnSDPAnswerSuccess());
	}

	m_pWebRTCPeerConnection->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), sessionDescription);
	m_fSDPSet = true;

	LOG(INFO) << "OnSuccess " << m_strSessionDescriptionProtocol;
	CR(PrintSDP());

	CR(ClearSessionDescriptionProtocol());
	/*
	// For loopback test. To save some connecting delay.
	if (m_fLoopback) {
		// Replace message type from "offer" to "answer"
		webrtc::SessionDescriptionInterface* pWebRTCSessionDescription(webrtc::CreateSessionDescription("answer", m_strSessionDescriptionProtocol, nullptr));
		m_pWebRTCPeerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), pWebRTCSessionDescription);
		return;
	}

	// TODO: peer ID stuff
	*/
	

Error:
	return;
}
>>>>>>> development

	return std::string("");
}

<<<<<<< HEAD
std::string WebRTCConductor::GetRemoteSDPJSONString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
=======
void WebRTCConductor::OnFailure(const std::string& error) {
	DEBUG_LINEOUT("WebRTC Error: %s", error.c_str());
	LOG(ERROR) << "OnFailure " << error.c_str();
}
>>>>>>> development

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

<<<<<<< HEAD
	if (pWebRTCPeerConnection != nullptr) {
		for (auto &iceCandidate : pPeerConnection->GetOfferCandidates()) {
			CR(pWebRTCPeerConnection->AddIceCandidate(iceCandidate));
		}
=======
	/*
	pAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateAudioTrack(kAudioLabel, m_pWebRTCPeerConnectionFactory->CreateAudioSource(nullptr)));
		*/
	
	pMediaStreamInterface = m_pWebRTCPeerConnectionFactory->CreateLocalMediaStream(kStreamLabel);

	CR(AddAudioStream(pMediaStreamInterface));
	//CR(AddVideoStream(pMediaStreamInterface));

	// TODO: STREAMMMM
	//main_wnd_->StartLocalRenderer(video_track);


	// Add streams
	if (!m_pWebRTCPeerConnection->AddStream(pMediaStreamInterface)) {
		LOG(INFO) << "Adding stream to PeerConnection failed";
>>>>>>> development
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

<<<<<<< HEAD
	CBM((m_pWebRTCPeerConnectionFactory == nullptr), "Peer Connection Factory already initialized");
	m_pWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory();
	CNM(m_pWebRTCPeerConnectionFactory.get(), "WebRTC Error Failed to initialize PeerConnectionFactory");
=======
	//m_SignalOnDataChannel

	auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);
	CN(m_pDataChannelInterface);

	//CB(pWebRTCDataChannel->Send(webrtc::DataBuffer(strMessage)));
	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(rtc::CopyOnWriteBuffer(strMessage.c_str(), strMessage.length()), true)));//webrtc::DataBuffer(strMessage)));
>>>>>>> development

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

<<<<<<< HEAD
=======
	//auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);

Error:
	return;
}

RESULT WebRTCConductor::CreateOffer(){
	RESULT r = R_PASS;
	CN(m_pWebRTCPeerConnection);
	
	m_fOffer = true;
	m_pWebRTCPeerConnection->CreateOffer(this, NULL);
>>>>>>> development
Error:
	return r;
}

// WebRTCPeerConnectionObserver Interface
// TODO: implement these and pass back the right vars
RESULT WebRTCConductor::OnWebRTCConnectionStable(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnWebRTCConnectionStable(peerConnectionID);
	}
<<<<<<< HEAD

	return R_NOT_HANDLED;
}
=======
	*/
	//if (!CreatePeerConnection(DTLS_OFF)) {
	if (!CreatePeerConnection(DTLS_ON)) {
		DEBUG_LINEOUT("Error CreatePeerConnection failed");
		LOG(ERROR) << "Error CreatePeerConnection failed";
		DeletePeerConnection();
	}

	CN(m_pWebRTCPeerConnection.get());

#ifndef WEBRTC_NO_CANDIDATES
	CR(AddStreams());
>>>>>>> development

RESULT WebRTCConductor::OnWebRTCConnectionClosed(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnWebRTCConnectionClosed(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnSDPOfferSuccess(long peerConnectionID) {		// TODO: Consolidate with below
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnSDPOfferSuccess(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

<<<<<<< HEAD
RESULT WebRTCConductor::OnSDPAnswerSuccess(long peerConnectionID) {	// TODO: Consolidate with below
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnSDPAnswerSuccess(peerConnectionID);
	}

	return R_NOT_HANDLED;
=======
RESULT WebRTCConductor::CreatePeerConnection(bool dtls) {
	RESULT r = R_PASS;

	webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;

	webrtc::PeerConnectionInterface::IceServer iceServer;
	webrtc::FakeConstraints webrtcConstraints;
	std::unique_ptr<rtc::RTCCertificateGeneratorInterface> pCertificateGenerator = nullptr;

	CN(m_pWebRTCPeerConnectionFactory.get());		// ensure factory is valid
	CB((m_pWebRTCPeerConnection.get() == nullptr));	// ensure peer connection is nullptr

	iceServer.uri = GetPeerConnectionString();
	rtcConfiguration.servers.push_back(iceServer);

	if (dtls) {

		if (rtc::SSLStreamAdapter::HaveDtlsSrtp()) {
			pCertificateGenerator = std::unique_ptr<rtc::RTCCertificateGeneratorInterface>(new FakeRTCCertificateGenerator());
		}

		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
		m_pWebRTCPeerConnection = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, &webrtcConstraints, NULL, std::move(pCertificateGenerator), this);
	}
	else {
		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "false");
		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, "true");
		
		m_pWebRTCPeerConnection = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, &webrtcConstraints, NULL, NULL, this);
	}

	CNM(m_pWebRTCPeerConnection.get(), "WebRTC Peer Connection failed to initialize");
Error:
	return r;
>>>>>>> development
}

RESULT WebRTCConductor::OnSDPSuccess(long peerConnectionID, bool fOffer) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SDP Success on peer connection ID %d %s", peerConnectionID, fOffer ? "offerer" : "answerer");

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

RESULT WebRTCConductor::SendDataChannelStringMessageByPeerUserID(long peerUserID, std::string& strMessage) {
	RESULT r = R_PASS;

<<<<<<< HEAD
	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnectionByPeerUserID(peerUserID);
	CNM(pWebRTCPeerConnection, "Peer Connection for user ID %d not found", peerUserID);
=======
	DEBUG_LINEOUT("WebRTCConductor:OnMessageFromPeer: %d: %s", peerID, strMessage.c_str());
	LOG(INFO) << "WebRTCConductor:OnMessageFromPeer: " << peerID << "," << strMessage.c_str();

	CBM(((m_WebRTCPeerID == peerID) || (m_WebRTCPeerID == -1)), "PeerID mismatch %d not %d", peerID, m_WebRTCPeerID);
	CBM((!strMessage.empty()), "Message cannot be empty for OnMessageOnPeer");
	
	{
		if (!m_pWebRTCPeerConnection.get()) {
			CB((m_WebRTCPeerID == -1));
			m_WebRTCPeerID = peerID;

			if (!InitializePeerConnection()) {

				LOG(INFO) << "Failed to initialize our PeerConnection instance";
				m_pWebRTCClient->SignOut();

				return;
			}
		}
		else if (peerID != m_WebRTCPeerID) {
			CB((m_WebRTCPeerID != -1));
			LOG(WARNING) << "Received a message from unknown peer while already in a conversation with a different peer.";
			return;
		}

		Json::Reader reader;
		Json::Value jmessage;

		if (!reader.parse(strMessage, jmessage)) {
			LOG(WARNING) << "Received unknown message. " << strMessage;
			return;
		}

		std::string type;
		std::string json_object;

		rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName, &type);

		if (!type.empty()) {
			if (type == "offer-loopback") {
				// This is a loopback call.
				// Recreate the peerconnection with DTLS disabled.
				if (!ReinitializePeerConnectionForLoopback()) {
					LOG(INFO) << "Failed to initialize our PeerConnection instance";
					DeletePeerConnection();
					m_pWebRTCClient->SignOut();
				}
				return;
			}

			std::string strSDP;

			if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName, &strSDP)) {
				LOG(WARNING) << "Can't parse received session description message.";
				return;
			}
>>>>>>> development

	CR(pWebRTCPeerConnection->SendDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessageByPeerUserID(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnectionByPeerUserID(peerUserID);
	CNM(pWebRTCPeerConnection, "Peer Connection for user ID %d not found", peerUserID);

<<<<<<< HEAD
	CR(pWebRTCPeerConnection->SendDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));
=======
		std::unique_ptr<webrtc::IceCandidateInterface> candidate(
			webrtc::CreateIceCandidate(iceCandidate.m_strSDPMediaID, iceCandidate.m_SDPMediateLineIndex,
									   iceCandidate.m_strSDPCandidate, &sdpError));

		CBM((candidate.get()), "Can't parse received candidate message. SdpParseError was: %s", sdpError.description.c_str());
		CBM((m_pWebRTCPeerConnection->AddIceCandidate(candidate.get())), "Failed to apply the received candidate");

		DEBUG_LINEOUT("Received candidate : %s", iceCandidate.m_strSDPCandidate.c_str());
		LOG(INFO) << "Received candidate : " << iceCandidate.m_strSDPCandidate.c_str();
	//}
>>>>>>> development

// Success:
	return r;

Error:
	LOG(INFO) << "Candidate " << iceCandidate.m_strSDPCandidate.c_str() << " failed with error: " << sdpError.description.c_str();
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
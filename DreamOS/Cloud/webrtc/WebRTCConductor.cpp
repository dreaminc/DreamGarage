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

WebRTCConductor::WebRTCConductor(WebRTCImp *pParentWebRTCImp) :
	m_pParentWebRTCImp(pParentWebRTCImp),
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

RESULT WebRTCConductor::AddNewPeerConnection(long peerConnectionID) {
	RESULT r = R_PASS;

	CBM((FindPeerConnectionByID(peerConnectionID) == false), "Peer Conncetion ID %d already present");
	std::shared_ptr<WebRTCPeerConnection> pWebRTCPeerConnection = std::make_shared<WebRTCPeerConnection>(peerConnectionID, m_pWebRTCPeerConnectionFactory);
	CNM(pWebRTCPeerConnection, "Failed to allocate new peer connection");

	m_webRTCPeerConnections.push_back(pWebRTCPeerConnection);

Error:
	return r;
}

std::shared_ptr<WebRTCPeerConnection> WebRTCConductor::GetPeerConnection(long peerConnectionID) {
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

RESULT WebRTCConductor::SendDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	//m_SignalOnDataChannel

	auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);
	CN(m_pDataChannelInterface);

	//CB(pWebRTCDataChannel->Send(webrtc::DataBuffer(strMessage)));
	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(strMessage)));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	CN(m_pDataChannelInterface);

	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(rtc::CopyOnWriteBuffer(pDataChannelBuffer, pDataChannelBuffer_n), true)));

Error:
	return r;
}

std::string GetDataStateString(webrtc::DataChannelInterface::DataState state) {
	switch (state) {
		case webrtc::DataChannelInterface::DataState::kConnecting: return std::string("connecting"); break;
		case webrtc::DataChannelInterface::DataState::kOpen: return std::string("open"); break;
		case webrtc::DataChannelInterface::DataState::kClosing: return std::string("closing"); break;
		case webrtc::DataChannelInterface::DataState::kClosed: return std::string("closed"); break;
		default:  return std::string("invalid state"); break;
	}
}

//std::list<ICECandidate> g_peerICECandidates;

RESULT WebRTCConductor::AddIceCandidate(WebRTCICECandidate iceCandidate) {
	RESULT r = R_PASS;

	//for (auto &peerICECandidate : g_peerICECandidates) {
		webrtc::SdpParseError sdpError;
		//std::unique_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(strSDPMID, sdpMLineIndex, strSDP, &sdpError));

		std::unique_ptr<webrtc::IceCandidateInterface> candidate(
			webrtc::CreateIceCandidate(iceCandidate.m_strSDPMediaID, iceCandidate.m_SDPMediateLineIndex,
									   iceCandidate.m_strSDPCandidate, &sdpError));

		CBM((candidate.get()), "Can't parse received candidate message. SdpParseError was: %s", sdpError.description.c_str());
		CBM((m_pWebRTCPeerConnection->AddIceCandidate(candidate.get())), "Failed to apply the received candidate");

		DEBUG_LINEOUT("Received candidate : %s", iceCandidate.m_strSDPCandidate.c_str());
	//}

Error:
	return r;
}
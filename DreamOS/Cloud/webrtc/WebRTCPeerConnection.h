#ifndef WEBRTC_PEER_CONNECTION_H_
#define WEBRTC_PEER_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCPeerConnection.h
// The container for the WebRTCPeerConnection per our implementation

#include "webrtc/api/peerconnectioninterface.h"

class WebRTCPeerConnection {
public:
	WebRTCPeerConnection(long peerConnectionID, rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory) :
		m_peerConnectionID(peerConnectionID),
		m_pWebRTCPeerConnectionFactory(pWebRTCPeerConnectionFactory),
		m_pWebRTCPeerConnectionInterface(nullptr)
	{
		// empty for now
	}

	RESULT SetPeerConnectionFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory) {
		m_pWebRTCPeerConnectionFactory = pWebRTCPeerConnectionFactory;
		return R_PASS;
	}

public:
	long GetPeerConnectionID() { return m_peerConnectionID; }
	
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> GetWebRTCPeerConnectionInterface() { return m_pWebRTCPeerConnectionInterface; }

private:
	long m_peerConnectionID;

	std::string m_strLocalSessionDescriptionProtocol;
	std::string m_strLocalSessionDescriptionType;

	std::string m_strRemoteSessionDescriptionProtocol;
	std::string m_strRemoteSessionDescriptionType;

	rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_pWebRTCPeerConnectionInterface;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;
};


#endif // ! WEBRTC_PEER_CONNECTION_H_

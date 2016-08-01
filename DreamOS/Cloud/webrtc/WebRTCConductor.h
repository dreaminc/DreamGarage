#ifndef WEBRTC_CONDUCTOR_H_
#define WEBRTC_CONDUCTOR_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCConductor.h
// The WebRTC Conductor which conducts the peer connection and generally handles the WebRTC 
// connections

#include <deque>
#include <map>
#include <set>
#include <string>

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"

#define DTLS_ON  true
#define DTLS_OFF false

class WebRTCImp;
class WebRTCClient;

class WebRTCConductor : public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver /*, public PeerConnectionClientObserver, public MainWndCallback */
{

public:
	enum CallbackID {
		MEDIA_CHANNELS_INITIALIZED = 1,
		PEER_CONNECTION_CLOSED,
		SEND_MESSAGE_TO_PEER,
		NEW_STREAM_ADDED,
		STREAM_REMOVED,
	};

public:
	WebRTCConductor(WebRTCClient *pWebRTCClient, WebRTCImp *pParentWebRTCImp);

protected:

	// PeerConnectionObserver implementation.
	void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {};

	void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

	void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
	void OnRenegotiationNeeded() override {}
	void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {};
	void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {};

	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
	
	void OnIceConnectionReceivingChange(bool receiving) override {}

	// PeerConnectionClientObserver implementation.
	virtual void OnSignedIn();
	virtual void OnDisconnected();
	virtual void OnPeerConnected(int id, const std::string& name);
	virtual void OnPeerDisconnected(int id);
	virtual void OnMessageFromPeer(int peerID, const std::string& message);
	virtual void OnMessageSent(int err);
	virtual void OnServerConnectionFailure();

	// CreateSessionDescriptionObserver implementation.
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc);
	virtual void OnFailure(const std::string& error);

protected:
	// Send a message to the remote peer.
	void SendMessage(const std::string& json_object);

	RESULT CreatePeerConnection(bool dtls);
	RESULT DeletePeerConnection();
	RESULT InitializePeerConnection();
	RESULT ReinitializePeerConnectionForLoopback();
	RESULT AddStreams();
	cricket::VideoCapturer* OpenVideoCaptureDevice();

private:
	// Utility (TODO: Move this elsewhere?)
	std::string GetPeerConnectionString();

private:
	WebRTCImp *m_pParentWebRTCImp;
	WebRTCClient *m_pWebRTCClient;

	int m_WebRTCPeerID;
	bool m_fLoopback;

	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_pWebRTCPeerConnection;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> > m_WebRTCActiveStreams;

	std::string m_strWebRTCServer;
};

#endif	// ! WEBRTC_CONDUCTOR_H_
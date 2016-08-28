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

class WebRTCConductor : 
	public webrtc::PeerConnectionObserver, 
	public webrtc::CreateSessionDescriptionObserver,
	public webrtc::DataChannelObserver /*, public PeerConnectionClientObserver, public MainWndCallback */
{

public:
	enum CallbackID {
		MEDIA_CHANNELS_INITIALIZED = 1,
		PEER_CONNECTION_CLOSED,
		SEND_MESSAGE_TO_PEER,
		NEW_STREAM_ADDED,
		STREAM_REMOVED,
	};

	friend class WebRTCImp;
	friend class WebRTCClient;

public:
	WebRTCConductor(WebRTCClient *pWebRTCClient, WebRTCImp *pParentWebRTCImp);
	~WebRTCConductor();
	
	RESULT Initialize();

	RESULT SendDataChannelStringMessage(std::string& strMessage);
	RESULT SendDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT CreateSDPOfferAnswer(std::string strSDPOffer);
	RESULT AddIceCandidates();

protected:
	// PeerConnectionObserver implementation.
	void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {};

	void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;

	void OnRenegotiationNeeded() override {}

	void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);
	void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);
	void OnIceConnectionReceivingChange(bool receiving);
	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;

	// PeerConnectionClientObserver implementation.
	virtual void OnSignedIn();
	virtual void OnDisconnected();
	virtual void OnPeerConnected(int id, const std::string& name);
	virtual void OnPeerDisconnected(int id);
	virtual void OnMessageFromPeer(int peerID, const std::string& strMessage);
	virtual void OnMessageSent(int err);
	virtual void OnServerConnectionFailure();

	// DataChannelObserver Implementation
	virtual void OnStateChange() override;
	virtual void OnMessage(const webrtc::DataBuffer& buffer) override;
	virtual void OnBufferedAmountChange(uint64_t previous_amount) override  {/* empty */};

	// CreateSessionDescriptionObserver implementation.
	void OnSuccess(webrtc::SessionDescriptionInterface* sessionDescription);
	virtual void OnFailure(const std::string& error);

protected:
	// Send a message to the remote peer.
	void SendMessage(const std::string& json_object);

	RESULT CreatePeerConnection(bool dtls);
	RESULT DeletePeerConnection();
	RESULT InitializePeerConnection(bool fAddDataChannel = false);
	RESULT ReinitializePeerConnectionForLoopback();
	cricket::VideoCapturer* OpenVideoCaptureDevice();

	bool IsPeerConnectionInitialized();
	
	RESULT AddStreams();
	RESULT AddVideoStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface);
	RESULT AddAudioStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface);
	//RESULT AddDataStream();
	RESULT AddDataChannel();

	RESULT CreateOffer();
	RESULT SendMessageToPeer(std::string* strMessage, int peerID);

private:
	// Utility (TODO: Move this elsewhere?)
	std::string GetPeerConnectionString();
	std::string GetSessionDescriptionString();
	RESULT ClearSessionDescriptionProtocol();
	RESULT PrintSDP();
	std::string GetSDPJSONString();
	std::string GetSDPString();
	std::string GetSDPTypeString();

public:
	RESULT SetPeerConnectionID(int peerID) {
		m_WebRTCPeerID = peerID;
		return R_PASS;
	}

	int GetPeerConnectionID() {
		return m_WebRTCPeerID;
	}

	void UIThreadCallback(int msgID, void* data);

private:
	WebRTCImp *m_pParentWebRTCImp;
	WebRTCClient *m_pWebRTCClient;

	int m_WebRTCPeerID;
	bool m_fLoopback;

	bool m_fOffer;	// TODO: this needs to be generalized

	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_pWebRTCPeerConnection;

	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> > m_WebRTCActiveStreams;
	std::map<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface> > m_WebRTCActiveDataChannels;

	rtc::scoped_refptr<webrtc::DataChannelInterface> m_pDataChannelInterface;

	sigslot::signal1<webrtc::DataChannelInterface*> m_SignalOnDataChannel;

	std::string m_strWebRTCServer;

	std::string m_strSessionDescriptionProtocol;
	std::string m_strSessionDescriptionType;
};

#endif	// ! WEBRTC_CONDUCTOR_H_
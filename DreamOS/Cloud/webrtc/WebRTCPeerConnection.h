#ifndef WEBRTC_PEER_CONNECTION_H_
#define WEBRTC_PEER_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCPeerConnection.h
// The container for the WebRTCPeerConnection per our implementation

#include <memory>

#include "webrtc/base/common.h"
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"

#include "WebRTCICECandidate.h"

class WebRTConductor;
class User;
class TwilioNTSInformation;

class WebRTCPeerConnection : 
	public webrtc::PeerConnectionObserver, 
	public webrtc::DataChannelObserver,
	public webrtc::CreateSessionDescriptionObserver,
	public webrtc::AudioTrackSinkInterface
{
public:
	class WebRTCPeerConnectionObserver {
	public:
		virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) = 0;
		virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) = 0;
		virtual RESULT OnSDPOfferSuccess(long peerConnectionID) = 0;		// TODO: Consolidate with below
		virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) = 0;	// TODO: Consolidate with below
		virtual RESULT OnSDPSuccess(long peerConnectionID, bool fOffer) = 0;
		virtual RESULT OnSDPFailure(long peerConnectionID, bool fOffer) = 0;
		virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) = 0;
		virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;

		virtual User GetUser() = 0;
		virtual TwilioNTSInformation GetTwilioNTSInformation() = 0;

		virtual RESULT OnAudioData(long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	};

	friend class WebRTCPeerConnectionObserver;
	friend class WebRTCConductor;

public:
	WebRTCPeerConnection(WebRTCPeerConnectionObserver *pParentObserver, long peerConnectionID, rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory);
	~WebRTCPeerConnection();

	RESULT AddStreams();
	RESULT AddVideoStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface);
	RESULT AddAudioStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface);
	RESULT AddDataChannel();

	RESULT SetPeerConnectionFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory);
	RESULT ClearSessionDescriptionProtocols();
	RESULT ClearLocalSessionDescriptionProtocol();
	RESULT ClearRemoteSessionDescriptionProtocol();
	std::list<WebRTCICECandidate> GetICECandidates();

protected:

	// PeerConnectionObserver implementation.
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
	virtual void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	virtual void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
	virtual void OnRenegotiationNeeded() override;
	virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);
	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);
	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
	virtual void OnIceConnectionReceivingChange(bool receiving) override;

	// DataChannelObserver Implementation
	virtual void OnStateChange() override;
	virtual void OnMessage(const webrtc::DataBuffer& buffer) override;
	virtual void OnBufferedAmountChange(uint64_t previous_amount) override  {};

	// CreateSessionDescriptionObserver implementation.
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* sessionDescription) override;
	virtual void OnFailure(const std::string& error) override;

	// webrtc::AudioTrackSinkInterface
	virtual void OnData(const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;

public:
	RESULT InitializePeerConnection(bool fAddDataChannel = false);
	RESULT CreatePeerConnection(bool dtls);
	RESULT CreateOffer();
	RESULT CreateSDPOfferAnswer(std::string strSDPOffer);
	RESULT SetSDPAnswer(std::string strSDPAnswer);
	RESULT AddIceCandidate(WebRTCICECandidate iceCandidate);

	RESULT SendDataChannelStringMessage(std::string& strMessage);
	RESULT SendDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

protected:
	// TODO: Move to peer Connection
	bool IsPeerConnectionInitialized();


public:
	// Video (TODO eventually)
	cricket::VideoCapturer* OpenVideoCaptureDevice();

public:
	long GetPeerConnectionID() { return m_peerConnectionID; }
	int GetPeerUserID() { return m_WebRTCPeerID; }
	RESULT SetPeerUserID(int peerID) { m_WebRTCPeerID = peerID; return R_PASS; }
	bool IsOfferer() { return (m_fOffer == true); }
	bool IsAnswerer() { return (m_fOffer == false); }

	rtc::scoped_refptr<webrtc::PeerConnectionInterface> GetWebRTCPeerConnectionInterface() { return m_pWebRTCPeerConnectionInterface; }

public:
	std::string GetPeerConnectionString();

	RESULT PrintSDP();
	std::string GetSDPJSONString(std::string strSessionDescriptionType, std::string strSessionDescriptionProtocol);

	RESULT PrintLocalSDP();
	std::string GetLocalSDPTypeString();
	std::string GetLocalSDPString();
	std::string GetLocalSDPJSONString();

	RESULT PrintRemoteSDP();
	std::string GetRemoteSDPTypeString();
	std::string GetRemoteSDPString();
	std::string GetRemoteSDPJSONString();

public:
	// TODO: Replace this with PeerConnection ID

private:
	//std::shared_ptr<WebRTConductor> m_pParentWebRTCConductor;
	WebRTCPeerConnectionObserver *m_pParentObserver;

	long m_peerConnectionID;
	int m_WebRTCPeerID;

	bool m_fOffer;	// TODO: this needs to be generalized
	bool m_fSDPSet;	// TODO: temp

	std::string m_strLocalSessionDescriptionProtocol;
	std::string m_strLocalSessionDescriptionType;

	std::string m_strRemoteSessionDescriptionProtocol;
	std::string m_strRemoteSessionDescriptionType;

	std::list<WebRTCICECandidate> m_webRTCICECandidates;

	rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_pWebRTCPeerConnectionInterface;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;

	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> > m_WebRTCActiveStreams;
	std::map<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface> > m_WebRTCActiveDataChannels;

	rtc::scoped_refptr<webrtc::DataChannelInterface> m_pDataChannelInterface;
	sigslot::signal1<webrtc::DataChannelInterface*> m_SignalOnDataChannel;
};


#endif // ! WEBRTC_PEER_CONNECTION_H_


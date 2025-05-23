#ifndef WEBRTC_PEER_CONNECTION_H_
#define WEBRTC_PEER_CONNECTION_H_

#include "core/ehm/EHM.h"

// Dream Cloud Webrtc
// dos/src/cloud/webrtc/WebRTCPeerConnection.h

// The container for the WebRTCPeerConnection per our implementation

#include <memory>

//#include "rtc_base/common.h"
#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"
#include "media/base/videocommon.h"
#include "api/video/video_frame.h"
#include "pc/localaudiosource.h"

#include "WebRTCICECandidate.h"
#include "WebRTCIceConnection.h"

#include "core/types/Proxy.h"
#include "core/primitives/color.h"

#include "WebRTCLocalAudioSource.h"
#include "WebRTCAudioTrackSink.h"
#include "WebRTCVideoSink.h"

class WebRTConductor;
//class WebRTCLocalAudioSource;
//class WebRTCAudioTrackSink;
class User;
class TwilioNTSInformation;
class AudioPacket;

class WebRTCPeerConnectionProxy : public Proxy<WebRTCPeerConnectionProxy> {
public:
	virtual RESULT SetAudioVolume(double val) = 0;
};

class WebRTCPeerConnection : 
	public webrtc::PeerConnectionObserver, 
	public webrtc::DataChannelObserver,
	public webrtc::CreateSessionDescriptionObserver,
	//public rtc::VideoSinkInterface<webrtc::VideoFrame>,
	public WebRTCPeerConnectionProxy,
	public WebRTCAudioTrackSink::observer,
	public WebRTCVideoSink::observer
{
public:
	

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
		virtual RESULT OnICECandidateGathered(WebRTCICECandidate *pICECandidate, long peerConnectionID) = 0;
		virtual RESULT OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) = 0;
		virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;

		virtual RESULT OnRenegotiationNeeded(long peerConnectionID) = 0;
		virtual RESULT OnAddStream(long peerConnectionID, rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStream) = 0;
		virtual RESULT OnRemoveStream(long peerConnectionID, rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStream) = 0;
		virtual RESULT OnDataChannel(long peerConnectionID, rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannel) = 0;
		virtual RESULT OnDataChannelStateChange(long peerConnectionID, rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannel) = 0;

		virtual User GetUser() = 0;
		virtual TwilioNTSInformation GetTwilioNTSInformation() = 0;

		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
	};

	friend class WebRTCPeerConnectionObserver;
	friend class WebRTCConductor;

public:
	WebRTCPeerConnection(WebRTCPeerConnectionObserver *pParentObserver, 
						 long peerConnectionID, 
						 rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory,
						 WebRTCConductor *pParentWebRTCConductor
						 );

	~WebRTCPeerConnection();

	// TODO: Generalize this when we add renegotiation 
	// so that they're not hard coded per WebRTCCommon
	RESULT AddStreams(bool fAddDataChannel = true);
	RESULT AddVideoStream(const std::string &strVideoCaptureDevice, const std::string &strVideoTrackLabel, const std::string &strMediaStreamLabel);
	RESULT AddAudioStream(const std::string &strAudioTrackLabel);
	RESULT AddLocalAudioSource(const std::string &strAudioTrackLabel, const std::string &strMediaStreamLabel);
	RESULT AddDataChannel();

	RESULT SetUserPeerConnectionFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory);
	
	RESULT ClearSessionDescriptionProtocols();
	RESULT ClearLocalSessionDescriptionProtocol();
	RESULT ClearRemoteSessionDescriptionProtocol();
	std::list<WebRTCICECandidate> GetICECandidates();

	RESULT CloseWebRTCPeerConnection();

public:
	// WebRTCPeerConnectionProxy
	virtual RESULT SetAudioVolume(double val) override;
	virtual WebRTCPeerConnectionProxy* GetProxy() override;

protected:

	// PeerConnectionObserver implementation.
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
	virtual void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	virtual void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

	// TODO: Migrate to unified API
	virtual void OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
		const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&streams) override;

	virtual void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;

	virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
	virtual void OnRenegotiationNeeded() override;
	virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);
	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);
	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* pICECandidate) override;
	virtual void OnIceConnectionReceivingChange(bool fReceiving) override;

	// DataChannelObserver Implementation
	// Need to create a separate WebRTCDataChannel object
	virtual void OnStateChange() override;
	virtual void OnMessage(const webrtc::DataBuffer& buffer) override;
	virtual void OnBufferedAmountChange(uint64_t previous_amount) override  {};

	// CreateSessionDescriptionObserver implementation.
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* sessionDescription) override;
	virtual void OnFailure(const std::string& error) override;

	// rtc::VideoSinkInterface<cricket::VideoFrame>
	//virtual void OnFrame(const webrtc::VideoFrame& cricketVideoFrame) override;

	// WebRTCVideoSink::observer
	virtual RESULT OnVideoFrame(std::string strVideoTrackName, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;

	// WebRTCAudioTrackSink::observer
	virtual void OnAudioTrackSinkData(std::string strAudioTrackLabel, const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;

public:
	RESULT InitializePeerConnection(bool fAddDataChannel = false);
	RESULT CreatePeerConnection(bool dtls);
	RESULT CreateOffer();
	RESULT CreateSDPOfferAnswer(std::string strSDPOffer);
	RESULT SetSDPAnswer(std::string strSDPAnswer);
	RESULT AddIceCandidate(WebRTCICECandidate iceCandidate);

	RESULT SendDataChannelStringMessage(std::string& strMessage);
	RESULT SendDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	// Video
	RESULT SendVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT StartVideoStreaming(const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat);
	RESULT StopVideoStreaming(const std::string &strVideoTrackLabel);
	bool IsVideoStreamingRunning(const std::string &strVideoTrackLabel);

	// Audio
	RESULT SendAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket);

protected:
	// TODO: Move to peer Connection
	bool IsPeerConnectionInitialized();


public:
	// Video
	std::unique_ptr<cricket::VideoCapturer> OpenVideoCaptureDevice();

	RESULT InitializeVideoCaptureDevice(std::string strDeviceName, std::string strVideoTrackLabel);
	cricket::VideoCapturer* GetVideoCaptureDeviceByTrackName(std::string strTrackName);

	std::map<std::string, std::unique_ptr<cricket::VideoCapturer>> m_videoCaptureDevices;
	
	RESULT InitializeVideoSink(std::string strTrackName, webrtc::VideoTrackSourceInterface* pVideoTrackSource);
	WebRTCVideoSink *GetVideoSink(std::string strTrackName);
	
	std::map<std::string, std::unique_ptr<WebRTCVideoSink>> m_videoSinks;

	//std::unique_ptr<cricket::VideoCapturer> m_pCricketVideoCapturer = nullptr;

public:
	long GetPeerConnectionID() { return m_peerConnectionID; }
	int GetPeerUserID() { return m_WebRTCPeerID; }
	RESULT SetPeerUserID(int peerID) { m_WebRTCPeerID = peerID; return R_PASS; }
	bool IsOfferer() { return (m_fOffer == true); }
	bool IsAnswerer() { return (m_fOffer == false); }

	long GetUserID() { return m_userID; }
	RESULT SetUserID(long userID) { m_userID = userID; return R_PASS; }

	rtc::scoped_refptr<webrtc::PeerConnectionInterface> GetWebRTCPeerConnectionInterface() { return m_pWebRTCPeerConnectionInterface; }

	// Convenience Function
	std::string GetLogSignature() {
		return (std::string("[WebRTCPeerConnection] PID:") + std::to_string(GetPeerConnectionID()) + 
			" UserID:" + std::to_string(GetUserID()) +
			" PeerID:" + std::to_string(GetPeerUserID() )
		);
	}

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
	long m_userID;

	bool m_fOffer;	// TODO: this needs to be generalized
	bool m_fSDPSet;	// TODO: temp

	WebRTCConductor *m_pParentWebRTCConductor = nullptr;

	std::string m_strLocalSessionDescriptionProtocol;
	std::string m_strLocalSessionDescriptionType;

	std::string m_strRemoteSessionDescriptionProtocol;
	std::string m_strRemoteSessionDescriptionType;

	std::list<WebRTCICECandidate> m_webRTCICECandidates;

	rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_pWebRTCPeerConnectionInterface;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;

	//std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> > m_WebRTCLocalActiveStreams;
	std::map<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface> > m_WebRTCLocalActiveDataChannels;

	//std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> > m_WebRTCRemoteActiveStreams;
	std::map<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface> > m_WebRTCRemoteActiveDataChannels;

	// Sinks
	std::map<std::string, std::shared_ptr<WebRTCAudioTrackSink>> m_webRTCAudioTrackSinks;

	rtc::scoped_refptr<webrtc::DataChannelInterface> m_pDataChannelInterface;
	sigslot::signal1<webrtc::DataChannelInterface*> m_SignalOnDataChannel;

	// local audio sources
	std::map<std::string, rtc::scoped_refptr<WebRTCLocalAudioSource>> m_pWebRTCLocalAudioSources;
};


#endif // ! WEBRTC_PEER_CONNECTION_H_


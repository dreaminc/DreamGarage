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

#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"

#define DTLS_ON  true
#define DTLS_OFF false

class WebRTCImp;
class WebRTCICECandidate;
class PeerConnection;
class User;
class TwilioNTSInformation;
class AudioPacket;

#include "WebRTCPeerConnection.h"

#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_device/dummy/audio_device_dummy.h"

#include "WebRTCAudioDeviceModule.h"

class WebRTCConductor : 
	public WebRTCPeerConnection::WebRTCPeerConnectionObserver//,
	//public AudioDeviceDataCapturer
{
public:
	class WebRTCConductorObserver {
	public:
		virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) = 0;
		virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) = 0;
		virtual RESULT OnSDPOfferSuccess(long peerConnectionID) = 0;		// TODO: Consolidate with below
		virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) = 0;	// TODO: Consolidate with below
		virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) = 0;
		virtual RESULT OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) = 0;
		virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;

		virtual RESULT OnRenegotiationNeeded(long peerConnectionID) = 0;
		virtual RESULT OnDataChannel(long peerConnectionID) = 0;
		virtual RESULT OnAudioChannel(long peerConnectionID) = 0;

		virtual User GetUser() = 0;
		virtual TwilioNTSInformation GetTwilioNTSInformation() = 0;
		
		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
		virtual RESULT OnVideoFrame(long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
	};

	friend class WebRTCImp;

public:
	WebRTCConductor(WebRTCConductorObserver *pParentObserver);
	~WebRTCConductor();
	
	RESULT Initialize();
	RESULT InitializeNewPeerConnection(long peerConnectionID, long userID, long peerUserID, bool fCreateOffer, bool fAddDataChannel);
	RESULT CloseAllPeerConnections();
	RESULT CloseWebRTCPeerConnection(PeerConnection *pPeerConnection);

	RESULT Shutdown();

	friend class WebRTCImp;
	friend class WebRTCPeerConnection;

public:
	// WebRTCPeerConnectionObserver Interface
	virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) override;
	virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) override;
	virtual RESULT OnSDPOfferSuccess(long peerConnectionID) override;		// TODO: Consolidate with below
	virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) override;	// TODO: Consolidate with below
	virtual RESULT OnSDPSuccess(long peerConnectionID, bool fOffer) override;
	virtual RESULT OnSDPFailure(long peerConnectionID, bool fOffer) override;
	virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) override;
	virtual RESULT OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) override;
	virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnRenegotiationNeeded(long peerConnectionID) override;
	virtual RESULT OnAddStream(long peerConnectionID, rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStream) override;
	virtual RESULT OnRemoveStream(long peerConnectionID, rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStream) override;
	virtual RESULT OnDataChannel(long peerConnectionID, rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannel) override;
	virtual RESULT OnDataChannelStateChange(long peerConnectionID, rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannel) override;

	virtual User GetUser() override;
	virtual TwilioNTSInformation GetTwilioNTSInformation() override;

	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnVideoFrame(long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;

	// TODO: AudioDeviceCapturer

private:
	RESULT ClearPeerConnections();
	rtc::scoped_refptr<WebRTCPeerConnection> AddNewPeerConnection(long peerConnectionID);
	rtc::scoped_refptr<WebRTCPeerConnection> GetPeerConnection(long peerConnectionID);
	bool FindPeerConnectionByID(long peerConnectionID);
	RESULT RemovePeerConnectionByID(long peerConnectionID);

	rtc::scoped_refptr<WebRTCPeerConnection> GetPeerConnectionByPeerUserID(long peerConnectionID);
	bool FindPeerConnectionByPeerUserID(long peerUserID);

protected:
	bool IsPeerConnectionInitialized(long peerConnectionID);
	bool IsConnected(long peerConnectionID);
	bool IsOfferer(long peerConnectionID);
	bool IsAnswerer(long peerConnectionID);
	std::list<WebRTCICECandidate> GetICECandidates(long peerConnectionID);

	std::string GetLocalSDPString(long peerConnectionID);
	std::string GetLocalSDPJSONString(long peerConnectionID);
	std::string GetRemoteSDPString(long peerConnectionID);
	std::string GetRemoteSDPJSONString(long peerConnectionID);

	RESULT CreateSDPOfferAnswer(long peerConnectionID, std::string strSDPOffer);
	RESULT SetSDPAnswer(long peerConnectionID, std::string strSDPAnswer);

	RESULT AddOfferCandidates(PeerConnection *pPeerConnection);
	RESULT AddAnswerCandidates(PeerConnection *pPeerConnection);

	WebRTCPeerConnectionProxy* GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection);

public:
	RESULT SendDataChannelStringMessageByPeerUserID(long peerUserID, std::string& strMessage);
	RESULT SendDataChannelMessageByPeerUserID(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage);
	RESULT SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT SendVideoFrame(long peerConnectionID, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT StartVideoStreaming(long peerConnectionID, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat);
	RESULT StopVideoStreaming(long peerConnectionID);
	bool IsVideoStreamingRunning(long peerConnectionID);

	RESULT SendAudioPacket(const std::string &strAudioTrackLabel, long peerConnectionID, const AudioPacket &pendingAudioPacket);
	float GetRunTimeMicAverage();

private:
	//WebRTCImp *m_pParentWebRTCImp;	// TODO: Replace this with observer interface
	WebRTCConductorObserver *m_pParentObserver;

	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;

	std::vector<rtc::scoped_refptr<WebRTCPeerConnection>> m_webRTCPeerConnections;


	// Audio Device Module
	rtc::scoped_refptr<webrtc::AudioDeviceModule> m_pAudioDeviceModule = nullptr;
	//rtc::scoped_refptr<webrtc::AudioDeviceModule> m_pAudioDeviceDummyModule = nullptr;

protected:
	// Worker and Network Threads
	// TODO: Might need to close these down on exit
	std::unique_ptr<rtc::Thread> m_networkThread = nullptr;
	std::unique_ptr<rtc::Thread> m_workerThread = nullptr;
	std::unique_ptr<rtc::Thread> m_signalingThread = nullptr;
};

#endif	// ! WEBRTC_CONDUCTOR_H_
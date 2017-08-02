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
class WebRTCICECandidate;
class PeerConnection;
class User;
class TwilioNTSInformation;

#include "WebRTCPeerConnection.h"

class WebRTCConductor : public WebRTCPeerConnection::WebRTCPeerConnectionObserver {
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
		
		virtual RESULT OnAudioData(long peerConnectionID,
			const void* audio_data,
			int bits_per_sample,
			int sample_rate,
			size_t number_of_channels,
			size_t number_of_frames) = 0;
	};

	friend class WebRTCImp;

public:
	WebRTCConductor(WebRTCConductorObserver *pParentObserver);
	~WebRTCConductor();
	
	RESULT Initialize();
	RESULT InitializeNewPeerConnection(long peerConnectionID, bool fCreateOffer, bool fAddDataChannel);

	friend class WebRTCImp;

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

	virtual RESULT OnAudioData(long peerConnectionID,
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames) override;

private:
	RESULT ClearPeerConnections();
	rtc::scoped_refptr<WebRTCPeerConnection> AddNewPeerConnection(long peerConnectionID);
	rtc::scoped_refptr<WebRTCPeerConnection> GetPeerConnection(long peerConnectionID);
	bool FindPeerConnectionByID(long peerConnectionID);

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

private:
	//WebRTCImp *m_pParentWebRTCImp;	// TODO: Replace this with observer interface
	WebRTCConductorObserver *m_pParentObserver;

	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;
	std::vector<rtc::scoped_refptr<WebRTCPeerConnection>> m_webRTCPeerConnections;
};

#endif	// ! WEBRTC_CONDUCTOR_H_
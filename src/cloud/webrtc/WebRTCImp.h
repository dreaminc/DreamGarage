#ifndef WEBRTC_IMP_H_
#define WEBRTC_IMP_H_

#include "core/ehm/EHM.h"

// Dream Cloud WebRTC
// dos/src/cloud/webrtc/WebRTCImp.h

// The WebRTC Implementation

// TODO: CEF will need to be moved to a cross platform implementation
// and this will be the top level

#include <memory>

#include "WebRTCCommon.h"

#include "WebRTCConductor.h"

#include "cloud/CloudImp.h"
#include "cloud/User/User.h"
#include "cloud/User/TwilioNTSInformation.h"

#include "core/types/Proxy.h"

//class WebRTCClient;
class WebRTCICECandidate;
class PeerConnection;

class WebRTCImpProxy : public Proxy<WebRTCImpProxy> {
public:
	virtual WebRTCPeerConnectionProxy *GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection) = 0;
};

class WebRTCImp : public CloudImp, 
				  public std::enable_shared_from_this<WebRTCImp>, 
				  public WebRTCConductor::WebRTCConductorObserver,
				  public WebRTCImpProxy
{
public:

#ifdef _WIN32
	enum WindowMessages {
		UI_THREAD_CALLBACK = WM_APP + 1,
	};
#endif

public:
	class WebRTCObserver {
	public:
		virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) = 0;
		virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) = 0;
		virtual RESULT OnSDPOfferSuccess(long peerConnectionID) = 0;
		virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) = 0;
		virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) = 0;
		virtual RESULT OnICECandidateGathered(WebRTCICECandidate *pICECandidate, long peerConnectionID) = 0;
		virtual RESULT OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) = 0;
		virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
		virtual RESULT OnRenegotiationNeeded(long peerConnectionID) = 0;
		virtual RESULT OnDataChannel(long peerConnectionID) = 0;
		virtual RESULT OnAudioChannel(long peerConnectionID) = 0;
	};

public:
	WebRTCImp(CloudController *pParentCloudController);
	~WebRTCImp();

	//friend class WebRTCClient;
	friend class WebRTCConductor;

	RESULT Shutdown();
	RESULT CloseAllPeerConnections();
	RESULT CloseWebRTCPeerConnection(PeerConnection *pPeerConnection);

	// CloudImp Interface
	RESULT Initialize();
	RESULT RegisterObserver(WebRTCObserver *pWebRTCObserver);
	virtual RESULT CreateNewURLRequest(std::wstring& strURL) override ;
	virtual RESULT Update() override;
	virtual User GetUser() override;
	virtual TwilioNTSInformation GetTwilioNTSInformation() override;

	virtual RESULT SetUser(User currentUser) override;
	virtual RESULT SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation) override;

	bool IsConnected(long peerConnectionID);
	bool IsOfferer(long peerConnectionID);
	bool IsAnswerer(long peerConnectionID);
	std::list<WebRTCICECandidate> GetCandidates(long peerConnectionID);

	// WebRTCImpProxy
	virtual WebRTCPeerConnectionProxy *GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection) override;
	virtual WebRTCImpProxy *GetProxy() override;

	// Functionality
	// TODO: Hand around PeerConnection object instead of peerConnectionID?
	RESULT InitializeNewPeerConnection(long peerConnectionID, long userID, long peerUserID, bool fCreateOffer);

	RESULT SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage);
	RESULT SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);
	RESULT SendVideoFrame(long peerConnectionID, const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT StartVideoStreaming(long peerConnectionID, const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat);
	RESULT StopVideoStreaming(long peerConnectionID, const std::string &strVideoTrackLabel);
	bool IsVideoStreamingRunning(long peerConnectionID, const std::string &strVideoTrackLabel);

	// Audio
	RESULT SendAudioPacket(const std::string &strAudioTrackLabel, long peerConnectionID, const AudioPacket &pendingAudioPacket);
	float GetRunTimeMicAverage();

	RESULT SendDataChannelStringMessageByPeerUserID(long peerUserID, std::string& strMessage);
	RESULT SendDataChannelMessageByPeerUserID(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

public:
	std::string GetLocalSDPString(long peerConnectionID);
	std::string GetRemoteSDPString(long peerConnectionID);
	std::string GetLocalSDPJSONString(long peerConnectionID);
	std::string GetRemoteSDPJSONString(long peerConnectionID);
	RESULT CreateSDPOfferAnswer(long peerConnectionID, std::string strSDPOffer);
	RESULT SetSDPAnswer(long peerConnectionID, std::string strSDPAnswer);

	RESULT AddOfferCandidates(PeerConnection *pPeerConnection);
	RESULT AddAnswerCandidates(PeerConnection *pPeerConnection);

	static std::string GetEnvVarOrDefault(const char* env_var_name, const char* default_value);

protected:
	// WebRTCConductorObserver 
	virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) override;
	virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) override;
	virtual RESULT OnSDPOfferSuccess(long peerConnectionID) override;
	virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) override;
	virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) override;
	virtual RESULT OnICECandidateGathered(WebRTCICECandidate *pICECandidate, long peerConnectionID) override;
	virtual RESULT OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) override;
	virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	virtual RESULT OnRenegotiationNeeded(long peerConnectionID) override;
	virtual RESULT OnDataChannel(long peerConnectionID) override;
	virtual RESULT OnAudioChannel(long peerConnectionID) override;

protected:

    // TODO: Move into proper win32 implementation
#ifdef _WIN32
	// WebRTC Specific
	DWORD GetUIThreadID() { return m_UIThreadID; }
#endif

private:
	std::shared_ptr<WebRTCConductor> m_pWebRTCConductor;

// TODO: Move into proper win32 implementation
#ifdef _WIN32
	rtc::Win32Thread* m_pWin32thread = nullptr;
	rtc::Win32SocketServer* m_pWin32SocketServer = nullptr;
	DWORD m_UIThreadID;
#endif

	std::string m_strServer;
	WebRTCObserver *m_pWebRTCObserver;

	User m_currentUser;
	TwilioNTSInformation m_twilioNTSInformation;
};

#endif	// ! WEBRTC_IMP_H_
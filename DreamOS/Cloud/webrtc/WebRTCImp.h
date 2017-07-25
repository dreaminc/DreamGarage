#ifndef WEBRTC_IMP_H_
#define WEBRTC_IMP_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCImp.h
// The WebRTC Implementation

// TODO: CEF will need to be moved to a cross platform implementation
// and this will be the top level
//#define WIN32_LEAN_AND_MEAN

#include <memory>

#include "WebRTCCommon.h"
#include "webrtc/base/win32socketserver.h"

#include "WebRTCConductor.h"

#include "Cloud/User/User.h"
#include "Cloud/User/TwilioNTSInformation.h"

class WebRTCClient;
class WebRTCICECandidate;
class PeerConnection;

class WebRTCImp : public CloudImp, public std::enable_shared_from_this<WebRTCImp>, public WebRTCConductor::WebRTCConductorObserver {
public:
	enum WindowMessages {
		UI_THREAD_CALLBACK = WM_APP + 1,
	};

public:
	class WebRTCObserver {
	public:
		virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) = 0;
		virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) = 0;
		virtual RESULT OnSDPOfferSuccess(long peerConnectionID) = 0;
		virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) = 0;
		virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) = 0;
		virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
		virtual RESULT OnAudioData(long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	};

public:
	WebRTCImp(CloudController *pParentCloudController);
	~WebRTCImp();

	friend class WebRTCClient;
	friend class WebRTCConductor;

	// CloudImp Interface
	RESULT Initialize();
	RESULT RegisterObserver(WebRTCObserver *pWebRTCObserver);
	RESULT CreateNewURLRequest(std::wstring& strURL);
	RESULT Update();
	virtual User GetUser() override;
	virtual TwilioNTSInformation GetTwilioNTSInformation() override;

	virtual RESULT SetUser(User currentUser) override;
	virtual RESULT SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation) override;

	bool IsConnected(long peerConnectionID);
	bool IsOfferer(long peerConnectionID);
	bool IsAnswerer(long peerConnectionID);
	std::list<WebRTCICECandidate> GetCandidates(long peerConnectionID);

	// Functionality
	// TODO: Hand around PeerConnection object instead of peerConnectionID?
	RESULT InitializeNewPeerConnection(long peerConnectionID, bool fCreateOffer);

	RESULT SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage);
	RESULT SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

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
	RESULT OnWebRTCConnectionStable(long peerConnectionID);
	RESULT OnWebRTCConnectionClosed(long peerConnectionID);
	RESULT OnSDPOfferSuccess(long peerConnectionID);
	RESULT OnSDPAnswerSuccess(long peerConnectionID);
	RESULT OnICECandidatesGatheringDone(long peerConnectionID);
	RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage);
	RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);
	RESULT OnAudioData(long peerConnectionID,
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames);

protected:
	// WebRTC Specific
	DWORD GetUIThreadID() { return m_UIThreadID; }

private:
	std::shared_ptr<WebRTCConductor> m_pWebRTCConductor;
	rtc::Win32Thread* m_pWin32thread;

	DWORD m_UIThreadID;
	std::string m_strServer;
	WebRTCObserver *m_pWebRTCObserver;

	User m_currentUser;
	TwilioNTSInformation m_twilioNTSInformation;
};

#endif	// ! WEBRTC_IMP_H_
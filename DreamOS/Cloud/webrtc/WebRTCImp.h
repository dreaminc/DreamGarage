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

#include "webrtc/base/win32socketserver.h"

const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const char kDataLabel[] = "data_label";
const uint16_t kDefaultServerPort = 8888;

class WebRTCClient;
class WebRTCConductor;
class ICECandidate;

class WebRTCImp : public CloudImp, public std::enable_shared_from_this<WebRTCImp> {
public:
	enum WindowMessages {
		UI_THREAD_CALLBACK = WM_APP + 1,
	};

public:
	class WebRTCObserver {
	public:
		virtual RESULT OnSDPOfferSuccess() = 0;
		virtual RESULT OnSDPAnswerSuccess() = 0;
		virtual RESULT OnICECandidatesGatheringDone() = 0;
		virtual RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
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
	bool IsConnected();
	bool IsOfferer();
	bool IsAnswerer();
	std::list<ICECandidate> GetCandidates();

	// Functionality
	RESULT StartLogin(const std::string& server, int port);
	//RESULT InitializeConnection(bool fMaster, bool fAddDataChannel);
	//RESULT InitializePeerConnection(bool fAddDataChannel);
	RESULT InitializePeerConnection(bool fCreateOffer, bool fAddDataChannel = true);
	int GetFirstPeerID();
	
	virtual RESULT ConnectToPeer(int peerID) override;
	virtual std::function<void(int msg_id, void* data)> GetUIThreadCallback() override;
	void QueueUIThreadCallback(int msg_id, void* data);
	virtual RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage) override;
	virtual RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;

public:
	// Utilities
	std::string GetSDPString();

	static std::string GetEnvVarOrDefault(const char* env_var_name, const char* default_value);
	static std::string GetPeerName();
	virtual std::string GetSDPOfferString() override;
	//virtual RESULT InitializeConnection(bool fMaster, bool fAddDataChannel) override;
	virtual RESULT CreateSDPOfferAnswer(std::string strSDPOffer) override;
	virtual RESULT AddIceCandidates() override;

protected:
	RESULT OnSignedIn();
	RESULT OnDisconnected();
	RESULT OnPeerConnected(int id, const std::string& name);
	RESULT OnPeerDisconnected(int peer_id);
	RESULT OnMessageFromPeer(int peerID, const std::string& strMessage);
	RESULT OnMessageSent(int err);
	RESULT OnServerConnectionFailure();
	RESULT OnSDPOfferSuccess();
	RESULT OnSDPAnswerSuccess();
	RESULT OnICECandidatesGatheringDone();
	RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage);
	RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

protected:
	// WebRTC Specific
	//RESULT QueueUIThreadCallback(int msgID, void* data);
	DWORD GetUIThreadID() { return m_UIThreadID; }

private:
	std::shared_ptr<WebRTCClient> m_pWebRTCClient;
//	std::shared_ptr<WebRTCConductor> m_pWebRTCConductor;
	rtc::scoped_refptr<WebRTCConductor> m_pWebRTCConductor;
	//std::shared_ptr<rtc::Win32Thread> m_pWin32thread;
	rtc::Win32Thread m_Win32thread;

	DWORD m_UIThreadID;

	std::string m_strServer;

	WebRTCObserver *m_pWebRTCObserver;
};

#endif	// ! WEBRTC_IMP_H_
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
const uint16_t kDefaultServerPort = 8888;

class WebRTCClient;
class WebRTCConductor;

class WebRTCImp : public CloudImp, public std::enable_shared_from_this<WebRTCImp> {
public:
	enum WindowMessages {
		UI_THREAD_CALLBACK = WM_APP + 1,
	};

public:
	WebRTCImp();
	~WebRTCImp();

	friend class WebRTCClient;
	friend class WebRTCConductor;

	// CloudImp Interface
	RESULT Initialize();
	RESULT CreateNewURLRequest(std::wstring& strURL);
	RESULT Update();

	// Functionality
	RESULT StartLogin(const std::string& server, int port);

public:
	// Utilities
	static std::string GetEnvVarOrDefault(const char* env_var_name, const char* default_value);
	static std::string GetPeerName();

protected:
	RESULT OnSignedIn();
	RESULT OnDisconnected();
	RESULT OnPeerConnected(int id, const std::string& name);
	RESULT OnPeerDisconnected(int peer_id);
	RESULT OnMessageFromPeer(int peer_id, const std::string& message);
	RESULT OnMessageSent(int err);
	RESULT OnServerConnectionFailure();

protected:
	// WebRTC Specific
	RESULT QueueUIThreadCallback(int msg_id, void* data);
	DWORD GetUIThreadID() { return m_UIThreadID; }

private:
	std::shared_ptr<WebRTCClient> m_pWebRTCClient;
	std::shared_ptr<WebRTCConductor> m_pWebRTCConductor;
	//std::shared_ptr<rtc::Win32Thread> m_pWin32thread;
	rtc::Win32Thread m_Win32thread;

	DWORD m_UIThreadID;

	std::string m_strServer;
};

#endif	// ! WEBRTC_IMP_H_
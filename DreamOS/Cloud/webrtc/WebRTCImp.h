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

#include <algorithm>

#include "webrtc/base/win32.h"

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"
#include "webrtc/base/refcount.h"

#include "WebRTCClient.h"
#include "WebRTCConductor.h"

const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const uint16_t kDefaultServerPort = 8888;

class WebRTCImp : public CloudImp {
public:
	enum WindowMessages {
		UI_THREAD_CALLBACK = WM_APP + 1,
	};

public:
	WebRTCImp();
	~WebRTCImp();

	// CloudImp Interface
	RESULT Initialize();
	RESULT CreateNewURLRequest(std::wstring& strURL);
	RESULT Update();

	friend class WebRTCClient;
	friend class WebRTCConductor;

protected:
	// WebRTC Specific
	RESULT QueueUIThreadCallback(int msg_id, void* data);
	DWORD GetUIThreadID() { return m_UIThreadID; }

private:
	WebRTCClient m_WebRTCClient;
	rtc::Win32Thread m_Win32thread;

	DWORD m_UIThreadID;
};

#endif	// ! WEBRTC_IMP_H_
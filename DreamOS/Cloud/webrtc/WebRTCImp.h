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

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"

class WebRTCImp : public CloudImp {
public:
	WebRTCImp() :
		CloudImp()
	{
		// TodO
	}

	~WebRTCImp() {
		// TODO
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		rtc::EnsureWinsockInit();
		rtc::ThreadManager::Instance()->SetCurrentThread(&m_Win32thread);

	Error:
		return r;
	}

	RESULT CreateNewURLRequest(std::wstring& strURL) {
		return R_NOT_IMPLEMENTED;
	}

	// Will simply update the message loop as needed
	RESULT Update() {
		return R_NOT_IMPLEMENTED;
	}

private:
	rtc::Win32Thread m_Win32thread;
};

#endif	// ! WEBRTC_IMP_H_
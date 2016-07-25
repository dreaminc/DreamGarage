#ifndef WEBRTC_IMP_H_
#define WEBRTC_IMP_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCImp.h
// The WebRTC Implementation

class WebRTCImp : public CloudImp {
public:
	WebRTCImp() :
		CloudImp()
	{
		// TOdO
	}

	~WebRTCImp() {
		// TODO
	}


	RESULT CreateNewURLRequest(std::wstring& strURL) {
		return R_NOT_IMPLEMENTED;
	}

	// Will simply update the message loop as needed
	RESULT Update() {
		return R_NOT_IMPLEMENTED;
	}

private:
	
};

#endif	// ! WEBRTC_IMP_H_
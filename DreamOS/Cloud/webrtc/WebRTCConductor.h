#ifndef WEBRTC_CONDUCTOR_H_
#define WEBRTC_CONDUCTOR_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCConductor.h
// The WebRTC Conductor which conducts the peer connection and generally handles the WebRTC 
// connections

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"

class WebRTCConductor : public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver /*, public PeerConnectionClientObserver, public MainWndCallback */
{
public:
	WebRTCConductor() {
		// TODO
	}
}

#endif	// ! WEBRTC_CONDUCTOR_H_
#ifndef WEBRTC_CLIENT_H_
#define WEBRTC_CLIENT_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCClient.h
// The WebRTC Client that handles all WebRTC messages 

#include <map>

#include "webrtc/base/nethelpers.h"
#include "webrtc/base/physicalsocketserver.h"
#include "webrtc/base/signalthread.h"
#include "webrtc/base/sigslot.h"

class WebRTCClient : public sigslot::has_slots<>, public rtc::MessageHandler {
public:
	enum State {
		UNINITIALIZED,
		NOT_CONNECTED,
		RESOLVING,
		SIGNING_IN,
		CONNECTED,
		SIGNING_OUT_WAITING,
		SIGNING_OUT,
	};

public:
	WebRTCClient();
	~WebRTCClient();

	// implements the MessageHandler interface
	void OnMessage(rtc::Message* msg);

	RESULT SignOut();

	int GetID() const { return m_WebRTCID; }
	State GetState() const { return m_WebRTCState; }
	bool IsConnected() const { return (m_WebRTCID != -1); }

private:
	RESULT ConnectControlSocket();

private:
	RESULT Close();

private:
	State m_WebRTCState;
	int m_WebRTCID;

	rtc::SocketAddress m_SocketAddressServer;

	rtc::AsyncResolver* m_pAsyncResolver;
	std::unique_ptr<rtc::AsyncSocket> m_pAsyncSocketControl;
	std::unique_ptr<rtc::AsyncSocket> m_pAsyncSocketHangingGet;
	std::string m_strOnConnectData;

	std::map<int, std::string> m_peers;
};

#endif	// ! WEBRTC_CLIENT_H_
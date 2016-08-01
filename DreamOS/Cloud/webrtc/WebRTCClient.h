#ifndef WEBRTC_CLIENT_H_
#define WEBRTC_CLIENT_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCClient.h
// The WebRTC Client that handles all WebRTC messages 

#include <map>
#include <memory>
#include <string>

#include "webrtc/base/nethelpers.h"
#include "webrtc/base/physicalsocketserver.h"
#include "webrtc/base/signalthread.h"
#include "webrtc/base/sigslot.h"

class WebRTCImp;

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
	WebRTCClient(const std::shared_ptr<WebRTCImp> &pParentWebRTCImp);
	~WebRTCClient();

	// implements the MessageHandler interface
	void OnMessage(rtc::Message* msg);
	void OnClose(rtc::AsyncSocket* socket, int err);	// TODO: Not implemented?
	void OnConnect(rtc::AsyncSocket* socket);
	void OnHangingGetConnect(rtc::AsyncSocket* socket);
	void OnRead(rtc::AsyncSocket* socket);
	void OnHangingGetRead(rtc::AsyncSocket* socket);

	void OnMessageFromPeer(int peer_id, const std::string& message);

	RESULT SignOut();
	RESULT Connect(const std::string& strServer, int port, const std::string& strClientName);

	int WebRTCClient::GetID() const {
		return m_WebRTCID;
	}

	State WebRTCClient::GetState() const {
		return m_WebRTCState;
	}

	bool WebRTCClient::IsConnected() const {
		return (m_WebRTCID != -1);
	}

private:
	RESULT DoConnect();
	RESULT ConnectControlSocket();
	RESULT InitSocketSignals();

private:
	RESULT Close();

protected:
	void OnResolveResult(rtc::AsyncResolverInterface* resolver);

private:
	State m_WebRTCState;
	int m_WebRTCID;

	rtc::SocketAddress m_SocketAddressServer;
	std::string m_strClientName;

	rtc::AsyncResolver* m_pAsyncResolver;
	std::unique_ptr<rtc::AsyncSocket> m_pAsyncSocketControl;
	std::unique_ptr<rtc::AsyncSocket> m_pAsyncSocketHangingGet;
	std::string m_strOnConnectData;

	std::map<int, std::string> m_peers;

	std::shared_ptr<WebRTCImp> m_pParentWebRTCImp;
};

#endif	// ! WEBRTC_CLIENT_H_
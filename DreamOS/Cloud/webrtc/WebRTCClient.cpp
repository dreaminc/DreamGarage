#include "WebRTCClient.h"

#include "webrtc/base/stringutils.h"

WebRTCClient::WebRTCClient() :
	m_WebRTCID(-1),
	m_WebRTCState(UNINITIALIZED)
{
	// empty
}

WebRTCClient::~WebRTCClient() {
	// empty
}


void WebRTCClient::OnMessage(rtc::Message* msg) {

}

RESULT WebRTCClient::SignOut() {
	RESULT r = R_PASS;

	CBR((m_WebRTCState != NOT_CONNECTED), R_PASS); 
	CBR((m_WebRTCState != SIGNING_OUT), R_PASS);

	if (m_pAsyncSocketHangingGet->GetState() != rtc::Socket::CS_CLOSED) {
		m_pAsyncSocketHangingGet->Close();
	}

	if (m_pAsyncSocketControl->GetState() == rtc::Socket::CS_CLOSED) {
		m_WebRTCState = SIGNING_OUT;

		// Can occur if the app is closed before we finish connecting.
		CB((m_WebRTCID == -1), R_PASS);

		char buffer[1024];
		rtc::sprintfn(buffer, sizeof(buffer), "GET /sign_out?peer_id=%i HTTP/1.0\r\n\r\n", m_WebRTCID);
		m_strOnConnectData = buffer;

		return ConnectControlSocket();
	}
	else {
		m_WebRTCState = SIGNING_OUT_WAITING;
	}

Error:
	return r;
}

RESULT WebRTCClient::ConnectControlSocket() {
	RESULT r = R_PASS;

	CB((m_pAsyncSocketControl->GetState() == rtc::Socket::CS_CLOSED));

	int err = m_pAsyncSocketControl->Connect(m_SocketAddressServer);
	
	if (err == SOCKET_ERROR) {
		Close();
		return R_FAIL;
	}
	
Error:
	return r;
}

RESULT WebRTCClient::Close() {
	RESULT r = R_PASS;

	m_pAsyncSocketControl->Close();
	m_pAsyncSocketHangingGet->Close();
	m_strOnConnectData.clear();
	m_peers.clear();
	
	if (m_pAsyncResolver != nullptr) {
		m_pAsyncResolver->Destroy(false);
		m_pAsyncResolver = nullptr;
	}
	
	m_WebRTCID = -1;
	m_WebRTCState = NOT_CONNECTED;

Error:
	return r;
}
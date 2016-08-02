#include "WebRTCClient.h"

#include "WebRTCImp.h"
#include "WebRTCConductor.h"

//WebRTCClient::WebRTCClient() :
WebRTCClient::WebRTCClient(std::shared_ptr<WebRTCImp> pParentWebRTCImp) :
	m_WebRTCID(-1),
	m_WebRTCState(UNINITIALIZED),
	m_pAsyncResolver(nullptr),
	m_pParentWebRTCImp(pParentWebRTCImp)
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

/*
void WebRTCClient::OnConnect(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;

	ASSERT(!onconnect_data_.empty());
	size_t sent = socket->Send(onconnect_data_.c_str(), onconnect_data_.length());
	ASSERT(sent == onconnect_data_.length());
	RTC_UNUSED(sent);
	onconnect_data_.clear();

Error:
	return;
}

void WebRTCClient::OnHangingGetConnect(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;
	char buffer[1024];

	rtc::sprintfn(buffer, sizeof(buffer), "GET /wait?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
	int len = static_cast<int>(strlen(buffer));
	int sent = socket->Send(buffer, len);

	CB((sent == len));
	RTC_UNUSED2(sent, len);

Error:
	return;
}

void WebRTCClient::OnMessageFromPeer(int peer_id, const std::string& message) {
	RESULT r = R_PASS;

	if (message.length() == (sizeof(kByeMessage) - 1) && message.compare(kByeMessage) == 0) {
		//callback_->OnPeerDisconnected(peer_id);
		// TODO
	}
	else {
		//callback_->OnMessageFromPeer(peer_id, message);
		// TODO
	}

Error:
	return;
}

void WebRTCClient::OnRead(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;

	size_t content_length = 0;
	if (ReadIntoBuffer(socket, &control_data_, &content_length)) {
		size_t peer_id = 0, eoh = 0;
		bool ok = ParseServerResponse(control_data_, content_length, &peer_id,
			&eoh);
		if (ok) {
			if (my_id_ == -1) {
				// First response.  Let's store our server assigned ID.
				ASSERT(state_ == SIGNING_IN);
				my_id_ = static_cast<int>(peer_id);
				ASSERT(my_id_ != -1);

				// The body of the response will be a list of already connected peers.
				if (content_length) {
					size_t pos = eoh + 4;
					while (pos < control_data_.size()) {
						size_t eol = control_data_.find('\n', pos);
						if (eol == std::string::npos)
							break;
						int id = 0;
						std::string name;
						bool connected;
						if (ParseEntry(control_data_.substr(pos, eol - pos), &name, &id,
							&connected) && id != my_id_) {
							peers_[id] = name;
							callback_->OnPeerConnected(id, name);
						}
						pos = eol + 1;
					}
				}
				ASSERT(is_connected());
				callback_->OnSignedIn();
			}
			else if (state_ == SIGNING_OUT) {
				Close();
				callback_->OnDisconnected();
			}
			else if (state_ == SIGNING_OUT_WAITING) {
				SignOut();
			}
		}

		control_data_.clear();

		if (state_ == SIGNING_IN) {
			ASSERT(hanging_get_->GetState() == rtc::Socket::CS_CLOSED);
			state_ = CONNECTED;
			hanging_get_->Connect(server_address_);
		}
	}

Error:
	return;
}

void WebRTCClient::OnHangingGetRead(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;

	LOG(INFO) << __FUNCTION__;
	size_t content_length = 0;
	if (ReadIntoBuffer(socket, &notification_data_, &content_length)) {
		size_t peer_id = 0, eoh = 0;
		bool ok = ParseServerResponse(notification_data_, content_length,
			&peer_id, &eoh);

		if (ok) {
			// Store the position where the body begins.
			size_t pos = eoh + 4;

			if (my_id_ == static_cast<int>(peer_id)) {
				// A notification about a new member or a member that just
				// disconnected.
				int id = 0;
				std::string name;
				bool connected = false;
				if (ParseEntry(notification_data_.substr(pos), &name, &id,
					&connected)) {
					if (connected) {
						peers_[id] = name;
						callback_->OnPeerConnected(id, name);
					}
					else {
						peers_.erase(id);
						callback_->OnPeerDisconnected(id);
					}
				}
			}
			else {
				OnMessageFromPeer(static_cast<int>(peer_id),
					notification_data_.substr(pos));
			}
		}

		notification_data_.clear();
	}

	if (hanging_get_->GetState() == rtc::Socket::CS_CLOSED &&
		state_ == CONNECTED) {
		hanging_get_->Connect(server_address_);
	}

Error:
	return;
}
*/

RESULT WebRTCClient::InitSocketSignals() {
	RESULT r = R_PASS;

	CN(m_pAsyncSocketControl);
	CN(m_pAsyncSocketHangingGet);

	m_pAsyncSocketControl->SignalCloseEvent.connect(this, &WebRTCClient::OnClose);
	m_pAsyncSocketHangingGet->SignalCloseEvent.connect(this, &WebRTCClient::OnClose);

	m_pAsyncSocketControl->SignalConnectEvent.connect(this, &WebRTCClient::OnConnect);
	m_pAsyncSocketHangingGet->SignalConnectEvent.connect(this, &WebRTCClient::OnHangingGetConnect);
	
	m_pAsyncSocketControl->SignalReadEvent.connect(this, &WebRTCClient::OnRead);
	m_pAsyncSocketHangingGet->SignalReadEvent.connect(this, &WebRTCClient::OnHangingGetRead);

Error:
	return r;
}


namespace {

	// This is our magical hangup signal.
	const char kByeMessage[] = "BYE";
	// Delay between server connection retries, in milliseconds
	const int kReconnectDelay = 2000;

	rtc::AsyncSocket* CreateClientSocket(int family) {
#ifdef WIN32
		rtc::Win32Socket* sock = new rtc::Win32Socket();
		sock->CreateT(family, SOCK_STREAM);
		return sock;
#elif defined(WEBRTC_POSIX)
		rtc::Thread* thread = rtc::Thread::Current();
		ASSERT(thread != NULL);
		return thread->socketserver()->CreateAsyncSocket(family, SOCK_STREAM);
#else
#error Platform not supported.
#endif
	}

}  // namespace

RESULT WebRTCClient::DoConnect() {
	RESULT r = R_PASS;

	m_pAsyncSocketControl.reset(CreateClientSocket(m_SocketAddressServer.ipaddr().family()));
	m_pAsyncSocketHangingGet.reset(CreateClientSocket(m_SocketAddressServer.ipaddr().family()));

	InitSocketSignals();
	char buffer[1024];
	
	rtc::sprintfn(buffer, sizeof(buffer), "GET /sign_in?%s HTTP/1.0\r\n\r\n", m_strClientName.c_str());
	m_strOnConnectData = buffer;

	if (ConnectControlSocket() == R_PASS) {
		m_WebRTCState = SIGNING_IN;
	}
	else {
		// TODO:
#pragma message("TODO")
		//callback_->OnServerConnectionFailure();
	}

Error:
	return r;
}

RESULT WebRTCClient::Connect(const std::string& strServer, int port, const std::string& strClientName) {
	RESULT r = R_PASS;

	CB((!strServer.empty()));
	CB((!strClientName.empty()));

	CBM((m_WebRTCState != NOT_CONNECTED), "The client must not be connected before you can call Connect()");
	// TODO:
	//callback_->OnServerConnectionFailure();

	CB((strServer.empty() || strServer.empty()));
	// TODO: 
	// callback_->OnServerConnectionFailure();

	if (port <= 0) {
		port = kDefaultServerPort;
	}

	m_SocketAddressServer.SetIP(strServer);
	m_SocketAddressServer.SetPort(port);
	m_strClientName = strClientName;

	if (m_SocketAddressServer.IsUnresolvedIP()) {
		m_WebRTCState = RESOLVING;

		m_pAsyncResolver = new rtc::AsyncResolver();
		m_pAsyncResolver->SignalDone.connect(this, &WebRTCClient::OnResolveResult);
		m_pAsyncResolver->Start(m_SocketAddressServer);
	}
	else {
		DoConnect();
	}

Error:
	return r;
}

void WebRTCClient::OnResolveResult(rtc::AsyncResolverInterface* resolver) {
	RESULT r = R_PASS;

	if (m_pAsyncResolver->GetError() != 0) {
		m_pParentWebRTCImp->OnServerConnectionFailure();
		
		m_pAsyncResolver->Destroy(false);
		
		m_pAsyncResolver = nullptr;
		m_WebRTCState = NOT_CONNECTED;
	}
	else {
		m_SocketAddressServer = m_pAsyncResolver->address();
		DoConnect();
	}

Error:
	return;
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
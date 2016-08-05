#include "WebRTCClient.h"

#include "WebRTCImp.h"
#include "WebRTCConductor.h"

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

//WebRTCClient::WebRTCClient() :
WebRTCClient::WebRTCClient(WebRTCImp* pParentWebRTCImp) :
	m_WebRTCID(-1),
	m_WebRTCState(UNINITIALIZED),
	m_pAsyncResolver(nullptr),
	m_pParentWebRTCImp(nullptr)
{
	m_pParentWebRTCImp = std::shared_ptr<WebRTCImp>(pParentWebRTCImp);
}

WebRTCClient::~WebRTCClient() {
	// empty
}

void WebRTCClient::OnMessage(rtc::Message* msg) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCClient: OnMessage");

	// ignore msg; there is currently only one supported message ("retry")
	CR(DoConnect());

Error:
	return;
}

void WebRTCClient::OnClose(rtc::AsyncSocket* socket, int err) {
	RESULT r = R_PASS;

	LOG(INFO) << __FUNCTION__;

	socket->Close();

#ifdef WIN32
	if (err != WSAECONNREFUSED) {
#else
	if (err != ECONNREFUSED) {
#endif
		if (socket == m_pAsyncSocketHangingGet.get()) {
			if (m_WebRTCState == CONNECTED) {
				m_pAsyncSocketHangingGet->Close();
				m_pAsyncSocketHangingGet->Connect(m_SocketAddressServer);
			}
		}
		else {
			CR(m_pParentWebRTCImp->OnMessageSent(err));
		}
	}
	else {
		if (socket == m_pAsyncSocketControl.get()) {
			LOG(WARNING) << "Connection refused; retrying in 2 seconds";
			rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kReconnectDelay, this, 0);
		}
		else {
			Close();
			CR(m_pParentWebRTCImp->OnDisconnected());
		}
	}

Error:
	return;
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

void WebRTCClient::OnConnect(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCClient: OnConnect");

	CB((!m_strOnConnectData.empty()));

	size_t sent = socket->Send(m_strOnConnectData.c_str(), m_strOnConnectData.length());
	CB((sent == m_strOnConnectData.length()));
	
	RTC_UNUSED(sent);
	
	m_strOnConnectData.clear();

Error:
	return;
}

void WebRTCClient::OnHangingGetConnect(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;
	char buffer[1024];

	DEBUG_LINEOUT("WebRTCClient: OnHangingGetConnect");

	rtc::sprintfn(buffer, sizeof(buffer), "GET /wait?peer_id=%i HTTP/1.0\r\n\r\n", m_WebRTCID);
	int len = static_cast<int>(strlen(buffer));
	int sent = socket->Send(buffer, len);

	CB((sent == len));
	RTC_UNUSED2(sent, len);

Error:
	return;
}

void WebRTCClient::OnMessageFromPeer(int peer_id, const std::string& message) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCClient: OnMessageFromPeer");

	if (message.length() == (sizeof(kByeMessage) - 1) && message.compare(kByeMessage) == 0) {
		CR(m_pParentWebRTCImp->OnPeerDisconnected(peer_id));
	}
	else {
		CR(m_pParentWebRTCImp->OnMessageFromPeer(peer_id, message));
	}

Error:
	return;
}

int WebRTCClient::GetResponseStatus(const std::string& response) {
	int status = -1;
	
	size_t pos = response.find(' ');
	
	if (pos != std::string::npos)
		status = atoi(&response[pos + 1]);

	return status;
}

RESULT WebRTCClient::ParseServerResponse(const std::string& response, size_t content_length, size_t* peer_id, size_t* eoh) {
	RESULT r = R_PASS;

	int status = GetResponseStatus(response.c_str());
	CB((status == 200));

	*eoh = response.find("\r\n\r\n");
	CB((*eoh != std::string::npos));

	*peer_id = -1;

	// See comment in peer_channel.cc for why we use the Pragma header and not e.g. "X-Peer-Id".
	CR(GetHeaderValue(response, *eoh, "\r\nPragma: ", peer_id));

Success:
	return r;

Error:
	CR(m_pParentWebRTCImp->OnDisconnected());
	LOG(LS_ERROR) << "Received error from server";
	Close();
	return r;
}

RESULT WebRTCClient::ParseEntry(const std::string& entry, std::string* name, int* id, bool* connected) {
	RESULT r = R_PASS;

	CB((name != nullptr));
	CB((id != nullptr));
	CB((connected != nullptr));
	CB((!entry.empty()));

	*connected = false;
	size_t separator = entry.find(',');

	if (separator != std::string::npos) {
		*id = atoi(&entry[separator + 1]);
		name->assign(entry.substr(0, separator));
		separator = entry.find(',', separator + 1);
		
		if (separator != std::string::npos) 
			*connected = atoi(&entry[separator + 1]) ? true : false;
	}

	CB((!name->empty()));

Error:
	return r;
}

RESULT WebRTCClient::GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, size_t* value) {
	RESULT r = R_PASS;

	CB((value != nullptr));
	size_t found = data.find(header_pattern);
	
	CB((found != std::string::npos));
	CB((found < eoh));

	*value = atoi(&data[found + strlen(header_pattern)]);

Error:
	return r;
}

RESULT WebRTCClient::GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, std::string* value) {
	RESULT r = R_PASS;

	CB((value != nullptr));

	size_t found = data.find(header_pattern);

	CB((found != std::string::npos));
	CB((found < eoh));

	size_t begin = found + strlen(header_pattern);
	size_t end = data.find("\r\n", begin);
	
	if (end == std::string::npos)
		end = eoh;

	value->assign(data.substr(begin, end - begin));

Error:
	return r;
}

RESULT WebRTCClient::ReadIntoBuffer(rtc::AsyncSocket* socket, std::string* data, size_t* content_length) {
	RESULT r = R_PASS;
	char buffer[0xffff];

	do {
		int bytes = socket->Recv(buffer, sizeof(buffer), nullptr);

		if (bytes <= 0) 
			break;

		data->append(buffer, bytes);
	} while (true);

	bool ret = false;
	size_t i = data->find("\r\n\r\n");

	if (i != std::string::npos) {
		LOG(INFO) << "Headers received";
		if (GetHeaderValue(*data, i, "\r\nContent-Length: ", content_length)) {
			size_t total_response_size = (i + 4) + *content_length;
			if (data->length() >= total_response_size) {
				ret = true;
				std::string should_close;
				const char kConnection[] = "\r\nConnection: ";

				if (GetHeaderValue(*data, i, kConnection, &should_close) && should_close.compare("close") != R_PASS) {
					socket->Close();
					
					// Since we closed the socket, there was no notification delivered
					// to us.  Compensate by letting ourselves know.
					OnClose(socket, 0);
				}
			}
			else {
				// We haven't received everything.  Just continue to accept data.
			}
		}
		else {
			LOG(LS_ERROR) << "No content length field specified by the server.";
		}
	}

Error:
	return r;
}

void WebRTCClient::OnRead(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCClient: OnRead");

	size_t content_length = 0;
	if (ReadIntoBuffer(socket, &m_strControlData, &content_length)) {
		size_t peer_id = 0, eoh = 0;

		CR(ParseServerResponse(m_strControlData, content_length, &peer_id, &eoh));

		if (m_WebRTCID == -1) {
			// First response.  Let's store our server assigned ID.
			CB((m_WebRTCState == SIGNING_IN));
				
			m_WebRTCID = static_cast<int>(peer_id);
			CB((m_WebRTCID != -1));

			// The body of the response will be a list of already connected peers.
			if (content_length) {
				size_t pos = eoh + 4;
				while (pos < m_strControlData.size()) {
					size_t eol = m_strControlData.find('\n', pos);
					if (eol == std::string::npos)
						break;
					int id = 0;
					std::string name;
					bool connected;

					if (ParseEntry(m_strControlData.substr(pos, eol - pos), &name, &id, &connected) && id != m_WebRTCID) {
						m_peers[id] = name;
						CR(m_pParentWebRTCImp->OnPeerConnected(id, name));
					}
					pos = eol + 1;
				}
			}

			CB((IsConnected()));
			CR(m_pParentWebRTCImp->OnSignedIn());
		}
		else if (m_WebRTCState == SIGNING_OUT) {
			Close();
			CR(m_pParentWebRTCImp->OnDisconnected());
		}
		else if (m_WebRTCState == SIGNING_OUT_WAITING) {
			SignOut();
		}

		m_strControlData.clear();

		if (m_WebRTCState == SIGNING_IN) {
			CB((m_pAsyncSocketHangingGet->GetState() == rtc::Socket::CS_CLOSED));
			m_WebRTCState = CONNECTED;
			m_pAsyncSocketHangingGet->Connect(m_SocketAddressServer);
		}
	}

Error:
	return;
}

void WebRTCClient::OnHangingGetRead(rtc::AsyncSocket* socket) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCClient: OnHandingGetRead");

	LOG(INFO) << __FUNCTION__;
	size_t content_length = 0;
	if (ReadIntoBuffer(socket, &m_strNotificatonData, &content_length)) {

		size_t peer_id = 0, eoh = 0;
		CR(ParseServerResponse(m_strNotificatonData, content_length, &peer_id, &eoh));

		// Store the position where the body begins.
		size_t pos = eoh + 4;

		if (m_WebRTCID == static_cast<int>(peer_id)) {
			// A notification about a new member or a member that just
			// disconnected.
			int id = 0;
			std::string name;
			bool connected = false;

			// TODO: The ID is server assigned, this has got to go!
			if (ParseEntry(m_strNotificatonData.substr(pos), &name, &id, &connected) == R_PASS) {
				if (connected) {
					m_peers[id] = name;
					CR(m_pParentWebRTCImp->OnPeerConnected(id, name));
				}
				else {
					m_peers.erase(id);
					CR(m_pParentWebRTCImp->OnPeerDisconnected(id));
				}
			}
		}
		else {
			OnMessageFromPeer(static_cast<int>(peer_id), m_strNotificatonData.substr(pos));
		}
	}

	m_strNotificatonData.clear();

	if (m_pAsyncSocketHangingGet->GetState() == rtc::Socket::CS_CLOSED && m_WebRTCState == CONNECTED) {
		m_pAsyncSocketHangingGet->Connect(m_SocketAddressServer);
	}

Error:
	return;
}

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

RESULT WebRTCClient::DoConnect() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCClient: DoConnect");

	m_pAsyncSocketControl.reset(CreateClientSocket(m_SocketAddressServer.ipaddr().family()));
	m_pAsyncSocketHangingGet.reset(CreateClientSocket(m_SocketAddressServer.ipaddr().family()));

	CR(InitSocketSignals());
	char buffer[1024];
	
	rtc::sprintfn(buffer, sizeof(buffer), "GET /sign_in?%s HTTP/1.0\r\n\r\n", m_strClientName.c_str());
	m_strOnConnectData = buffer;

	if (ConnectControlSocket() == R_PASS) {
		m_WebRTCState = SIGNING_IN;
	}
	else {
		CR(m_pParentWebRTCImp->OnServerConnectionFailure());
	}

Error:
	return r;
}

// This will send the data to the server to be passed to the other peer
RESULT WebRTCClient::SendMessageToPeer(int peerID, const std::string& message) {
	RESULT r = R_PASS;

	CB((m_WebRTCState == CONNECTED));

	CB((IsConnected()));
	CB((m_pAsyncSocketControl->GetState() == rtc::Socket::CS_CLOSED));
	CB(peerID != -1);

	char headers[1024];
	rtc::sprintfn(headers, sizeof(headers),
		"POST /message?peer_id=%i&to=%i HTTP/1.0\r\n"
		"Content-Length: %i\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n",
		m_WebRTCID, peerID, message.length());

	m_strOnConnectData = headers;
	m_strOnConnectData += message;
	
	CR(ConnectControlSocket());

Error:
	return r;
}

RESULT WebRTCClient::Connect(const std::string& strServer, int port, const std::string& strClientName) {
	RESULT r = R_PASS;

	CB((!strServer.empty()));
	CB((!strClientName.empty()));

	CBM((m_WebRTCState != NOT_CONNECTED), "The client must not be connected before you can call Connect()");

	CB((!strServer.empty()) || (!strServer.empty()));

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

Success:
	return r;

Error:
	m_pParentWebRTCImp->OnServerConnectionFailure();
	return r;
}

void WebRTCClient::OnResolveResult(rtc::AsyncResolverInterface* resolver) {
	RESULT r = R_PASS;

	if (m_pAsyncResolver->GetError() != 0) {
		CR(m_pParentWebRTCImp->OnServerConnectionFailure());
		
		m_pAsyncResolver->Destroy(false);
		
		m_pAsyncResolver = nullptr;
		m_WebRTCState = NOT_CONNECTED;
	}
	else {
		m_SocketAddressServer = m_pAsyncResolver->address();
		CR(DoConnect());
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
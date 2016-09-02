#include "WebRTCImp.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"

#include "WebRTCClient.h"
#include "WebRTCConductor.h"

#include "webrtc/base/arraysize.h"
#include "Cloud/CloudController.h"
#include "Cloud/Environment/PeerConnection.h"

WebRTCImp::WebRTCImp(CloudController *pParentCloudController) :
	CloudImp(pParentCloudController),
	m_pWebRTCConductor(nullptr),
	m_pWebRTCObserver(nullptr)
{
	// empty
}

WebRTCImp::~WebRTCImp() {
	rtc::CleanupSSL();

	if (m_pWebRTCConductor != nullptr) {
		m_pWebRTCConductor.release();
	}
}

// CloudImp Interface
RESULT WebRTCImp::Initialize() {
	RESULT r = R_PASS;

	rtc::EnsureWinsockInit();
	rtc::ThreadManager::Instance()->SetCurrentThread(&m_Win32thread);
	rtc::InitializeSSL();

	// TODO: Remove client - not doing anything clearly
	//m_pWebRTCClient = std::make_shared<WebRTCClient>(this);
	//CN(m_pWebRTCClient);

	m_pWebRTCConductor = rtc::scoped_refptr<WebRTCConductor>(new rtc::RefCountedObject<WebRTCConductor>(m_pWebRTCClient.get(), this));
	CN(m_pWebRTCConductor);
	CR(m_pWebRTCConductor->Initialize());

Error:
	return r;
}

RESULT WebRTCImp::RegisterObserver(WebRTCObserver *pWebRTCObserver) {
	RESULT r = R_PASS;

	CNM(pWebRTCObserver, "Can't register null observer");
	CBM((m_pWebRTCObserver == nullptr), "Observer already registered");

	m_pWebRTCObserver = pWebRTCObserver;

Error:
	return r;
}

RESULT WebRTCImp::CreateNewURLRequest(std::wstring& strURL) {
	return R_NOT_IMPLEMENTED;
}

// Will simply update the message loop as needed
RESULT WebRTCImp::Update() {
	return R_NOT_IMPLEMENTED;
}

// WebRTC Specific
/*
RESULT WebRTCImp::QueueUIThreadCallback(int msg_id, void* data) {
	RESULT r = R_PASS;
	
	CB(::PostThreadMessage(m_UIThreadID, UI_THREAD_CALLBACK, static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(data)));

Error:
	return r;
}
*/

// TOOD: peer user id currently not doing anything
RESULT WebRTCImp::SendDataChannelStringMessage(int peerID, std::string& strMessage) {
	RESULT r = R_PASS;

	// TODO: Remove this!
	int pid;

	CN(m_pWebRTCConductor);

	// TODO: this is failing
	//CB(m_pWebRTCClient->IsConnected());

	DEBUG_LINEOUT("WebRTCImp::SendMessageToPeer: Sending %s to peer on data channel", strMessage.c_str());

	//pid = GetFirstPeerID();
	pid = m_pWebRTCConductor->GetPeerConnectionID();

	CR(m_pWebRTCConductor->SendDataChannelStringMessage(strMessage));
	//CR(m_pWebRTCClient->SendMessageToPeer(pid, strMessage));

Error:
	return r;
}

// TODO: This should be a bit more robust
bool WebRTCImp::IsConnected() {
	if (m_pWebRTCConductor == nullptr)
		return false;

	return m_pWebRTCConductor->IsPeerConnectionInitialized();
}

bool WebRTCImp::IsOfferer() {
	return (m_pWebRTCConductor->m_fOffer == true);
}

bool WebRTCImp::IsAnswerer() {
	return (m_pWebRTCConductor->m_fOffer == false);
}

std::list<ICECandidate> WebRTCImp::GetCandidates() {
	return m_pWebRTCConductor->GetCandidates();
}

// TOOD: peer user id currently not doing anything
RESULT WebRTCImp::SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	// TODO: Remove this!
	int pid;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	pid = m_pWebRTCConductor->GetPeerConnectionID();
	CR(m_pWebRTCConductor->SendDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

void WebRTCImp::QueueUIThreadCallback(int msgID, void* data) {
	::PostThreadMessage(m_UIThreadID, UI_THREAD_CALLBACK, static_cast<WPARAM>(msgID), reinterpret_cast<LPARAM>(data));
}

// Functionality
RESULT WebRTCImp::StartLogin(const std::string& strServer, int port) {
	RESULT r = R_PASS;

	CB((m_pWebRTCClient->IsConnected() != true));

	m_strServer = strServer;
	m_pWebRTCClient->Connect(strServer, port, GetPeerName());

Error:
	return r;
}

// TODO: Data channel fucks it up
RESULT WebRTCImp::InitializePeerConnection(bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	CRM(m_pWebRTCConductor->InitializePeerConnection(fAddDataChannel), "Failed to initialize WebRTC Peer Connection");

	if (fCreateOffer) {
		CRM(m_pWebRTCConductor->CreateOffer(), "Failed to create WebRTC Offer");
	}

Error:
	return r;
}

/*
RESULT WebRTCImp::InitializePeerConnection(bool fAddDataChannel) {
	RESULT r = R_PASS;

	CRM(m_pWebRTCConductor->InitializePeerConnection(fAddDataChannel), "Failed to initialize WebRTC Peer Connection");
	CRM(m_pWebRTCConductor->CreateOffer(), "Failed to create WebRTC Offer");

Error:
	return r;
}
*/

int WebRTCImp::GetFirstPeerID() {

	int peerID = -1;
	std::map<int, std::string> peers = m_pWebRTCClient->GetPeers();
	
	if (peers.size() > 0) {
		peerID = peers.begin()->first;
	}

	return peerID;
}

// TODO: this is dead code
RESULT WebRTCImp::AddIceCandidates() {
	//return m_pWebRTCConductor->AddIceCandidates();
	return R_NOT_IMPLEMENTED;
}

// TODO: Make sure we're the answerer
RESULT WebRTCImp::AddOfferCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	for (auto &iceCandidate : pPeerConnection->GetOfferCandidates()) {
		CR(m_pWebRTCConductor->AddIceCandidate(iceCandidate));
	}

Error:
	return r;
}

// TODO: Make sure we're the offerer
RESULT WebRTCImp::AddAnswerCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	for (auto &iceCandidate : pPeerConnection->GetAnswerCandidates()) {
		CR(m_pWebRTCConductor->AddIceCandidate(iceCandidate));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnWebRTCConnectionStable() {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnWebRTCConnectionStable());
	}

Error:
	return r;
}

RESULT WebRTCImp::OnWebRTCConnectionClosed() {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnWebRTCConnectionClosed());
	}

Error:
	return r;
}

RESULT WebRTCImp::OnICECandidatesGatheringDone() {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnICECandidatesGatheringDone());
	}

Error:
	return r;
}

RESULT WebRTCImp::OnDataChannelStringMessage(const std::string& strDataChannelMessage) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnDataChannelStringMessage(strDataChannelMessage));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));
	}

Error:
	return r;
}

// Fill this out and junk
// TODO: REMOVE DEAD CODE
RESULT WebRTCImp::OnSDPOfferSuccess() {
	RESULT r = R_PASS;

	//int peerID = m_pWebRTCConductor->GetPeerConnectionID();
	// TODO: Remove this!
	/*
	int peerID = GetFirstPeerID();
	m_pWebRTCConductor->SetPeerConnectionID(peerID);

	CN(m_pWebRTCConductor);

	CR(m_pWebRTCClient->SendMessageToPeer(peerID, m_pWebRTCConductor->GetSDPJSONString()));
	*/
	//m_pWebRTCConductor->SendMessage(m_pWebRTCConductor->GetSDPJSONString());

	CN(m_pWebRTCObserver);
	CR(m_pWebRTCObserver->OnSDPOfferSuccess());

Error:
	return r;
}

RESULT WebRTCImp::OnSDPAnswerSuccess() {
	RESULT r = R_PASS;

	//int peerID = m_pWebRTCConductor->GetPeerConnectionID();
	// TODO: Remove this!
	/*
	int peerID = GetFirstPeerID();
	m_pWebRTCConductor->SetPeerConnectionID(peerID);

	CN(m_pWebRTCConductor);

	CR(m_pWebRTCClient->SendMessageToPeer(peerID, m_pWebRTCConductor->GetSDPJSONString()));
	*/
	//m_pWebRTCConductor->SendMessage(m_pWebRTCConductor->GetSDPJSONString());

	CN(m_pWebRTCObserver);
	CR(m_pWebRTCObserver->OnSDPAnswerSuccess());

Error:
	return r;
}

// Connect to peer will set up the data channel from the initiator
RESULT WebRTCImp::ConnectToPeer(int peerID) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	CN(m_pWebRTCClient);

	m_pWebRTCConductor->SetPeerConnectionID(peerID);
	CRM(InitializePeerConnection(true), "WebRTCImp: ConnectToPeer failed to Initialzie Peer Connection");

Error:
	return r;
}

std::string WebRTCImp::GetSDPString() {
	if (m_pWebRTCConductor->IsPeerConnectionInitialized()) {
		return m_pWebRTCConductor->GetSDPString();
	}
	else {
		return std::string("");
	}
}

std::string WebRTCImp::GetSDPOfferString() {
	if (m_pWebRTCConductor->IsPeerConnectionInitialized()) {
		return m_pWebRTCConductor->GetSDPJSONString();
	}
	else {
		return std::string("");
	}
}

RESULT WebRTCImp::CreateSDPOfferAnswer(std::string strSDPOffer) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	CR(m_pWebRTCConductor->CreateSDPOfferAnswer(strSDPOffer));

Error:
	return r;
}

RESULT WebRTCImp::SetSDPAnswer(std::string strSDPAnswer) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	CR(m_pWebRTCConductor->SetSDPAnswer(strSDPAnswer));

Error:
	return r;
}

std::function<void(int msg_id, void* data)> WebRTCImp::GetUIThreadCallback() {
	using std::placeholders::_1;
	using std::placeholders::_2;

	std::function<void(int msg_id, void* data)> fnUIThreadCallback = std::bind (&WebRTCConductor::UIThreadCallback, m_pWebRTCConductor, _1, _2);

	return fnUIThreadCallback;
}

// Utilities
std::string WebRTCImp::GetPeerName() {
	char computer_name[256];

	std::string ret(GetEnvVarOrDefault("USERNAME", "user"));
	ret += '@';

	if (gethostname(computer_name, arraysize(computer_name)) == 0) {
		ret += computer_name;
	}
	else {
		ret += "host";
	}

	return ret;
}

std::string WebRTCImp::GetEnvVarOrDefault(const char* env_var_name, const char* default_value) {
	std::string value;
	const char* env_var = getenv(env_var_name);

	if (env_var)
		value = env_var;

	if (value.empty())
		value = default_value;

	return value;
}

RESULT WebRTCImp::OnSignedIn() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnSignedIn");

//Error:
	return r;
}

RESULT WebRTCImp::OnDisconnected() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnDisconnected");

//Error:
	return r;
}

RESULT WebRTCImp::OnPeerConnected(int id, const std::string& name) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp:OnPeerConnected:");

	std::map<int, std::string> peers = m_pWebRTCClient->GetPeers();
	for (auto &peer : peers) {
		DEBUG_LINEOUT("%d: %s", peer.first, peer.second.c_str());
	}

//Error:
	return r;
}

RESULT WebRTCImp::OnPeerDisconnected(int peer_id) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnPeerDisconnected");

//Error:
	return r;
}

RESULT WebRTCImp::OnMessageFromPeer(int peerID, const std::string& strMessage) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnMessageFromPeer");

	CN(m_pWebRTCConductor);

	m_pWebRTCConductor->OnMessageFromPeer(peerID, strMessage);

Error:
	return r;
}

RESULT WebRTCImp::OnMessageSent(int err) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnMessageSent");

//Error:
	return r;
}

RESULT WebRTCImp::OnServerConnectionFailure() {
	DEBUG_LINEOUT("WebRTCImp Error: Failed to connect to %s", m_strServer.c_str());
	return R_PASS;
}
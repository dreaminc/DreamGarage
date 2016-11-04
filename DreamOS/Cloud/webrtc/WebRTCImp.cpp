#include "Logger/Logger.h"
#include "WebRTCImp.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"

#include "WebRTCClient.h"
#include "WebRTCConductor.h"

#include "webrtc/base/arraysize.h"
#include "Cloud/CloudController.h"
#include "Cloud/Environment/PeerConnection.h"

#include "Cloud/User/User.h"
#include "Cloud/User/TwilioNTSInformation.h"

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
		//m_pWebRTCConductor.release();
		m_pWebRTCConductor = nullptr;
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

	//m_pWebRTCConductor = rtc::scoped_refptr<WebRTCConductor>(new rtc::RefCountedObject<WebRTCConductor>(this));
	m_pWebRTCConductor = std::make_shared<WebRTCConductor>(this);

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

User WebRTCImp::GetUser() {
	return m_currentUser;
}

TwilioNTSInformation WebRTCImp::GetTwilioNTSInformation() {
	return m_twilioNTSInformation;
}

RESULT WebRTCImp::SetUser(User currentUser) {
	m_currentUser = currentUser;
	return R_PASS;
}

RESULT WebRTCImp::SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation) {
	m_twilioNTSInformation = twilioNTSInformation;
	return R_PASS;
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

// TODO: This should be a bit more robust
bool WebRTCImp::IsConnected(long peerConnectionID) {
	if (m_pWebRTCConductor != nullptr)
		return m_pWebRTCConductor->IsConnected(peerConnectionID);

	return false;
}

bool WebRTCImp::IsOfferer(long peerConnectionID) {
	if (m_pWebRTCConductor != nullptr) 
		return (m_pWebRTCConductor->IsOfferer(peerConnectionID) == true);

	return false;
}

bool WebRTCImp::IsAnswerer(long peerConnectionID) {
	if (m_pWebRTCConductor != nullptr) 
		return (m_pWebRTCConductor->IsAnswerer(peerConnectionID) == false);

	return false;
}

std::list<WebRTCICECandidate> WebRTCImp::GetCandidates(long peerConnectionID) {
	if (m_pWebRTCConductor != nullptr)
		return m_pWebRTCConductor->GetICECandidates(peerConnectionID);
	
	return std::list<WebRTCICECandidate>();
}

RESULT WebRTCImp::SendDataChannelStringMessageByPeerUserID(long peerUserID, std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	CR(m_pWebRTCConductor->SendDataChannelStringMessageByPeerUserID(peerUserID, strMessage));

Error:
	return r;
}

RESULT WebRTCImp::SendDataChannelMessageByPeerUserID(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);
	DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessageByUserID: Sending %d bytes peer on data channel to user %d", pDataChannelBuffer_n, peerUserID);

	CR(m_pWebRTCConductor->SendDataChannelMessageByPeerUserID(peerUserID, pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT WebRTCImp::SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	CR(m_pWebRTCConductor->SendDataChannelMessage(peerConnectionID, pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT WebRTCImp::SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	DEBUG_LINEOUT("WebRTCImp::SendMessageToPeer: Sending %s to peer on data channel", strMessage.c_str());

	CR(m_pWebRTCConductor->SendDataChannelStringMessage(peerConnectionID, strMessage));

Error:
	return r;
}

// Functionality

// TODO: Data channel fucks it up
RESULT WebRTCImp::InitializeNewPeerConnection(long peerConnectionID, bool fCreateOffer) {
	RESULT r = R_PASS;

	CRM(m_pWebRTCConductor->InitializeNewPeerConnection(peerConnectionID, fCreateOffer, true), "Failed to initialize WebRTC Peer Connection");

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

RESULT WebRTCImp::AddOfferCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	CR(m_pWebRTCConductor->AddOfferCandidates(pPeerConnection));

Error:
	return r;
}

RESULT WebRTCImp::AddAnswerCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	CR(m_pWebRTCConductor->AddAnswerCandidates(pPeerConnection));

Error:
	return r;
}

RESULT WebRTCImp::OnWebRTCConnectionStable(long peerConnectionID) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnWebRTCConnectionStable(peerConnectionID));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnWebRTCConnectionClosed(long peerConnectionID) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnWebRTCConnectionClosed(peerConnectionID));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnICECandidatesGatheringDone(long peerConnectionID) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnICECandidatesGatheringDone(peerConnectionID));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnDataChannelStringMessage(peerConnectionID, strDataChannelMessage));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnDataChannelMessage(peerConnectionID, pDataChannelBuffer, pDataChannelBuffer_n));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnAudioData(long peerConnectionID,
	const void* audio_data,
	int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnAudioData(peerConnectionID, audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames));
	}

Error:
	return r;
}

// Fill this out and junk
// TODO: REMOVE DEAD CODE
RESULT WebRTCImp::OnSDPOfferSuccess(long peerConnectionID) {
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
	CR(m_pWebRTCObserver->OnSDPOfferSuccess(peerConnectionID));

Error:
	return r;
}

RESULT WebRTCImp::OnSDPAnswerSuccess(long peerConnectionID) {
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
	CR(m_pWebRTCObserver->OnSDPAnswerSuccess(peerConnectionID));

Error:
	return r;
}

std::string WebRTCImp::GetLocalSDPString(long peerConnectionID) {
	if (m_pWebRTCConductor->IsPeerConnectionInitialized(peerConnectionID)) {
		return m_pWebRTCConductor->GetLocalSDPString(peerConnectionID);
	}
	
	return std::string("");
}

std::string WebRTCImp::GetRemoteSDPString(long peerConnectionID) {
	if (m_pWebRTCConductor->IsPeerConnectionInitialized(peerConnectionID)) {
		return m_pWebRTCConductor->GetRemoteSDPString(peerConnectionID);
	}

	return std::string("");
}

std::string WebRTCImp::GetLocalSDPJSONString(long peerConnectionID) {
	if (m_pWebRTCConductor->IsPeerConnectionInitialized(peerConnectionID)) {
		return m_pWebRTCConductor->GetLocalSDPJSONString(peerConnectionID);
	}
	
	return std::string("");
}

std::string WebRTCImp::GetRemoteSDPJSONString(long peerConnectionID) {
	if (m_pWebRTCConductor->IsPeerConnectionInitialized(peerConnectionID)) {
		return m_pWebRTCConductor->GetRemoteSDPJSONString(peerConnectionID);
	}

	return std::string("");
}

RESULT WebRTCImp::CreateSDPOfferAnswer(long peerConnectionID, std::string strSDPOffer) {
	RESULT r = R_PASS;
	
	CBM((m_pWebRTCConductor->IsPeerConnectionInitialized(peerConnectionID)), "Peer connection %d not initialized", peerConnectionID);
	CN(m_pWebRTCConductor);

	CR(m_pWebRTCConductor->CreateSDPOfferAnswer(peerConnectionID, strSDPOffer));

Error:
	return r;
}

RESULT WebRTCImp::SetSDPAnswer(long peerConnectionID, std::string strSDPAnswer) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	CR(m_pWebRTCConductor->SetSDPAnswer(peerConnectionID, strSDPAnswer));

Error:
	return r;
}

// Utilities
std::string WebRTCImp::GetEnvVarOrDefault(const char* env_var_name, const char* default_value) {
	std::string value;
	const char* env_var = getenv(env_var_name);

	if (env_var)
		value = env_var;

	if (value.empty())
		value = default_value;

	return value;
}
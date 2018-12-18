#include "WebRTCImp.h"

#include "DreamLogger/DreamLogger.h"

#include "rtc_base/ssladapter.h"
#include "rtc_base/win32socketinit.h"

#include "WebRTCClient.h"
#include "WebRTCConductor.h"

#include "rtc_base/arraysize.h"
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

	//BOOL res = PostThreadMessage(GetThreadId(thread.native_handle()), WM_QUIT, 0, 0);
	//rtc::ThreadManager::Instance()->

	//m_pWin32thread->Stop();
	//m_pWin32thread->Quit();

	rtc::ThreadManager::Instance()->SetCurrentThread(nullptr);
	rtc::CleanupSSL();
}

RESULT WebRTCImp::Shutdown() {
	RESULT r = R_PASS;

	m_pWebRTCConductor->Shutdown();
	m_pWin32thread->Quit();

//Error:
	return r;
}

RESULT WebRTCImp::CloseAllPeerConnections() {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	m_pWebRTCConductor->CloseAllPeerConnections();

Error:
	return r;
}

RESULT WebRTCImp::CloseWebRTCPeerConnection(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	m_pWebRTCConductor->CloseWebRTCPeerConnection(pPeerConnection);

Error:
	return r;
}

// CloudImp Interface
RESULT WebRTCImp::Initialize() {
	RESULT r = R_PASS;

	rtc::EnsureWinsockInit();

	m_pWin32SocketServer = new rtc::Win32SocketServer();
	CN(m_pWin32SocketServer);

	m_pWin32thread = new rtc::Win32Thread(m_pWin32SocketServer);
	CN(m_pWin32thread);

	rtc::ThreadManager::Instance()->SetCurrentThread(m_pWin32thread);

	rtc::InitializeSSL();

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
	auto pUserController = dynamic_cast<UserController*>(GetParentCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
	
	return pUserController->GetTwilioNTSInformation();
}

RESULT WebRTCImp::SetUser(User currentUser) {
	m_currentUser = currentUser;
	return R_PASS;
}

// TODO: remove this path
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

RESULT WebRTCImp::SendVideoFrame(long peerConnectionID, const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	CR(m_pWebRTCConductor->SendVideoFrame(peerConnectionID, strVideoTrackLabel, pVideoFrameBuffer, pxWidth, pxHeight, channels));

Error:
	return r;
}

// Audio
float WebRTCImp::GetRunTimeMicAverage() {
	if (m_pWebRTCConductor != nullptr) {
		return m_pWebRTCConductor->GetRunTimeMicAverage();
	}

	return 0.0f;
}

RESULT WebRTCImp::SendAudioPacket(const std::string &strAudioTrackLabel, long peerConnectionID, const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);
	CR(m_pWebRTCConductor->SendAudioPacket(strAudioTrackLabel, peerConnectionID, pendingAudioPacket));

Error:
	return r;
}

RESULT WebRTCImp::StartVideoStreaming(long peerConnectionID, const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	CR(m_pWebRTCConductor->StartVideoStreaming(peerConnectionID, strVideoTrackLabel, pxDesiredWidth, pxDesiredHeight, desiredFPS, pixelFormat));

Error:
	return r;
}

RESULT WebRTCImp::StopVideoStreaming(long peerConnectionID, const std::string &strVideoTrackLabel) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	CR(m_pWebRTCConductor->StopVideoStreaming(peerConnectionID, strVideoTrackLabel));

Error:
	return r;
}

bool WebRTCImp::IsVideoStreamingRunning(long peerConnectionID, const std::string &strVideoTrackLabel) {
	RESULT r = R_PASS;

	CN(m_pWebRTCConductor);

	//DEBUG_LINEOUT("WebRTCImp::SendDataChannelMessage: Sending %d bytes peer on data channel", pDataChannelBuffer_n);

	return m_pWebRTCConductor->IsVideoStreamingRunning(peerConnectionID, strVideoTrackLabel);

Error:
	return false;
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
RESULT WebRTCImp::InitializeNewPeerConnection(long peerConnectionID, long userID, long peerUserID, bool fCreateOffer) {
	RESULT r = R_PASS;

	CRM(m_pWebRTCConductor->InitializeNewPeerConnection(peerConnectionID, userID, peerUserID, fCreateOffer, true), "Failed to initialize WebRTC Peer Connection");

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

WebRTCPeerConnectionProxy* WebRTCImp::GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection) {
	return m_pWebRTCConductor->GetWebRTCPeerConnectionProxy(pPeerConnection);
}

WebRTCImpProxy* WebRTCImp::GetProxy() {
	return (WebRTCImpProxy*)(this);
}

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

RESULT WebRTCImp::OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnIceConnectionChange(peerConnectionID, webRTCIceConnectionState));
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

RESULT WebRTCImp::OnRenegotiationNeeded(long peerConnectionID) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnRenegotiationNeeded(peerConnectionID));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnDataChannel(long peerConnectionID) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnDataChannel(peerConnectionID));
	}

Error:
	return r;
}


RESULT WebRTCImp::OnAudioChannel(long peerConnectionID) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnAudioChannel(peerConnectionID));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnVideoFrame(const std::string &strVideoTrackLabel, long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnVideoFrame(strVideoTrackLabel, peerConnectionID, pVideoFrameDataBuffer, pxWidth, pxHeight));
	}

Error:
	return r;
}

RESULT WebRTCImp::OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	RESULT r = R_PASS;

	if (m_pWebRTCObserver != nullptr) {
		CR(m_pWebRTCObserver->OnAudioData(strAudioTrackLabel, peerConnectionID, pAudioDataBuffer, bitsPerSample, samplingRate, channels, frames));
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
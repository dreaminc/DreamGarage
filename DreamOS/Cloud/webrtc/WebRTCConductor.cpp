#include "WebRTCConductor.h"

#include "DreamLogger/DreamLogger.h"

#include "WebRTCImp.h"

#include <memory>
#include <utility>
#include <vector>

#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/examples/peerconnection/client/defaults.h"

#include "webrtc/api/test/fakertccertificategenerator.h"
#include "webrtc/p2p/base/fakeportallocator.h"

#include "WebRTCPeerConnection.h"
#include "WebRTCICECandidate.h"
#include "Cloud/Environment/PeerConnection.h"

#include "Cloud/User/User.h"
#include "Cloud//User/TwilioNTSInformation.h"

#include "Sound/AudioPacket.h"

#define WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
#include "webrtc/modules/audio_device/audio_device_impl.h"

#include "Sound/AudioPacket.h"

WebRTCConductor::WebRTCConductor(WebRTCConductorObserver *pParetObserver) :
	m_pParentObserver(pParetObserver),
	m_pWebRTCPeerConnectionFactory(nullptr)
{
	ClearPeerConnections();
}

WebRTCConductor::~WebRTCConductor() {
	if (m_networkThread)
		m_networkThread = nullptr;

	if (m_workerThread)
		m_workerThread = nullptr;
}

RESULT WebRTCConductor::ClearPeerConnections() {
	m_webRTCPeerConnections.clear();
	return R_PASS;
}

rtc::scoped_refptr<WebRTCPeerConnection> WebRTCConductor::AddNewPeerConnection(long peerConnectionID) {
	RESULT r = R_PASS;
	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = nullptr;

	CBM((FindPeerConnectionByID(peerConnectionID) == false), "Peer Conncetion ID %d already present", peerConnectionID);
	
	//pWebRTCPeerConnection = std::make_shared<WebRTCPeerConnection>(this, peerConnectionID, m_pWebRTCPeerConnectionFactory);
	pWebRTCPeerConnection = 
		rtc::scoped_refptr<WebRTCPeerConnection>(
			new rtc::RefCountedObject<WebRTCPeerConnection>(
				this, 
				peerConnectionID, 
				m_pWebRTCPeerConnectionFactory) 
			);
	
	CNM(pWebRTCPeerConnection, "Failed to allocate new peer connection");

	m_webRTCPeerConnections.push_back(pWebRTCPeerConnection);

// Success:
	return pWebRTCPeerConnection;

Error:
	if (pWebRTCPeerConnection != nullptr) {
		pWebRTCPeerConnection = nullptr;
	}

	return nullptr;
}

rtc::scoped_refptr<WebRTCPeerConnection> WebRTCConductor::GetPeerConnection(long peerConnectionID) {
	for (auto &pWebRTCPeerConnection : m_webRTCPeerConnections)
		if (pWebRTCPeerConnection->GetPeerConnectionID() == peerConnectionID)
			return pWebRTCPeerConnection;

	return nullptr;
}

bool WebRTCConductor::FindPeerConnectionByID(long peerConnectionID) {
	if (GetPeerConnection(peerConnectionID) == nullptr) 
		return false;
	else 
		return true;
}

RESULT WebRTCConductor::RemovePeerConnectionByID(long peerConnectionID) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CN(pWebRTCPeerConnection);
	
	{
		auto it = std::find(m_webRTCPeerConnections.begin(), m_webRTCPeerConnections.end(), pWebRTCPeerConnection);
		CB((it != m_webRTCPeerConnections.end()));

		m_webRTCPeerConnections.erase(it);
	}

Error:
	return r;
}

rtc::scoped_refptr<WebRTCPeerConnection> WebRTCConductor::GetPeerConnectionByPeerUserID(long peerUserID) {
	for (auto &pWebRTCPeerConnection : m_webRTCPeerConnections)
		if (pWebRTCPeerConnection->GetPeerUserID() == peerUserID)
			return pWebRTCPeerConnection;

	return nullptr;
}

bool WebRTCConductor::FindPeerConnectionByPeerUserID(long peerUserID) {
	if (GetPeerConnectionByPeerUserID(peerUserID) == nullptr)
		return false;
	else
		return true;
}

bool WebRTCConductor::IsPeerConnectionInitialized(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->IsPeerConnectionInitialized();
	}

	return false;
}

bool WebRTCConductor::IsConnected(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	
	if (pWebRTCPeerConnection != nullptr) {
		// TODO: Switch to a robust connection check
		return pWebRTCPeerConnection->IsPeerConnectionInitialized();
	}

	return false;
}

bool WebRTCConductor::IsOfferer(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->IsOfferer();
	}

	return false;
}

bool WebRTCConductor::IsAnswerer(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->IsAnswerer();
	}

	return false;
}

std::list<WebRTCICECandidate> WebRTCConductor::GetICECandidates(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetICECandidates();
	}

	return std::list<WebRTCICECandidate>();
}

std::string WebRTCConductor::GetLocalSDPString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetLocalSDPString();
	}

	return std::string("");
}

std::string WebRTCConductor::GetRemoteSDPString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetRemoteSDPString();
	}

	return std::string("");
}

std::string WebRTCConductor::GetLocalSDPJSONString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetLocalSDPJSONString();
	}
	
	return std::string("");
}

std::string WebRTCConductor::GetRemoteSDPJSONString(long peerConnectionID) {
	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);

	if (pWebRTCPeerConnection != nullptr) {
		return pWebRTCPeerConnection->GetRemoteSDPJSONString();
	}

	return std::string("");
}

RESULT WebRTCConductor::CreateSDPOfferAnswer(long peerConnectionID, std::string strSDPOffer) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CN(pWebRTCPeerConnection);

	CR(pWebRTCPeerConnection->CreateSDPOfferAnswer(strSDPOffer));

Error:
	return r;
}

RESULT WebRTCConductor::SetSDPAnswer(long peerConnectionID, std::string strSDPAnswer) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CN(pWebRTCPeerConnection);

	CR(pWebRTCPeerConnection->SetSDPAnswer(strSDPAnswer));

Error:
	return r;
}

// TODO: Make sure PeerConnect is the answerer
RESULT WebRTCConductor::AddOfferCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(pPeerConnection->GetPeerConnectionID());

	if (pWebRTCPeerConnection != nullptr) {
		for (auto &iceCandidate : pPeerConnection->GetOfferCandidates()) {
			CR(pWebRTCPeerConnection->AddIceCandidate(iceCandidate));
		}
	}

Error:
	return r;
}

// TODO: Make sure PeerConnect is the offerer
RESULT WebRTCConductor::AddAnswerCandidates(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(pPeerConnection->GetPeerConnectionID());

	if (pWebRTCPeerConnection != nullptr) {
		for (auto &iceCandidate : pPeerConnection->GetAnswerCandidates()) {
			CR(pWebRTCPeerConnection->AddIceCandidate(iceCandidate));
		}
	}

Error:
	return r;
}

WebRTCPeerConnectionProxy* WebRTCConductor::GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection) {
	RESULT r = R_PASS;

	auto pWebRTCPeerConnection = GetPeerConnection(pPeerConnection->GetPeerConnectionID());
	CN(pWebRTCPeerConnection);

	// Get proxy from WebRTCPeerConnection
	return pWebRTCPeerConnection->GetProxy();

Error:
	return nullptr;
}


/*
RESULT WebRTCConductor::PushAudioPacket(const AudioPacket audioPacket) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::AudioDeviceModuleImpl> pAudioDeviceModuleImpl = rtc::scoped_refptr<webrtc::AudioDeviceModuleImpl>(m_pAudioDeviceModule);
	CN(pAudioDeviceModuleImpl);

	pAudioDeviceModuleImpl->GetAudioDeviceBuffer()->_ptrCbAudioTransport
		->PushCaptureData(15, 
			              static_cast<void*>(audioPacket.GetDataBuffer()), 
			              audioPacket.GetBitsPerSample(), 
						  audioPacket.GetSamplingRate(), 
						  audioPacket.GetNumChannels(), 
						  audioPacket.GetNumFrames()
		);

Error:
	return r;
}
*/

RESULT WebRTCConductor::Initialize() {
	RESULT r = R_PASS;

	// User Peer Connection Factory
	/* OLD
	CBM((m_pWebRTCPeerConnectionFactory == nullptr), "Peer Connection Factory already initialized");

	m_pWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory();
	m_pWebRTCPeerConnectionFactory->AddRef();
	
	CNM(m_pWebRTCPeerConnectionFactory.get(), "WebRTC Error Failed to initialize PeerConnectionFactory");
	*/

	///*
	// Chrome Peer Connection Factory (testing)
	CBM((m_pWebRTCPeerConnectionFactory == nullptr), "Peer Connection Factory already initialized");

	// Network Thread
	m_networkThread = rtc::Thread::CreateWithSocketServer();
	CNM(m_networkThread, "failed to start network thread");
	m_networkThread->Start();

	// Worker Thread
	m_workerThread = rtc::Thread::Create();
	CN(m_workerThread);
	m_workerThread->Start();

	/*
	// Custom Audio Device Module
	m_pAudioDeviceModule = 
		m_workerThread->Invoke<rtc::scoped_refptr<webrtc::AudioDeviceModule>>(RTC_FROM_HERE,[&]()
	{
		//return webrtc::AudioDeviceModuleImpl::Create(webrtc::VoEId(1, -1), webrtc::AudioDeviceModule::AudioLayer::kPlatformDefaultAudio);
		return CreateAudioDeviceWithDataCapturer(0, webrtc::AudioDeviceModule::AudioLayer::kPlatformDefaultAudio, this);
	});

	//m_pAudioDeviceModule = webrtc::AudioDeviceModule::Create(15, webrtc::AudioDeviceModule::AudioLayer::kPlatformDefaultAudio);
	while (m_pAudioDeviceModule == nullptr) {
		// wait;
	}

	CN(m_pAudioDeviceModule);

	m_pAudioDeviceModule->SetPlayoutSampleRate(44100);
	m_pAudioDeviceModule->SetRecordingSampleRate(44100);
	m_pAudioDeviceModule->SetStereoRecording(true);
	m_pAudioDeviceModule->SetStereoPlayout(true);

	//m_pAudioDeviceModule->RegisterAudioCallback(this);
	*/

	// Create a dummy module which will not actually capture / playback audio
	// and we will handle the end points manually 
	m_pAudioDeviceDummyModule = m_workerThread->Invoke<rtc::scoped_refptr<webrtc::AudioDeviceModule>>(RTC_FROM_HERE, [&]() {
		return webrtc::AudioDeviceModule::Create(0, webrtc::AudioDeviceModule::kDummyAudio);
	});

	//m_pAudioDeviceModule = webrtc::AudioDeviceModule::Create(15, webrtc::AudioDeviceModule::AudioLayer::kPlatformDefaultAudio);
	while (m_pAudioDeviceDummyModule == nullptr) { /* wait for module to be available, TODO: replace with future? */}
	CN(m_pAudioDeviceDummyModule);

	m_pAudioDeviceDummyModule->SetPlayoutSampleRate(44100);
	m_pAudioDeviceDummyModule->SetRecordingSampleRate(44100);
	m_pAudioDeviceDummyModule->SetStereoRecording(true);
	m_pAudioDeviceDummyModule->SetStereoPlayout(true);

	m_pWebRTCPeerConnectionFactory =
		webrtc::CreatePeerConnectionFactory(m_networkThread.get(),	// network thread
											m_workerThread.get(),	// worker thread
											rtc::ThreadManager::Instance()->WrapCurrentThread(),	// signaling thread
											//m_pAudioDeviceModule,	// TODO: Default ADM
											m_pAudioDeviceDummyModule,		// Dummy ADM
											nullptr,	// Video Encoder Factory
											nullptr		// Audio Encoder Factory
		);

	m_pWebRTCPeerConnectionFactory->AddRef();

	CNM(m_pWebRTCPeerConnectionFactory.get(), "WebRTC Error Failed to initialize PeerConnectionFactory");
	//*/

//Success:
	return r;

Error:
	m_pWebRTCPeerConnectionFactory->Release();

	return r;
}

RESULT WebRTCConductor::InitializeNewPeerConnection(long peerConnectionID, long userID, long peerUserID, bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = AddNewPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Failed to add new peer connection %d", peerConnectionID);

	CR(pWebRTCPeerConnection->SetPeerUserID(peerUserID));
	CR(pWebRTCPeerConnection->SetUserID(userID));

	CRM(pWebRTCPeerConnection->InitializePeerConnection(fAddDataChannel), "Failed to initialize WebRTC Peer Connection");

	if (fCreateOffer) {
		CRM(pWebRTCPeerConnection->CreateOffer(), "Failed to create WebRTC Offer");
	}

Error:
	return r;
}

// WebRTCPeerConnectionObserver Interface
// TODO: implement these and pass back the right vars
RESULT WebRTCConductor::OnWebRTCConnectionStable(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnWebRTCConnectionStable(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnWebRTCConnectionClosed(long peerConnectionID) {
	RESULT r = R_PASS;

	// First give observer message
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnWebRTCConnectionClosed(peerConnectionID);
	}

	DEBUG_LINEOUT("ICE Connection disconnected, remove webrtc peer connection");
	CR(RemovePeerConnectionByID(peerConnectionID));

Error:
	return r;
}

RESULT WebRTCConductor::Shutdown() {
	m_webRTCPeerConnections.clear();
	return R_PASS;
}

RESULT WebRTCConductor::OnSDPOfferSuccess(long peerConnectionID) {		// TODO: Consolidate with below
	DOSLOG(INFO, "[WebRTCConductor] OnSDPOfferSuccess");

	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnSDPOfferSuccess(peerConnectionID);
	}

	return R_NOT_HANDLED;
}


RESULT WebRTCConductor::OnSDPAnswerSuccess(long peerConnectionID) {	// TODO: Consolidate with below
	DOSLOG(INFO, "[WebRTCConductor] OnSDPAnswerSuccess"); 
	
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnSDPAnswerSuccess(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnSDPSuccess(long peerConnectionID, bool fOffer) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SDP Success on peer connection ID %d %s", peerConnectionID, fOffer ? "offerer" : "answerer");

	DOSLOG(INFO, "[WebRTCConductor] SDP Success on peer connection %v : %v", peerConnectionID, (fOffer ? "offerer" : "answerer"));

//Error:
	return r;
}

RESULT WebRTCConductor::OnSDPFailure(long peerConnectionID, bool fOffer) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SDP Failure on peer connection ID %d %s", peerConnectionID, fOffer ? "offerer" : "answerer");

	//Error:
	return r;
}

RESULT WebRTCConductor::OnICECandidatesGatheringDone(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnICECandidatesGatheringDone(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) {
	RESULT r = R_PASS;

	switch (webRTCIceConnectionState) {
		case WebRTCIceConnection::state::DISCONNECTED: {
			// Close the WebRTC connection
			auto pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
			CN(pWebRTCPeerConnection);

			CR(pWebRTCPeerConnection->CloseWebRTCPeerConnection());
		} break;
	}

	if (m_pParentObserver != nullptr) {
		CR(m_pParentObserver->OnIceConnectionChange(peerConnectionID, webRTCIceConnectionState));
	}

Error:
	return r;
}

RESULT WebRTCConductor::OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnDataChannelStringMessage(peerConnectionID, strDataChannelMessage);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnDataChannelMessage(peerConnectionID, pDataChannelBuffer, pDataChannelBuffer_n);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnRenegotiationNeeded(long peerConnectionID) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnRenegotiationNeeded(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnAddStream(long peerConnectionID, rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStream) {
	RESULT r = R_PASS;

	//if (pMediaStream->label() == kAudioLabel) {

	if(pMediaStream->FindAudioTrack(kUserAudioLabel)) {
		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnAudioChannel(peerConnectionID);
		}
	}

	return r;
}

RESULT WebRTCConductor::OnRemoveStream(long peerConnectionID, rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStream) {
	// TODO: 
	//if (m_pParentObserver != nullptr) {
	//	return m_pParentObserver->OnRemoveStream(peerConnectionID);
	//}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnDataChannel(long peerConnectionID, rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannel) {
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnDataChannel(peerConnectionID);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnDataChannelStateChange(long peerConnectionID, rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannel) {
	// TODO: 
	//if (m_pParentObserver != nullptr) {
	//	return m_pParentObserver->OnDataChannelStateChange(peerConnectionID);
	//}

	return R_NOT_HANDLED;
}

User WebRTCConductor::GetUser() {
	return m_pParentObserver->GetUser();
}

TwilioNTSInformation WebRTCConductor::GetTwilioNTSInformation() {
	return m_pParentObserver->GetTwilioNTSInformation();
}

RESULT WebRTCConductor::OnVideoFrame(long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {

	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnVideoFrame(peerConnectionID, pVideoFrameDataBuffer, pxWidth, pxHeight);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
		
	if (m_pParentObserver != nullptr) {
		return m_pParentObserver->OnAudioData(strAudioTrackLabel, peerConnectionID, pAudioDataBuffer, bitsPerSample, samplingRate, channels, frames);
	}

	return R_NOT_HANDLED;
}

RESULT WebRTCConductor::SendDataChannelStringMessageByPeerUserID(long peerUserID, std::string& strMessage) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnectionByPeerUserID(peerUserID);
	CNM(pWebRTCPeerConnection, "Peer Connection for user ID %d not found", peerUserID);

	CR(pWebRTCPeerConnection->SendDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessageByPeerUserID(long peerUserID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnectionByPeerUserID(peerUserID);
	CNM(pWebRTCPeerConnection, "Peer Connection for user ID %d not found", peerUserID);

	CR(pWebRTCPeerConnection->SendDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

// Success:
	return r;

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->SendDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->SendDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT WebRTCConductor::SendAudioPacket(const std::string &strAudioTrackLabel, long peerConnectionID, const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->SendAudioPacket(strAudioTrackLabel, pendingAudioPacket));

Error:
	return r;
}

RESULT WebRTCConductor::SendVideoFrame(long peerConnectionID, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->SendVideoFrame(pVideoFrameBuffer, pxWidth, pxHeight, channels));

Error:
	return r;
}

RESULT WebRTCConductor::StartVideoStreaming(long peerConnectionID, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->StartVideoStreaming(pxDesiredWidth, pxDesiredHeight, desiredFPS, pixelFormat));

Error:
	return r;
}

RESULT WebRTCConductor::StopVideoStreaming(long peerConnectionID) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	CR(pWebRTCPeerConnection->StopVideoStreaming());

Error:
	return r;
}

bool WebRTCConductor::IsVideoStreamingRunning(long peerConnectionID) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCPeerConnection> pWebRTCPeerConnection = GetPeerConnection(peerConnectionID);
	CNM(pWebRTCPeerConnection, "Peer Connection %d not found", peerConnectionID);

	return pWebRTCPeerConnection->IsVideoStreamingRunning();

Error:
	return false;
}
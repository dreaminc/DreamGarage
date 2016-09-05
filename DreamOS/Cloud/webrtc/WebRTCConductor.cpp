#include "WebRTCConductor.h"

#include "WebRTCImp.h"
//#include "WebRTCClient.h"

#include <memory>
#include <utility>
#include <vector>

#include "webrtc/api/test/fakeconstraints.h"
#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/examples/peerconnection/client/defaults.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/video_capture_factory.h"

#include "webrtc/api/test/fakertccertificategenerator.h"
#include "webrtc/p2p/base/fakeportallocator.h"

#include "WebRTCPeerConnection.h"

// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";
const char kSDPName[] = "sdp";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

// TODO: Make this more legitimate
class DummySetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
	static DummySetSessionDescriptionObserver* Create() {
		return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
	}

protected:
	DummySetSessionDescriptionObserver() {
		// empty
	}

	~DummySetSessionDescriptionObserver() {
		// empty
	}

public:
	virtual void OnSuccess() {
		DEBUG_LINEOUT("DummySetSessionDescriptionObserver On Success");
	}

	virtual void OnFailure(const std::string& strError) {
		//LOG(INFO) << __FUNCTION__ << " " << error;
		DEBUG_LINEOUT("DummySetSessionDescriptionObserver On Failure: %s", strError.c_str());
	}
};

WebRTCConductor::WebRTCConductor(WebRTCImp *pParentWebRTCImp) :
	m_pParentWebRTCImp(pParentWebRTCImp),
	//m_pWebRTCClient(pWebRTCClient),
	//m_pWebRTCPeerConnection(nullptr),
	m_pWebRTCPeerConnectionFactory(nullptr),
	m_WebRTCPeerID(-1),
	//m_fLoopback(false),
	m_pDataChannelInterface(nullptr),
	m_fOffer(false),
	m_fSDPSet(false)
{
	if (m_pWebRTCPeerConnectionFactory.get() != nullptr) {
		m_pWebRTCPeerConnectionFactory.release();
	}

	/*
	if (m_pWebRTCPeerConnection.get() != nullptr) {
		m_pWebRTCPeerConnection.release();
	}
	*/

	ClearSessionDescriptionProtocol();
	ClearPeerConnections();
}

RESULT WebRTCConductor::ClearPeerConnections() {
	m_webRTCPeerConnections.clear();
	return R_PASS;
}

RESULT WebRTCConductor::AddNewPeerConnection(long peerConnectionID) {
	RESULT r = R_PASS;

	CBM((FindPeerConnectionByID(peerConnectionID) == false), "Peer Conncetion ID %d already present");
	std::shared_ptr<WebRTCPeerConnection> pWebRTCPeerConnection = std::make_shared<WebRTCPeerConnection>(peerConnectionID, m_pWebRTCPeerConnectionFactory);
	CNM(pWebRTCPeerConnection, "Failed to allocate new peer connection");

	m_webRTCPeerConnections.push_back(pWebRTCPeerConnection);

Error:
	return r;
}

std::shared_ptr<WebRTCPeerConnection> WebRTCConductor::GetPeerConnection(long peerConnectionID) {
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

WebRTCConductor::~WebRTCConductor() {
	if (m_pWebRTCPeerConnectionFactory.get() != nullptr) {
		m_pWebRTCPeerConnectionFactory.release();
	}
}

RESULT WebRTCConductor::Initialize() {
	RESULT r = R_PASS;

	CBM((m_pWebRTCPeerConnectionFactory == nullptr), "Peer Connection Factory already initialized");
	m_pWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory();
	CNM(m_pWebRTCPeerConnectionFactory.get(), "WebRTC Error Failed to initialize PeerConnectionFactory");

//Success:
	return r;

Error:
	if (m_pWebRTCPeerConnectionFactory.get() != nullptr) {
		m_pWebRTCPeerConnectionFactory.release();
	}

	return r;
}

std::string WebRTCConductor::GetSessionDescriptionString() {
	return m_strSessionDescriptionProtocol;
}

RESULT WebRTCConductor::ClearSessionDescriptionProtocol() {
	m_strSessionDescriptionProtocol.clear();
	m_strSessionDescriptionType.clear();
	return R_PASS;
}

void WebRTCConductor::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
	DEBUG_LINEOUT("OnAddStream: %s", stream->label().c_str());

	// TODO:
	// m_pParentWebRTCImp->QueueUIThreadCallback(NEW_STREAM_ADDED, stream.release());
}

void WebRTCConductor::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
	DEBUG_LINEOUT("OnRemoveStream: %s", stream->label().c_str());
	
	// TODO:
	// m_pParentWebRTCImp->QueueUIThreadCallback(STREAM_REMOVED, stream.release());
}

std::list<ICECandidate> WebRTCConductor::GetCandidates() {
	return m_iceCandidates;
}

// TODO: Add callbacks
void WebRTCConductor::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) {
	DEBUG_OUT("WebRTC Connection Signaling Changed: ");

	switch (new_state) {
		case webrtc::PeerConnectionInterface::kStable: {
			DEBUG_LINEOUT("WebRTC Connection Stable");
			m_pParentWebRTCImp->OnWebRTCConnectionStable();
		} break;

		case webrtc::PeerConnectionInterface::kHaveLocalOffer: {
			DEBUG_LINEOUT("WebRTC Connection Has Local Offer");
		} break;

		case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer: {
			DEBUG_LINEOUT("WebRTC Connection Has Local Answer");
		} break;

		case webrtc::PeerConnectionInterface::kHaveRemoteOffer: {
			DEBUG_LINEOUT("WebRTC Connection has remote offer");
		} break;

		case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer: {
			DEBUG_LINEOUT("WebRTC Connection has remote answer");
		} break;

		case webrtc::PeerConnectionInterface::kClosed: {
			DEBUG_LINEOUT("WebRTC Connection closed");
			m_pParentWebRTCImp->OnWebRTCConnectionClosed();
		} break;
	}
}

void WebRTCConductor::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {
	DEBUG_OUT("ICE Connection Change: ");

	switch (new_state) {
		case webrtc::PeerConnectionInterface::kIceConnectionNew:			DEBUG_LINEOUT("ICE Connection New"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionChecking:		DEBUG_LINEOUT("ICE Connection Checking"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionConnected:		DEBUG_LINEOUT("ICE Connection Connected"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionCompleted:		DEBUG_LINEOUT("ICE Connection Completed"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionFailed:			DEBUG_LINEOUT("ICE Connection Failed"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:	DEBUG_LINEOUT("ICE Connection Disconnected"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionClosed:			DEBUG_LINEOUT("ICE Connection Closed"); break;
		case webrtc::PeerConnectionInterface::kIceConnectionMax:			DEBUG_LINEOUT("ICE Connection Max"); break;
	}
}

void WebRTCConductor::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) {
	DEBUG_OUT("ICE Connection Change: ");

	switch (new_state) {
		case webrtc::PeerConnectionInterface::kIceGatheringNew:					DEBUG_LINEOUT("ICE Gathering New"); break;
		case webrtc::PeerConnectionInterface::kIceGatheringGathering:			DEBUG_LINEOUT("ICE Garthering"); break;
		case webrtc::PeerConnectionInterface::kIceGatheringComplete: {
			DEBUG_LINEOUT("ICE Gathering Complete");
			m_pParentWebRTCImp->OnICECandidatesGatheringDone();
		}break;
	}
}

void WebRTCConductor::OnIceConnectionReceivingChange(bool receiving) {
	DEBUG_LINEOUT("ICE Receiving %s", (receiving) ? "true" : "false");
}

void WebRTCConductor::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
	DEBUG_LINEOUT("OnIceCandidate: %s %d", candidate->sdp_mid().c_str(), candidate->sdp_mline_index());

	Json::StyledWriter writer;
	Json::Value jmessage;

	ICECandidate iceCandidate;
	iceCandidate.m_SDPMediateLineIndex = candidate->sdp_mline_index();
	iceCandidate.m_strSDPMediaID = candidate->sdp_mid();
	
	if (!candidate->ToString(&(iceCandidate.m_strSDPCandidate))) {
		LOG(LS_ERROR) << "Failed to serialize candidate";
		return;
	}

	m_iceCandidates.push_back(iceCandidate);
}

// TODO: Should be based on peer connection ID not peer ID
// This is removed (webrtc client code)
/*
RESULT WebRTCConductor::SendMessageToPeer(std::string* strMessage, int peerID) {
	RESULT r = R_PASS;

	// TODO: Add a queue here for messages, this is not correct

	CN(m_pWebRTCPeerConnection);
	CRM(m_pWebRTCClient->SendMessageToPeer(peerID, *strMessage), "Failed to send message to peer");

	// TODO: Disconnect server if failed to connect?

Error:
	return r;
}
*/

/*
// Also removed, not using the UI Callback anymore
void WebRTCConductor::SendMessage(const std::string& strJSONObject) {
	std::string* msg = new std::string(strJSONObject);
	m_pParentWebRTCImp->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, msg);
}
*/

RESULT WebRTCConductor::PrintSDP() {
	DEBUG_LINEOUT("WebRTCConductor: SDP:");
	DEBUG_LINEOUT("%s", m_strSessionDescriptionProtocol.c_str());
	return R_PASS;
}

std::string WebRTCConductor::GetSDPTypeString() {
	return m_strSessionDescriptionType;
}

std::string WebRTCConductor::GetSDPString() {
	return m_strSessionDescriptionProtocol;
}

std::string WebRTCConductor::GetSDPJSONString() {
	Json::StyledWriter JSONWriter;
	Json::Value JSONMessage;

	JSONMessage[kSessionDescriptionTypeName] = m_strSessionDescriptionType;
	JSONMessage[kSessionDescriptionSdpName] = m_strSessionDescriptionProtocol;

	// Append Candidates
	for (auto &iceCandidate : m_iceCandidates) {
		Json::Value JSONIceCandidate;

		JSONIceCandidate[kCandidateSdpName] = iceCandidate.m_strSDPCandidate;
		JSONIceCandidate[kCandidateSdpMidName] = iceCandidate.m_strSDPMediaID;
		JSONIceCandidate[kCandidateSdpMlineIndexName] = iceCandidate.m_SDPMediateLineIndex;

		JSONMessage["candidates"].append(JSONIceCandidate);
	}

	std::string strReturn = JSONWriter.write(JSONMessage);

	return strReturn;
}

/*
// This code removed 
// TODO: This doesn't actually work
void WebRTCConductor::UIThreadCallback(int msgID, void* data) {
	//RESULT r = R_PASS;

	// TODO:

	DEBUG_LINEOUT("WebRTCConductor::UIThreadCallback: msg ID %d", msgID);

//Error:
	return;
}
*/

// OnSuccess called when PeerConnection established 
void WebRTCConductor::OnSuccess(webrtc::SessionDescriptionInterface* sessionDescription) {
	RESULT r = R_PASS;

	m_pWebRTCPeerConnection->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), sessionDescription);
	CR(ClearSessionDescriptionProtocol());

	m_fSDPSet = true;
	
	m_strSessionDescriptionType = sessionDescription->type();
	sessionDescription->ToString(&m_strSessionDescriptionProtocol);

	CR(PrintSDP());

	// TODO: peer ID stuff
	if (m_fOffer) {
		CR(m_pParentWebRTCImp->OnSDPOfferSuccess());
	}
	else {
		CR(m_pParentWebRTCImp->OnSDPAnswerSuccess());
	}

Error:
	return;
}

std::string WebRTCConductor::GetPeerConnectionString() {
	// Issues behind the NAT
	//return WebRTCImp::GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:74.125.196.127:19302");
	//return WebRTCImp::GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
	return WebRTCImp::GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.ekiga.net");
}

void WebRTCConductor::OnFailure(const std::string& error) {
	DEBUG_LINEOUT("WebRTC Error: %s", error.c_str());
}

cricket::VideoCapturer* WebRTCConductor::OpenVideoCaptureDevice() {
	std::vector<std::string> device_names;
	
	{
		std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
		if (!info) {
			return nullptr;
		}

		int num_devices = info->NumberOfDevices();
		for (int i = 0; i < num_devices; ++i) {
			const uint32_t kSize = 256;
			char name[kSize] = { 0 };
			char id[kSize] = { 0 };
			if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
				device_names.push_back(name);
			}
		}
	}

	cricket::WebRtcVideoDeviceCapturerFactory factory;
	cricket::VideoCapturer* capturer = nullptr;

	for (const auto& name : device_names) {
		capturer = factory.Create(cricket::Device(name, 0));
		
		if (capturer) {
			break;
		}
	}

	return capturer;
}

RESULT WebRTCConductor::AddVideoStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::VideoTrackInterface> pVideoTrack = nullptr;

	pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateVideoTrack(kVideoLabel, m_pWebRTCPeerConnectionFactory->CreateVideoSource(OpenVideoCaptureDevice(), nullptr)));

	pMediaStreamInterface->AddTrack(pVideoTrack);

//Error:
	return r;
}

RESULT WebRTCConductor::AddAudioStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::AudioTrackInterface> pAudioTrack = nullptr;

	// Set up constraints
	webrtc::FakeConstraints audioSourceConstraints;

	///*
	audioSourceConstraints.AddMandatory(webrtc::MediaConstraintsInterface::kGoogEchoCancellation, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kExtendedFilterEchoCancellation, true);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kDAEchoCancellation, true);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kAutoGainControl, true);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl, true);
	audioSourceConstraints.AddMandatory(webrtc::MediaConstraintsInterface::kNoiseSuppression, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kHighpassFilter, true);
	//*/

	//audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, true);

	pAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateAudioTrack(kAudioLabel, m_pWebRTCPeerConnectionFactory->CreateAudioSource(&audioSourceConstraints)));

	pMediaStreamInterface->AddTrack(pAudioTrack);

	//pAudioTrack->GetSource()

//Error:
	return r;
}

/*
RESULT WebRTCConductor::AddDataStream() {
	RESULT r = R_PASS;

Error:
	return r;
}
*/

RESULT WebRTCConductor::AddStreams() {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface = nullptr;
	rtc::scoped_refptr<webrtc::AudioTrackInterface> pAudioTrack = nullptr;
	
	CB((m_WebRTCActiveStreams.find(kStreamLabel) == m_WebRTCActiveStreams.end()));

	/*
	pAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateAudioTrack(kAudioLabel, m_pWebRTCPeerConnectionFactory->CreateAudioSource(nullptr)));
		*/
	
	pMediaStreamInterface = m_pWebRTCPeerConnectionFactory->CreateLocalMediaStream(kStreamLabel);

	CR(AddAudioStream(pMediaStreamInterface));
	//CR(AddVideoStream(pMediaStreamInterface));

	// TODO: STREAMMMM
	//main_wnd_->StartLocalRenderer(video_track);


	// Add streams
	if (!m_pWebRTCPeerConnection->AddStream(pMediaStreamInterface)) {
		LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
	}
	
	typedef std::pair<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> MediaStreamPair;
	m_WebRTCActiveStreams.insert(MediaStreamPair(pMediaStreamInterface->label(), pMediaStreamInterface));
	
	//main_wnd_->SwitchToStreamingUI();

Error:
	return r;
}

RESULT WebRTCConductor::AddDataChannel() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCConductor::AddDataChannel");

	//rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannelInterface = nullptr;
	webrtc::DataChannelInit dataChannelInit;

	CB((m_WebRTCActiveDataChannels.find(kDataLabel) == m_WebRTCActiveDataChannels.end()));

	m_pDataChannelInterface = m_pWebRTCPeerConnection->CreateDataChannel(kDataLabel, &dataChannelInit);
	CN(m_pDataChannelInterface);

	typedef std::pair<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface>> DataChannelPair;
	m_WebRTCActiveDataChannels.insert(DataChannelPair(m_pDataChannelInterface->label(), m_pDataChannelInterface));

Error:
	return r;
}

void WebRTCConductor::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
	DEBUG_LINEOUT("OnDataChannel: %s", channel->label().c_str());

	//channel->Send(webrtc::DataBuffer("DEADBEEF"));

	// Register self as observer 
	//rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannelInterface = m_WebRTCActiveDataChannels[kDataLabel];
	//pDataChannelInterface->RegisterObserver(this);

	// TODO: 
	// m_pParentWebRTCImp->QueueUIThreadCallback(NEW_DATA, stream.release());

	channel->RegisterObserver(this);
}

RESULT WebRTCConductor::SendDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	//m_SignalOnDataChannel

	auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);
	CN(m_pDataChannelInterface);

	//CB(pWebRTCDataChannel->Send(webrtc::DataBuffer(strMessage)));
	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(strMessage)));

Error:
	return r;
}

RESULT WebRTCConductor::SendDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	CN(m_pDataChannelInterface);

	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(rtc::CopyOnWriteBuffer(pDataChannelBuffer, pDataChannelBuffer_n), true)));

Error:
	return r;
}

std::string GetDataStateString(webrtc::DataChannelInterface::DataState state) {
	switch (state) {
		case webrtc::DataChannelInterface::DataState::kConnecting: return std::string("connecting"); break;
		case webrtc::DataChannelInterface::DataState::kOpen: return std::string("open"); break;
		case webrtc::DataChannelInterface::DataState::kClosing: return std::string("closing"); break;
		case webrtc::DataChannelInterface::DataState::kClosed: return std::string("closed"); break;
		default:  return std::string("invalid state"); break;
	}
}

// DataChannelObserver Implementation
void WebRTCConductor::OnStateChange() {
	RESULT r = R_PASS;

	auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);
	CN(m_pDataChannelInterface);

	//DEBUG_LINEOUT("WebRTCConductor::OnStateChange %d", pWebRTCDataChannel->state());
	DEBUG_LINEOUT("WebRTCConductor::OnStateChange %s", GetDataStateString(m_pDataChannelInterface->state()).c_str());

Error:
	return;
}

// Data Channel OnMessage
void WebRTCConductor::OnMessage(const webrtc::DataBuffer& buffer) {
	RESULT r = R_PASS;
	
	if (buffer.binary) {
		//DEBUG_LINEOUT("WebRTCConductor::OnMessage (Binary Databuffer %d bytes)", (int)buffer.size());

		int pDataBuffer_n = (int)(buffer.size());
		uint8_t *pDataBuffer = new uint8_t[pDataBuffer_n];
		memset(pDataBuffer, 0, sizeof(char) * pDataBuffer_n);
		memcpy(pDataBuffer, buffer.data.data<uint8_t>(), buffer.size());

		CR(m_pParentWebRTCImp->OnDataChannelMessage(pDataBuffer, pDataBuffer_n));
	}
	else {
		//std::string strData = std::string(buffer.data.data<char>());
		int pszBufferString_n = (int)(buffer.size()) + 1;
		char *pszBufferString = new char[pszBufferString_n];
		memset(pszBufferString, 0, sizeof(char) * pszBufferString_n);
		memcpy(pszBufferString, buffer.data.data<char>(), buffer.size());

		std::string strData = std::string(pszBufferString);

		//DEBUG_LINEOUT("WebRTCConductor::OnMessage: %s (String Databuffer)", strData.c_str());
		CR(m_pParentWebRTCImp->OnDataChannelStringMessage(strData));
	}

	//auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);

Error:
	return;
}

RESULT WebRTCConductor::CreateOffer(){
	RESULT r = R_PASS;

	CN(m_pWebRTCPeerConnection);
	
	m_fOffer = true;
	m_pWebRTCPeerConnection->CreateOffer(this, NULL);

Error:
	return r;
}

// TODO: This is not ideal, should be replaced with more robust flag
bool WebRTCConductor::IsPeerConnectionInitialized() {
	if (m_pWebRTCPeerConnection.get() == nullptr)
		return false;
	else
		return true;
}

// TODO: Support many peer connections
RESULT WebRTCConductor::InitializePeerConnection(bool fAddDataChannel) {
	RESULT r = R_PASS;

	//CB((m_pWebRTCPeerConnectionFactory.get() == nullptr));	// ensure peer connection factory uninitialized
	
	CN(m_pWebRTCPeerConnectionFactory);	// ensure peer connection initialized
	CB((m_pWebRTCPeerConnection.get() == nullptr));			// ensure peer connection uninitialized

	/*
	m_pWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory();

	if (!m_pWebRTCPeerConnectionFactory.get()) {
		DEBUG_LINEOUT("WebRTC Error Failed to initialize PeerConnectionFactory");
		DeletePeerConnection();
		return R_FAIL;
	}
	*/

	//if (!CreatePeerConnection(DTLS_OFF)) {
	if (!CreatePeerConnection(DTLS_ON)) {
		DEBUG_LINEOUT("Error CreatePeerConnection failed");
		DeletePeerConnection();
	}
	
	CN(m_pWebRTCPeerConnection.get());

#ifndef WEBRTC_NO_CANDIDATES
	CR(AddStreams());

	if (fAddDataChannel) {
		CR(AddDataChannel());
	}
#endif

Error:
	return r;
}

/*
// This code shouldn't be used
RESULT WebRTCConductor::ReinitializePeerConnectionForLoopback() {
	RESULT r = R_PASS;

	m_fLoopback = true;

	rtc::scoped_refptr<webrtc::StreamCollectionInterface> streams(m_pWebRTCPeerConnection->local_streams());
	m_pWebRTCPeerConnection = nullptr;

	if (CreatePeerConnection(DTLS_OFF)) {
		for (size_t i = 0; i < streams->count(); ++i) {
			m_pWebRTCPeerConnection->AddStream(streams->at(i));
		}
		m_pWebRTCPeerConnection->CreateOffer(this, NULL);
	}

	CN(m_pWebRTCPeerConnection.get());

Error:
	return r;
}
*/

RESULT WebRTCConductor::CreatePeerConnection(bool dtls) {
	RESULT r = R_PASS;

	webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;

	webrtc::PeerConnectionInterface::IceServer iceServer;
	webrtc::FakeConstraints webrtcConstraints;
	std::unique_ptr<rtc::RTCCertificateGeneratorInterface> pCertificateGenerator = nullptr;

	CN(m_pWebRTCPeerConnectionFactory.get());		// ensure factory is valid
	CB((m_pWebRTCPeerConnection.get() == nullptr));	// ensure peer connection is nullptr

	iceServer.uri = GetPeerConnectionString();
	rtcConfiguration.servers.push_back(iceServer);

	if (dtls) {
		if (rtc::SSLStreamAdapter::HaveDtlsSrtp()) {
			pCertificateGenerator = std::unique_ptr<rtc::RTCCertificateGeneratorInterface>(new FakeRTCCertificateGenerator());
		}

		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");

		m_pWebRTCPeerConnection = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, &webrtcConstraints, NULL, std::move(pCertificateGenerator), this);
	}
	else {
		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "false");
		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, "true");
		
		m_pWebRTCPeerConnection = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, &webrtcConstraints, NULL, NULL, this);
	}

	CNM(m_pWebRTCPeerConnection.get(), "WebRTC Peer Connection failed to initialize");

Error:
	return r;
}

RESULT WebRTCConductor::DeletePeerConnection() {
	RESULT r = R_PASS;

	m_pWebRTCPeerConnection = NULL;
	m_WebRTCActiveStreams.clear();
	
	//main_wnd_->StopLocalRenderer();
	//main_wnd_->StopRemoteRenderer();
	
	m_pWebRTCPeerConnectionFactory = NULL;

	m_WebRTCPeerID = -1;
	//m_fLoopback = false;

//Error:
	return r;
}

// PeerConnectionClientObserver implementation.
void WebRTCConductor::OnSignedIn() {
	LOG(INFO) << __FUNCTION__;

	//m_pParentWebRTCImp->SwitchToPeerList(client_->peers());

	// TODO: Do stuff
}

void WebRTCConductor::OnDisconnected() {
	RESULT r = R_PASS;

	LOG(INFO) << __FUNCTION__;

	CR(DeletePeerConnection());

	/*
	if (m_pParentWebRTCImp->->IsWindow())
		m_pParentWebRTCImp->->SwitchToConnectUI();
	*/

	// TODO: Do stuff

Error:
	return;
}

void WebRTCConductor::OnPeerConnected(int id, const std::string& name) {
	LOG(INFO) << __FUNCTION__;

	DEBUG_LINEOUT("WebRTCConductor:OnPeerConnected:");
	
	std::map<int, std::string> peers = m_pWebRTCClient->GetPeers();
	for (auto &peer : peers) {
		DEBUG_LINEOUT("%d: %s", peer.first, peer.second.c_str());
	}

	// Refresh the list if we're showing it.
	/*if (m_pParentWebRTCImp->current_ui() == MainWindow::LIST_PEERS)
		m_pParentWebRTCImp->SwitchToPeerList(client_->peers());
	*/

	// TODO: Handle peer connected
}

void WebRTCConductor::OnPeerDisconnected(int id) {
	LOG(INFO) << __FUNCTION__;
	if (id == m_WebRTCPeerID) {
		LOG(INFO) << "Our peer disconnected";
		m_pParentWebRTCImp->QueueUIThreadCallback(PEER_CONNECTION_CLOSED, NULL);
	}
	else {
		// Refresh the list if we're showing it.
		/*
		if (m_pParentWebRTCImp->->current_ui() == MainWindow::LIST_PEERS)
			m_pParentWebRTCImp->->SwitchToPeerList(client_->peers());
		*/
		// TODO: Handle a peer has disconnected
	}
}

void WebRTCConductor::OnMessageFromPeer(int peerID, const std::string& strMessage) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCConductor:OnMessageFromPeer: %d: %s", peerID, strMessage.c_str());
	
	CBM(((m_WebRTCPeerID == peerID) || (m_WebRTCPeerID == -1)), "PeerID mismatch %d not %d", peerID, m_WebRTCPeerID);
	CBM((!strMessage.empty()), "Message cannot be empty for OnMessageOnPeer");
	
	{
		if (!m_pWebRTCPeerConnection.get()) {
			CB((m_WebRTCPeerID == -1));
			m_WebRTCPeerID = peerID;

			if (!InitializePeerConnection()) {

				LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
				m_pWebRTCClient->SignOut();

				return;
			}
		}
		else if (peerID != m_WebRTCPeerID) {
			CB((m_WebRTCPeerID != -1));
			LOG(WARNING) << "Received a message from unknown peer while already in a conversation with a different peer.";
			return;
		}

		Json::Reader reader;
		Json::Value jmessage;

		if (!reader.parse(strMessage, jmessage)) {
			LOG(WARNING) << "Received unknown message. " << strMessage;
			return;
		}

		std::string type;
		std::string json_object;

		rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName, &type);

		if (!type.empty()) {
			if (type == "offer-loopback") {
				// This is a loopback call.
				// Recreate the peerconnection with DTLS disabled.
				if (!ReinitializePeerConnectionForLoopback()) {
					LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
					DeletePeerConnection();
					m_pWebRTCClient->SignOut();
				}
				return;
			}

			std::string strSDP;

			if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName, &strSDP)) {
				LOG(WARNING) << "Can't parse received session description message.";
				return;
			}

			webrtc::SdpParseError error;
			webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription(type, strSDP, &error));

			if (!session_description) {
				LOG(WARNING) << "Can't parse received session description message. SdpParseError was: " << error.description;
				return;
			}

			LOG(INFO) << " Received session description :" << strMessage;
			m_pWebRTCPeerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), session_description);

			if (session_description->type() == webrtc::SessionDescriptionInterface::kOffer) {
				m_pWebRTCPeerConnection->CreateAnswer(this, NULL);
			}

			return;
		}
		else {
			std::string sdp_mid;
			int sdp_mlineindex = 0;
			std::string sdp;

			if (!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpMidName, &sdp_mid) ||
				!rtc::GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName, &sdp_mlineindex) ||
				!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp))
			{
				LOG(WARNING) << "Can't parse received message.";
				return;
			}

			webrtc::SdpParseError error;
			std::unique_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));

			if (!candidate.get()) {
				LOG(WARNING) << "Can't parse received candidate message. SdpParseError was: " << error.description;
				return;
			}

			if (!m_pWebRTCPeerConnection->AddIceCandidate(candidate.get())) {
				LOG(WARNING) << "Failed to apply the received candidate";
				return;
			}

			LOG(INFO) << " Received candidate :" << strMessage;
			return;
		}
	}

Error:
	return;
}

//std::list<ICECandidate> g_peerICECandidates;

RESULT WebRTCConductor::AddIceCandidate(ICECandidate iceCandidate) {
	RESULT r = R_PASS;

	//for (auto &peerICECandidate : g_peerICECandidates) {
		webrtc::SdpParseError sdpError;
		//std::unique_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(strSDPMID, sdpMLineIndex, strSDP, &sdpError));

		std::unique_ptr<webrtc::IceCandidateInterface> candidate(
			webrtc::CreateIceCandidate(iceCandidate.m_strSDPMediaID, iceCandidate.m_SDPMediateLineIndex,
									   iceCandidate.m_strSDPCandidate, &sdpError));

		CBM((candidate.get()), "Can't parse received candidate message. SdpParseError was: %s", sdpError.description.c_str());
		CBM((m_pWebRTCPeerConnection->AddIceCandidate(candidate.get())), "Failed to apply the received candidate");

		DEBUG_LINEOUT(" Received candidate : %s", iceCandidate.m_strSDPCandidate.c_str());
	//}

Error:
	return r;
}

RESULT WebRTCConductor::SetSDPAnswer(std::string strSDPAnswer) {
	RESULT r = R_PASS;

	webrtc::SdpParseError sdpError;

	// TODO: Make this more generic (string)
	webrtc::SessionDescriptionInterface* sessionDescriptionInterface(webrtc::CreateSessionDescription("answer", strSDPAnswer, &sdpError));
	std::string strSDPType = sessionDescriptionInterface->type();

	CNM((sessionDescriptionInterface),
		"Can't parse received session description message. SdpParseError was: %s", sdpError.description.c_str());

	DEBUG_LINEOUT(" Received %s session description: %s", strSDPType.c_str(), strSDPAnswer.c_str());

	m_pWebRTCPeerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), sessionDescriptionInterface);

Error:
	return r;
}

RESULT WebRTCConductor::CreateSDPOfferAnswer(std::string strSDPOffer) {
	RESULT r = R_PASS;

	m_fOffer = false;
	
	/* TODO: Handle robust / bounce back
	if (strType == "offer-loopback") {
		// This is a loopback call.
		// Recreate the peer connection with DTLS disabled.
		if (!ReinitializePeerConnectionForLoopback()) {
			DEBUG_LINEOUT("Failed to initialize our PeerConnection instance");
			DeletePeerConnection();
			m_pWebRTCClient->SignOut();
		}
	}
	*/

	webrtc::SdpParseError sdpError;
	webrtc::SessionDescriptionInterface* sessionDescriptionInterface(webrtc::CreateSessionDescription("offer", strSDPOffer, &sdpError));
	std::string strSDPType;

	CNM((sessionDescriptionInterface),
		"Can't parse received session description message. SdpParseError was: %s", sdpError.description.c_str());

	strSDPType = sessionDescriptionInterface->type();

	DEBUG_LINEOUT(" Received %s session description: %s", strSDPType.c_str(), strSDPOffer.c_str());

	m_pWebRTCPeerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), sessionDescriptionInterface);

	//if (sessionDescriptionInterface->type() == webrtc::SessionDescriptionInterface::kOffer) {
	//if(strSDPType == webrtc::SessionDescriptionInterface::kOffer) {
	if(strSDPType == "offer") {
		m_pWebRTCPeerConnection->CreateAnswer(this, NULL);
	}
	
	// Saves the candidates
	/*  TODO: Move to candidates and handle them
	if (jsonMessage["candidates"].isArray() && jsonMessage["candidates"].size() > 0) {
		for (auto &jsonCandidate : jsonMessage["candidates"]) {
			std::string strSDPMID;
			int sdpMLineIndex = 0;
			std::string strSDPCandidate;

			ICECandidate peerICECandidate;

			CBM((rtc::GetStringFromJsonObject(jsonCandidate, kCandidateSdpMidName, &(peerICECandidate.m_strSDPMediaID))), "Failed to parse message");
			CBM((rtc::GetIntFromJsonObject(jsonCandidate, kCandidateSdpMlineIndexName, &(peerICECandidate.m_SDPMediateLineIndex))), "Failed to parse message");
			CBM((rtc::GetStringFromJsonObject(jsonCandidate, kCandidateSdpName, &(peerICECandidate.m_strSDPCandidate))), "Failed to parse message");

			g_peerICECandidates.push_back(peerICECandidate);
		}
	}
	AddIceCandidates();
	*/

Error:
	return r;
}

void WebRTCConductor::OnMessageSent(int err) {
	// Process the next pending message if any.
	m_pParentWebRTCImp->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, NULL);
}

void WebRTCConductor::OnServerConnectionFailure() {
	DEBUG_LINEOUT("WebRTC Error Failed to connect to %s", m_strWebRTCServer.c_str());
}
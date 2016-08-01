#include "WebRTCConductor.h"

#include "WebRTCImp.h"
#include "WebRTCClient.h"

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

// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

class DummySetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
	static DummySetSessionDescriptionObserver* Create() {
		return
			new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
	}

	virtual void OnSuccess() {
		LOG(INFO) << __FUNCTION__;
	}

	virtual void OnFailure(const std::string& error) {
		LOG(INFO) << __FUNCTION__ << " " << error;
	}

protected:
	DummySetSessionDescriptionObserver() {}
	~DummySetSessionDescriptionObserver() {}
};

WebRTCConductor::WebRTCConductor(WebRTCClient *pWebRTCClient, WebRTCImp *pParentWebRTCImp) :
	m_pParentWebRTCImp(pParentWebRTCImp),
	m_pWebRTCClient(pWebRTCClient),
	m_WebRTCPeerID(-1),
	m_fLoopback(false)
{
	// TODO
}

void WebRTCConductor::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
	DEBUG_LINEOUT("OnAddStream: %s", stream->label().c_str());

	m_pParentWebRTCImp->QueueUIThreadCallback(NEW_STREAM_ADDED, stream.release());
}

void WebRTCConductor::OnRemoveStream(
	rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
	DEBUG_LINEOUT("OnRemoveStream: %s", stream->label().c_str());
	
	m_pParentWebRTCImp->QueueUIThreadCallback(STREAM_REMOVED, stream.release());
}

void WebRTCConductor::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
	DEBUG_LINEOUT("OnIceCandidate: %d", candidate->sdp_mline_index());
	
	// For loopback test. To save some connecting delay.
	if (m_fLoopback) {
		if (!m_pWebRTCPeerConnection->AddIceCandidate(candidate)) {
			DEBUG_LINEOUT("Failed to apply the received candidate");
		}
		return;
	}

	Json::StyledWriter writer;
	Json::Value jmessage;

	jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
	jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
	std::string sdp;
	if (!candidate->ToString(&sdp)) {
		LOG(LS_ERROR) << "Failed to serialize candidate";
		return;
	}
	jmessage[kCandidateSdpName] = sdp;
	SendMessage(writer.write(jmessage));
}

void WebRTCConductor::SendMessage(const std::string& json_object) {
	std::string* msg = new std::string(json_object);

	// TODO:
	m_pParentWebRTCImp->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, msg);
}

void WebRTCConductor::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
	m_pWebRTCPeerConnection->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);

	std::string sdp;
	desc->ToString(&sdp);

	// For loopback test. To save some connecting delay.
	if (m_fLoopback) {
		// Replace message type from "offer" to "answer"
		webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription("answer", sdp, nullptr));
		m_pWebRTCPeerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), session_description);

		return;
	}

	Json::StyledWriter writer;
	Json::Value jmessage;

	jmessage[kSessionDescriptionTypeName] = desc->type();
	jmessage[kSessionDescriptionSdpName] = sdp;

	SendMessage(writer.write(jmessage));
}

std::string WebRTCConductor::GetPeerConnectionString() {
	return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
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

RESULT WebRTCConductor::AddStreams() {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::MediaStreamInterface> pStream = nullptr;
	rtc::scoped_refptr<webrtc::AudioTrackInterface> pAudioTrack = nullptr;
	rtc::scoped_refptr<webrtc::VideoTrackInterface> pVideoTrack = nullptr;

	CB((m_WebRTCActiveStreams.find(kStreamLabel) == m_WebRTCActiveStreams.end()));

	pAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateAudioTrack(kAudioLabel, m_pWebRTCPeerConnectionFactory->CreateAudioSource(nullptr)));

	pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateVideoTrack( kVideoLabel, 
			m_pWebRTCPeerConnectionFactory->CreateVideoSource(OpenVideoCaptureDevice(), nullptr)));

	// TODO: STREAMMMM
	//main_wnd_->StartLocalRenderer(video_track);

	pStream = m_pWebRTCPeerConnectionFactory->CreateLocalMediaStream(kStreamLabel);

	pStream->AddTrack(pAudioTrack);
	pStream->AddTrack(pVideoTrack);

	if (!m_pWebRTCPeerConnection->AddStream(pStream)) {
		LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
	}

	typedef std::pair<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> MediaStreamPair;
	m_WebRTCActiveStreams.insert(MediaStreamPair(pStream->label(), pStream));
	
	//main_wnd_->SwitchToStreamingUI();

Error:
	return r;
}

RESULT WebRTCConductor::InitializePeerConnection() {
	RESULT r = R_PASS;

	CB((m_pWebRTCPeerConnectionFactory.get() == nullptr));	// ensure peer connection factory uninitialized
	CB((m_pWebRTCPeerConnection.get() == nullptr));			// ensure peer connection uninitialized

	m_pWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory();

	if (!m_pWebRTCPeerConnectionFactory.get()) {
		DEBUG_LINEOUT("WebRTC Error Failed to initialize PeerConnectionFactory");
		DeletePeerConnection();
		return R_FAIL;
	}

	if (!CreatePeerConnection(DTLS_ON)) {
		DEBUG_LINEOUT("Error CreatePeerConnection failed");
		DeletePeerConnection();
	}
	
	CR(AddStreams());
	
	CN(m_pWebRTCPeerConnection.get());

Error:
	return r;
}

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

RESULT WebRTCConductor::CreatePeerConnection(bool dtls) {
	RESULT r = R_PASS;

	webrtc::PeerConnectionInterface::RTCConfiguration config;
	webrtc::PeerConnectionInterface::IceServer server;
	webrtc::FakeConstraints constraints;

	CN(m_pWebRTCPeerConnectionFactory.get());		// ensure factory is valid
	CB((m_pWebRTCPeerConnection.get() == nullptr));	// ensure peer connection is nullptr

	server.uri = GetPeerConnectionString();
	config.servers.push_back(server);

	if (dtls) {
		constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
	}
	else {
		constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "false");
	}

	m_pWebRTCPeerConnection = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(config, &constraints, NULL, NULL, this);

	CN(m_pWebRTCPeerConnection.get());

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
	m_fLoopback = false;

Error:
	return r;
}

// PeerConnectionClientObserver implementation.
void WebRTCConductor::OnSignedIn() {
	LOG(INFO) << __FUNCTION__;

	//m_pParentWebRTCImp->SwitchToPeerList(client_->peers());

	// TODO: Do stuff
}

void WebRTCConductor::OnDisconnected() {
	LOG(INFO) << __FUNCTION__;

	DeletePeerConnection();

	/*
	if (m_pParentWebRTCImp->->IsWindow())
		m_pParentWebRTCImp->->SwitchToConnectUI();
	*/

	// TODO: Do stuff
}

void WebRTCConductor::OnPeerConnected(int id, const std::string& name) {
	LOG(INFO) << __FUNCTION__;

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

void WebRTCConductor::OnMessageFromPeer(int peerID, const std::string& message) {
	ASSERT(m_WebRTCPeerID == peerID || m_WebRTCPeerID == -1);
	ASSERT(!message.empty());

	if (!m_pWebRTCPeerConnection.get()) {
		ASSERT(m_WebRTCPeerID == -1);
		m_WebRTCPeerID = peerID;

		if (!InitializePeerConnection()) {
			
			LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
			m_pWebRTCClient->SignOut();
			
			return;
		}
	}
	else if (peerID != m_WebRTCPeerID) {
		ASSERT(m_WebRTCPeerID != -1);
		LOG(WARNING) << "Received a message from unknown peer while already in a "
			"conversation with a different peer.";
		return;
	}

	Json::Reader reader;
	Json::Value jmessage;
	if (!reader.parse(message, jmessage)) {
		LOG(WARNING) << "Received unknown message. " << message;
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

		std::string sdp;
		
		if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName,
			&sdp)) {
			LOG(WARNING) << "Can't parse received session description message.";
			return;
		}
		
		webrtc::SdpParseError error;
		webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription(type, sdp, &error));

		if (!session_description) {
			LOG(WARNING) << "Can't parse received session description message. "
				<< "SdpParseError was: " << error.description;
			return;
		}
		LOG(INFO) << " Received session description :" << message;
		m_pWebRTCPeerConnection->SetRemoteDescription(
			DummySetSessionDescriptionObserver::Create(), session_description);
		if (session_description->type() ==
			webrtc::SessionDescriptionInterface::kOffer) {
			m_pWebRTCPeerConnection->CreateAnswer(this, NULL);
		}
		return;
	}
	else {
		std::string sdp_mid;
		int sdp_mlineindex = 0;
		std::string sdp;
		
		if (!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpMidName, &sdp_mid)				||
			!rtc::GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName, &sdp_mlineindex)	||
			!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp)) 
		{
			LOG(WARNING) << "Can't parse received message.";
			return;
		}
		
		webrtc::SdpParseError error;
		std::unique_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));

		if (!candidate.get()) {
			LOG(WARNING) << "Can't parse received candidate message. "
				<< "SdpParseError was: " << error.description;
			return;
		}

		if (!m_pWebRTCPeerConnection->AddIceCandidate(candidate.get())) {
			LOG(WARNING) << "Failed to apply the received candidate";
			return;
		}

		LOG(INFO) << " Received candidate :" << message;
		return;
	}
}

void WebRTCConductor::OnMessageSent(int err) {
	// Process the next pending message if any.
	m_pParentWebRTCImp->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, NULL);
}

void WebRTCConductor::OnServerConnectionFailure() {
	DEBUG_LINEOUT("WebRTC Error Failed to connect to %s", m_strWebRTCServer.c_str());
}
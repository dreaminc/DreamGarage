#include "WebRTCPeerConnection.h"

#include "Logger/Logger.h"
#include "easylogging++.h"

#include "WebRTCConductor.h"

#include "WebRTCCommon.h"

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

#include "Cloud/User/TwilioNTSInformation.h"

#include "Core/Utilities.h"

// TODO: Make this more legitimate + put in different file
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

WebRTCPeerConnection::WebRTCPeerConnection(WebRTCPeerConnectionObserver *pParentObserver, long peerConnectionID, rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory) :
	m_pParentObserver(pParentObserver),
	m_peerConnectionID(peerConnectionID),
	m_pWebRTCPeerConnectionFactory(nullptr),
	m_pWebRTCPeerConnectionInterface(nullptr),
	m_WebRTCPeerID(-1)
{
	if (pWebRTCPeerConnectionFactory != nullptr) {
		pWebRTCPeerConnectionFactory->AddRef();
		m_pWebRTCPeerConnectionFactory = pWebRTCPeerConnectionFactory;
	}
}

WebRTCPeerConnection::~WebRTCPeerConnection(){	
	// the following ref counters are wrong.
	// this is a way to solve the process being hand after shutdown
	// but this will work ok only in Release.
	// to solve this issue completely we need to go over all webrtc code
	// and be more precise about dealing with ref count, for example making sure we AddRef when we set pointers to others.
	m_pWebRTCPeerConnectionInterface->AddRef();
	m_pDataChannelInterface->AddRef();

	ClearSessionDescriptionProtocols();

	m_webRTCICECandidates.clear();

	m_WebRTCPeerID = -1;

	CloseWebRTCPeerConnection();
}

RESULT WebRTCPeerConnection::SetPeerConnectionFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory) {
	m_pWebRTCPeerConnectionFactory = pWebRTCPeerConnectionFactory;
	return R_PASS;
}

RESULT WebRTCPeerConnection::AddStreams() {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface = nullptr;
	//rtc::scoped_refptr<webrtc::AudioTrackInterface> pAudioTrack = nullptr;
	//rtc::scoped_refptr<webrtc::VideoTrackInterface> pVideoTrack = nullptr;

	CB((m_WebRTCLocalActiveStreams.find(kStreamLabel) == m_WebRTCLocalActiveStreams.end()));

	/*
	pAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
	m_pWebRTCPeerConnectionFactory->CreateAudioTrack(kAudioLabel, m_pWebRTCPeerConnectionFactory->CreateAudioSource(nullptr)));
	*/

	pMediaStreamInterface = m_pWebRTCPeerConnectionFactory->CreateLocalMediaStream(kStreamLabel);

	//CR(AddAudioStream(pMediaStreamInterface));
	CR(AddVideoStream(pMediaStreamInterface));

	// Add streams
	if (!m_pWebRTCPeerConnectionInterface->AddStream(pMediaStreamInterface)) {
		DEBUG_LINEOUT("Adding stream to PeerConnection failed");
	}

	typedef std::pair<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> MediaStreamPair;
	m_WebRTCLocalActiveStreams.insert(MediaStreamPair(pMediaStreamInterface->label(), pMediaStreamInterface));

Error:
	return r;
}

// Video
cricket::VideoCapturer* WebRTCPeerConnection::OpenVideoCaptureDevice() {
	std::vector<std::string> deviceNames;

	{
		std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> videoCaptureModuleDeviceInfo(webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
		if (!videoCaptureModuleDeviceInfo) {
			return nullptr;
		}

		int numCaptureDevices = videoCaptureModuleDeviceInfo->NumberOfDevices();

		for (int i = 0; i < numCaptureDevices; ++i) {
			const uint32_t kSize = 256;
			char szCaptureDeviceName[kSize] = { 0 };
			char id[kSize] = { 0 };

			if (videoCaptureModuleDeviceInfo->GetDeviceName(i, szCaptureDeviceName, kSize, id, kSize) != -1) {
				deviceNames.push_back(szCaptureDeviceName);
			}
		}
	}

	cricket::WebRtcVideoDeviceCapturerFactory webRTCVideoDeviceCapturerFactory;

	cricket::VideoCapturer* pCricketVideoCapturer = nullptr;

	for (const auto& strDeviceName : deviceNames) {
		pCricketVideoCapturer = webRTCVideoDeviceCapturerFactory.Create(cricket::Device(strDeviceName, 0));

		if (pCricketVideoCapturer) {
			break;
		}
	}

	return pCricketVideoCapturer;
}



RESULT WebRTCPeerConnection::InitializeVideoCaptureDevice(std::string strDeviceName) {
	RESULT r = R_PASS;

	//m_cricketVideoCapturer

	cricket::WebRtcVideoDeviceCapturerFactory webRTCVideoDeviceCapturerFactory;

	m_pCricketVideoCapturer = webRTCVideoDeviceCapturerFactory.Create(cricket::Device(strDeviceName, 0));
	CN(m_pCricketVideoCapturer);

	/*
	rtc::WindowId wid(hW);
	cricket::ScreencastId sid(wid);

	m_pCricketVideoCapturer = m_screenCaptureFactory.Create(sid);

	//	pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
	//		m_pWebRTCPeerConnectionFactory->CreateVideoTrack(kVideoLabel, m_pWebRTCPeerConnectionFactory->CreateVideoSource(OpenVideoCaptureDevice(), nullptr)));
	pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
	
	cricket::CaptureState state = m_screenCaptureFactory.capture_state();
	*/
	
	

Error:
	return r;
}

RESULT WebRTCPeerConnection::AddVideoStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::VideoTrackInterface> pVideoTrack = nullptr;
	rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> pVideoTrackSource = nullptr;
	cricket::VideoCapturer* pVideoCapturer = nullptr;

	// Set up constraints
	webrtc::FakeConstraints videoSourceConstraints;

	// TODO: Add video constraints if needed
	//pVideoCapturer = OpenVideoCaptureDevice();
	//CN(pVideoCapturer);

	CR(InitializeVideoCaptureDevice("default_capture"));
	pVideoCapturer = m_pCricketVideoCapturer;
	CN(pVideoCapturer);

	pVideoTrackSource = m_pWebRTCPeerConnectionFactory->CreateVideoSource(pVideoCapturer, &videoSourceConstraints);
	CN(pVideoTrackSource);

	pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateVideoTrack(kVideoLabel, pVideoTrackSource)
	);
	//CN(pVideoTrack);

	pVideoTrack->AddRef();
	pMediaStreamInterface->AddTrack(pVideoTrack);

Error:
	return r;
}

RESULT WebRTCPeerConnection::AddAudioStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
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
	pAudioTrack->AddRef();
	
	pMediaStreamInterface->AddTrack(pAudioTrack);

	//pAudioTrack->GetSource()

	//Error:
	return r;
}

RESULT WebRTCPeerConnection::AddDataChannel() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCConductor::AddDataChannel");

	webrtc::DataChannelInit dataChannelInit;

	// Set max transmit time to 3 frames
	dataChannelInit.maxRetransmitTime = ((int)(1000.0f / 90.0f) * 3);
	dataChannelInit.reliable = false;
	dataChannelInit.ordered = false;

	CB((m_WebRTCLocalActiveDataChannels.find(kDataLabel) == m_WebRTCLocalActiveDataChannels.end()));

	m_pDataChannelInterface = m_pWebRTCPeerConnectionInterface->CreateDataChannel(kDataLabel, &dataChannelInit);
	CN(m_pDataChannelInterface);
	
	typedef std::pair<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface>> DataChannelPair;
	m_WebRTCLocalActiveDataChannels.insert(DataChannelPair(m_pDataChannelInterface->label(), m_pDataChannelInterface));

Error:
	return r;
}

RESULT WebRTCPeerConnection::ClearSessionDescriptionProtocols() {
	RESULT r = R_PASS;

	CR(ClearLocalSessionDescriptionProtocol());
	CR(ClearRemoteSessionDescriptionProtocol());

Error:
	return r;
}

RESULT WebRTCPeerConnection::ClearLocalSessionDescriptionProtocol() {
	m_strLocalSessionDescriptionProtocol.clear();
	m_strLocalSessionDescriptionType.clear();
	return R_PASS;
}

RESULT WebRTCPeerConnection::ClearRemoteSessionDescriptionProtocol() {
	m_strRemoteSessionDescriptionProtocol.clear();
	m_strRemoteSessionDescriptionType.clear();
	return R_PASS;
}

RESULT WebRTCPeerConnection::CloseWebRTCPeerConnection() {
	RESULT r = R_PASS;

	CN(m_pWebRTCPeerConnectionInterface);

	m_pWebRTCPeerConnectionInterface->Close();

Error:
	return r;
}

std::list<WebRTCICECandidate> WebRTCPeerConnection::GetICECandidates() {
	return m_webRTCICECandidates;
}

RESULT WebRTCPeerConnection::SetAudioVolume(double val) {
	RESULT r = R_PASS;

	util::Clamp<double>(val, 0.0f, 10.0f);

	CB((m_WebRTCLocalActiveStreams.size() > 0));
	{
		auto pMediaStream = m_WebRTCRemoteActiveStreams[kStreamLabel];
		CN(pMediaStream);

		auto pAudioTrack = pMediaStream->FindAudioTrack(kAudioLabel);
		CN(pAudioTrack);

		// Set volume
		pAudioTrack->GetSource()->SetVolume(val);
	}

Error:
	return r;
}

WebRTCPeerConnectionProxy* WebRTCPeerConnection::GetProxy() {
	return (WebRTCPeerConnectionProxy*)(this);
}

// PeerConnectionObserver Interface

// TODO: Add multiple streams (video vector, audio vector etc)
// This is important if we want to multiple video streams as we will need to do soon (this is per peer connection)
void WebRTCPeerConnection::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	
	// TODO: do we add to a map like out going? Or check existing ?

	DEBUG_LINEOUT("OnAddStream: %s", pMediaStreamInterface->label().c_str());

	// Add to remote streams
	if (m_WebRTCRemoteActiveStreams.find(pMediaStreamInterface->label()) == m_WebRTCRemoteActiveStreams.end()) {
		typedef std::pair<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> MediaStreamPair;
		m_WebRTCRemoteActiveStreams.insert(MediaStreamPair(pMediaStreamInterface->label(), pMediaStreamInterface));
	}

	if (!pMediaStreamInterface) {
		DEBUG_LINEOUT("Cannot add stream");
		return;
	}

	// Audio track
	auto pAudioTrack = pMediaStreamInterface->FindAudioTrack(kAudioLabel);
	if (pAudioTrack != nullptr) {
		auto pAudioTrackSource = pAudioTrack->GetSource();

		if (pAudioTrackSource != nullptr) {
			DEBUG_LINEOUT("Found AudioTrackSourceInterface");

			pAudioTrackSource->AddSink(this);

			//pMediaStreamInterface->FindAudioTrack(kAudioLabel)->GetSource()->SetVolume(0.0f);
			SetAudioVolume(0.0f);

			DEBUG_LINEOUT("Added audio Sink");
		}
		else {
			DEBUG_LINEOUT("Cannot AudioTrackInterface::GetSource");
		}
	}

	// Video track
	auto pVideoTrack = pMediaStreamInterface->FindVideoTrack(kVideoLabel);
	if (pVideoTrack != nullptr) {
		DEBUG_LINEOUT("Found VideoTrackSourceInterface");

		auto pVideoTrackSource = pVideoTrack->GetSource();

		if (pVideoTrackSource != nullptr) {
			// Get some information
			webrtc::VideoTrackSourceInterface::Stats stats;
			pVideoTrackSource->GetStats(&stats);

			// This object informs the source of the format requirements of the sink
			rtc::VideoSinkWants videoSinkWants = rtc::VideoSinkWants();

			pVideoTrackSource->AddOrUpdateSink(this, videoSinkWants);
			//bool res = track->GetSource()->is_screencast();
			
			//const int64_t interval = 1000000000;//33333333
			//cricket::VideoFormat format(512, 512, 1000000000, cricket::FOURCC_RGBA);
			//g_capturer->Start(format);
			
			//track->GetSource()->Restart();
		}
		else {
			DEBUG_LINEOUT("Cannot VideoTrackInterface::GetSource");
		}

	}	

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnAddStream(m_peerConnectionID, pMediaStreamInterface);
	}
}

void WebRTCPeerConnection::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	DEBUG_LINEOUT("OnRemoveStream: %s", pMediaStreamInterface->label().c_str());

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnRemoveStream(m_peerConnectionID, pMediaStreamInterface);
	}
}

void WebRTCPeerConnection::OnRenegotiationNeeded() {
	DEBUG_LINEOUT("OnRenegotiationNeeded");

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnRenegotiationNeeded(m_peerConnectionID);
	}

	return;
}

void WebRTCPeerConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannelInterface) {
	DEBUG_LINEOUT("OnDataChannel: %s", pDataChannelInterface->label().c_str());

	// Add to remote data streams
	if (m_WebRTCRemoteActiveDataChannels.find(pDataChannelInterface->label()) == m_WebRTCRemoteActiveDataChannels.end()) {
		typedef std::pair<std::string, rtc::scoped_refptr<webrtc::DataChannelInterface>> DataStreamPair;
		m_WebRTCRemoteActiveDataChannels.insert(DataStreamPair(pDataChannelInterface->label(), pDataChannelInterface));
	}

	//channel->Send(webrtc::DataBuffer("DEADBEEF"));

	// Register self as observer 
	//rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannelInterface = m_WebRTCActiveDataChannels[kDataLabel];
	//pDataChannelInterface->RegisterObserver(this);

	// TODO: 
	// m_pParentWebRTCImp->QueueUIThreadCallback(NEW_DATA, stream.release());

	pDataChannelInterface->RegisterObserver(this);

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnDataChannel(m_peerConnectionID, pDataChannelInterface);
	}
}

void WebRTCPeerConnection::OnData(const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnAudioData(m_peerConnectionID, pAudioBuffer, bitsPerSample, samplingRate, channels, frames);
	}
}

#include "webrtc/common_video/libyuv/include/webrtc_libyuv.h"

// TODO: Update WebRTC version and move to webrtc::video_frame since 
// I'm not sure what the hell cricket is all about

// TODO: Need to be wary of memory stuff
// Might want to give observer the handle for the memory and 
// they will deallocate it
void WebRTCPeerConnection::OnFrame(const cricket::VideoFrame& cricketVideoFrame) {
	RESULT r = R_PASS;

	int videoFrameWidth = cricketVideoFrame.width();
	int videoFrameHeight = cricketVideoFrame.height();

	uint8_t *pVideoFrameDataBuffer = (uint8_t*)malloc(sizeof(uint8_t) * videoFrameWidth * videoFrameHeight * 4);
	//size_t res = frame.ConvertToRgbBuffer(webrtc::VideoType::kARGB, dst_frame, frame.height()*frame.width() * 4, frame.width() * 4);
	
	const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& webRTCVideoFrameBuffer = cricketVideoFrame.video_frame_buffer();
	webrtc::VideoFrame webRTCVideoFrame(webRTCVideoFrameBuffer, 0, 0, webrtc::VideoRotation::kVideoRotation_0);
	
	/*
	VideoFrame(const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
	uint32_t timestamp,
	int64_t render_time_ms,
	VideoRotation rotation);
	*/

	int convertFromI420Result = webrtc::ConvertFromI420(webRTCVideoFrame, webrtc::VideoType::kARGB, 0, pVideoFrameDataBuffer);
	CBM((convertFromI420Result == 0), "YUV I420 conversion failed");
	
	/*
	{
		std::lock_guard<std::mutex> lock(g_UpdateTextureMutex);
		memcpy_s(m_pRecieveBuffer, m_ScreenWidth * m_ScreenHeight * 4, dst_frame, frame.height()*frame.width() * 4);
		g_updateTexture = true;
	}
	*/

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnVideoFrame(m_peerConnectionID, pVideoFrameDataBuffer, videoFrameWidth, videoFrameHeight);
	}

	return;

Error:
	// In a non-error state, this is left to the app to do
	if (pVideoFrameDataBuffer != nullptr) {
		delete pVideoFrameDataBuffer;
		pVideoFrameDataBuffer = nullptr;
	}

	return;
}

// TODO: Add callbacks
void WebRTCPeerConnection::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) {
	DEBUG_OUT("WebRTC Connection Signaling Changed: ");

	switch (new_state) {
	case webrtc::PeerConnectionInterface::kStable: {
		DEBUG_LINEOUT("WebRTC Connection Stable");
		LOG(INFO) << "WebRTC Connection Stable";
		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnWebRTCConnectionStable(m_peerConnectionID);
		}
		else {
			DEBUG_LINEOUT("No WebRTCPeerConnection Observer registered");
		}
	} break;

	case webrtc::PeerConnectionInterface::kHaveLocalOffer: {
		DEBUG_LINEOUT("WebRTC Connection Has Local Offer");
		LOG(INFO) << "WebRTC Connection Has Local Offer";
	} break;

	case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer: {
		DEBUG_LINEOUT("WebRTC Connection Has Local Answer");
		LOG(INFO) << "WebRTC Connection Has Local Answer";
	} break;

	case webrtc::PeerConnectionInterface::kHaveRemoteOffer: {
		DEBUG_LINEOUT("WebRTC Connection has remote offer");
		LOG(INFO) << "WebRTC Connection Has remote Offer";
	} break;

	case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer: {
		DEBUG_LINEOUT("WebRTC Connection has remote answer");
		LOG(INFO) << "WebRTC Connection Has remote answer";
	} break;

	case webrtc::PeerConnectionInterface::kClosed: {
		DEBUG_LINEOUT("WebRTC Connection closed");
		LOG(INFO) << "WebRTC Connection closed";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnWebRTCConnectionClosed(m_peerConnectionID);
		}
		else {
			DEBUG_LINEOUT("No WebRTC Peer Connection Observer registered");
		}
	} break;
	}
}

void WebRTCPeerConnection::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {
	DEBUG_OUT("ICE Connection Change: ");

	switch (new_state) {
	case webrtc::PeerConnectionInterface::kIceConnectionNew: {
		DEBUG_LINEOUT("ICE Connection New");
		LOG(INFO) << "ICE Connection New";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::NEW);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionChecking: {
		DEBUG_LINEOUT("ICE Connection Checking");
		LOG(INFO) << "ICE Connection Checking";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::CHECKING);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionConnected: {
		DEBUG_LINEOUT("ICE Connection Connected");
		LOG(INFO) << "ICE Connection Connected";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::CONNECTED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionCompleted: {
		DEBUG_LINEOUT("ICE Connection Completed");
		LOG(INFO) << "ICE Connection Completed";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::COMPLETED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionFailed: {
		DEBUG_LINEOUT("ICE Connection Failed");
		LOG(INFO) << "ICE Connection Failed";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::FAILED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionDisconnected: {
		DEBUG_LINEOUT("ICE Connection Disconnected");
		LOG(INFO) << "ICE Connection Disconnected";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::DISCONNECTED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionClosed: {
		DEBUG_LINEOUT("ICE Connection Closed");
		LOG(INFO) << "ICE Connection Closed";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::CLOSED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionMax: {
		DEBUG_LINEOUT("ICE Connection Max");
		LOG(INFO) << "ICE Connection Max";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::MAX);
		}
	} break;

	}
}

void WebRTCPeerConnection::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) {
	DEBUG_OUT("ICE Gathering Change: ");
	LOG(INFO) << "OnIceGatheringChange";

	switch (new_state) {
	case webrtc::PeerConnectionInterface::kIceGatheringNew: {
		DEBUG_LINEOUT("ICE Gathering New");
		LOG(INFO) << "ICE Gathering New";
	} break;

	case webrtc::PeerConnectionInterface::kIceGatheringGathering: {
		DEBUG_LINEOUT("ICE Garthering");
		LOG(INFO) << "ICE Gathering";
	} break;

	case webrtc::PeerConnectionInterface::kIceGatheringComplete: {
		DEBUG_LINEOUT("ICE Gathering Complete");
		LOG(INFO) << "ICE Gathering Complete";

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnICECandidatesGatheringDone(m_peerConnectionID);
		}
		else {
			DEBUG_LINEOUT("No WebRTC Peer Connection Observer registered");
		}
	}break;
	}
}

void WebRTCPeerConnection::OnIceConnectionReceivingChange(bool fReceiving) {
	DEBUG_LINEOUT("ICE Receiving %s", (fReceiving) ? "true" : "false");
	LOG(INFO) << "OnIceConnectionReceivingChange: " << ((fReceiving) ? "true" : "false");
}

void WebRTCPeerConnection::OnIceCandidate(const webrtc::IceCandidateInterface* pICECandidate) {
	DEBUG_LINEOUT("OnIceCandidate: %s %d", pICECandidate->sdp_mid().c_str(), pICECandidate->sdp_mline_index());
	LOG(INFO) << "OnIceCandidate: " << pICECandidate->sdp_mid() << " " << pICECandidate->sdp_mline_index();

	//Json::StyledWriter writer;
	//Json::Value jmessage;

	WebRTCICECandidate iceCandidate;
	iceCandidate.m_SDPMediateLineIndex = pICECandidate->sdp_mline_index();
	iceCandidate.m_strSDPMediaID = pICECandidate->sdp_mid();

	if (!pICECandidate->ToString(&(iceCandidate.m_strSDPCandidate))) {
		LOG(ERROR) << "Failed to serialize candidate";
		return;
	}

	m_webRTCICECandidates.push_back(iceCandidate);
}

// DataChannelObserver Implementation
std::string GetDataStateString(webrtc::DataChannelInterface::DataState state) {
	switch (state) {
	case webrtc::DataChannelInterface::DataState::kConnecting: return std::string("connecting"); break;
	case webrtc::DataChannelInterface::DataState::kOpen: return std::string("open"); break;
	case webrtc::DataChannelInterface::DataState::kClosing: return std::string("closing"); break;
	case webrtc::DataChannelInterface::DataState::kClosed: return std::string("closed"); break;
	default:  return std::string("invalid state"); break;
	}
}

void WebRTCPeerConnection::OnStateChange() {
	RESULT r = R_PASS;

	// TODO: Support multiple data channels
	//auto pWebRTCDataChannel = m_WebRTCActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);
	CN(m_pDataChannelInterface);

	//DEBUG_LINEOUT("WebRTCConductor::OnStateChange %d", pWebRTCDataChannel->state());
	auto dataChannelState = m_pDataChannelInterface->state();
	DEBUG_LINEOUT("WebRTCConductor::OnStateChange %s", GetDataStateString(dataChannelState).c_str());

	switch (dataChannelState) {
		case webrtc::DataChannelInterface::DataState::kConnecting: {
			// nothing
		} break;
		case webrtc::DataChannelInterface::DataState::kOpen: {
			// nothing
			int a = 5;
		} break;
		case webrtc::DataChannelInterface::DataState::kClosing: {
			// nothing
			int a = 5;
		} break;

		case webrtc::DataChannelInterface::DataState::kClosed: {
			// nothing 
			int a = 5;
		} break;
	}

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnDataChannelStateChange(m_peerConnectionID, m_pDataChannelInterface);
	}

Error:
	return;
}

// Data Channel OnMessage
void WebRTCPeerConnection::OnMessage(const webrtc::DataBuffer& buffer) {
	RESULT r = R_PASS;

	if (buffer.binary) {
		int pDataBuffer_n = (int)(buffer.size());
		uint8_t *pDataBuffer = new uint8_t[pDataBuffer_n];

		memset(pDataBuffer, 0, sizeof(char) * pDataBuffer_n);
		memcpy(pDataBuffer, buffer.data.data<uint8_t>(), buffer.size());

		if (m_pParentObserver != nullptr) {
			CR(m_pParentObserver->OnDataChannelMessage(m_peerConnectionID, pDataBuffer, pDataBuffer_n));
		}
		else {
			DEBUG_LINEOUT("WebRTCConductor::OnMessage (Binary Databuffer %d bytes)", (int)buffer.size());
		}
	}
	else {
		int pszBufferString_n = (int)(buffer.size()) + 1;
		char *pszBufferString = new char[pszBufferString_n];

		memset(pszBufferString, 0, sizeof(char) * pszBufferString_n);
		memcpy(pszBufferString, buffer.data.data<char>(), buffer.size());

		std::string strData = std::string(pszBufferString);

		if (m_pParentObserver != nullptr) {
			CR(m_pParentObserver->OnDataChannelStringMessage(m_peerConnectionID, strData));
		}
		else {
			DEBUG_LINEOUT("WebRTCConductor::OnMessage: %s (String Databuffer)", strData.c_str());
		}
	}
Error:
	return;
}

// CreateSessionDescriptionObserver implementation.

// OnSuccess called when PeerConnection established 
void WebRTCPeerConnection::OnSuccess(webrtc::SessionDescriptionInterface* sessionDescription) {
	RESULT r = R_PASS;

	m_strLocalSessionDescriptionType = sessionDescription->type();
	sessionDescription->ToString(&m_strLocalSessionDescriptionProtocol);

	// TODO: peer ID stuff
	// TODO: Pass m_fOffer to single call since this can be consolidated
	if (m_fOffer) {
		if (m_pParentObserver != nullptr) {
			CR(m_pParentObserver->OnSDPOfferSuccess(m_peerConnectionID));
		}
		else {
			DEBUG_LINEOUT("SDP Offer Success");
			LOG(INFO) << "SDP Offer Success";
		}
	}
	else {
		if (m_pParentObserver != nullptr) {
			CR(m_pParentObserver->OnSDPAnswerSuccess(m_peerConnectionID));
		}
		else {
			DEBUG_LINEOUT("SDP Answer Success");
			LOG(INFO) << "SDP Answer Success";
		}
	}

	// TODO: Add a better thing than DummySetSessionDescriptionObserver 
	m_pWebRTCPeerConnectionInterface->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), sessionDescription);
	m_fSDPSet = true;

	LOG(INFO) << "(cloud) set local description for " << (m_fOffer?"offer":"answer");

	CR(PrintSDP());	

	CR(ClearLocalSessionDescriptionProtocol());

Error:
	return;
}


void WebRTCPeerConnection::OnFailure(const std::string& strError) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTC Error: %s", strError.c_str());
	LOG(INFO) << "(cloud) WebRTC Error: " << strError.c_str();

	if (m_pParentObserver != nullptr) {
		CR(m_pParentObserver->OnSDPFailure(m_peerConnectionID, m_fOffer));
	}
	else {
		DEBUG_LINEOUT("SDP %s Failure", m_fOffer ? "offer" : "answer");
		LOG(INFO) << "SDP " << (m_fOffer ? "offer" : "answer") << " failure";
	}

Error:
	return;
}

// TODO: Remove arbitrary data channels etc
RESULT WebRTCPeerConnection::InitializePeerConnection(bool fAddDataChannel) {
	RESULT r = R_PASS;

	CN(m_pWebRTCPeerConnectionFactory);	// ensure peer connection initialized
	CB((m_pWebRTCPeerConnectionInterface.get() == nullptr));			// ensure peer connection uninitialized

	//CBM((CreatePeerConnection(DTLS_OFF)), "Error CreatePeerConnection failed");
	CBM((CreatePeerConnection(DTLS_ON)), "Error CreatePeerConnection failed");
	CN(m_pWebRTCPeerConnectionInterface.get());

#ifndef WEBRTC_NO_CANDIDATES
	CR(AddStreams());

	// TODO: Do this moar bettar
	//if (fAddDataChannel) {
	//	CR(AddDataChannel());
	//}
#endif

Error:
	return r;
}

RESULT WebRTCPeerConnection::CreatePeerConnection(bool dtls) {
	RESULT r = R_PASS;

	webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;
	rtcConfiguration.dscp();

	webrtc::PeerConnectionInterface::IceServer iceServer;
	webrtc::FakeConstraints webrtcConstraints;
	std::unique_ptr<rtc::RTCCertificateGeneratorInterface> pCertificateGenerator = nullptr;
	TwilioNTSInformation twilioNTSInformation = m_pParentObserver->GetTwilioNTSInformation();

	CN(m_pWebRTCPeerConnectionFactory.get());		// ensure factory is valid
	CB((m_pWebRTCPeerConnectionInterface.get() == nullptr));	// ensure peer connection is nullptr

	for (auto &strICEServerURI : twilioNTSInformation.m_ICEServerURIs) {
		iceServer.uri = strICEServerURI;
		iceServer.username = twilioNTSInformation.GetUsername();
		iceServer.password = twilioNTSInformation.GetPassword();
		rtcConfiguration.servers.push_back(iceServer);
	}

	if (dtls) {
		if (rtc::SSLStreamAdapter::HaveDtlsSrtp()) {
			pCertificateGenerator = std::unique_ptr<rtc::RTCCertificateGeneratorInterface>(new FakeRTCCertificateGenerator());
		}

		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
		//webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, "true");

		rtcConfiguration.enable_dtls_srtp = rtc::Optional<bool>(true);
		//rtcConfiguration.enable_rtp_data_channel = true;

		m_pWebRTCPeerConnectionInterface = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, &webrtcConstraints, NULL, std::move(pCertificateGenerator), this);
	}
	else {
		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "false");
		webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, "true");

		m_pWebRTCPeerConnectionInterface = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, &webrtcConstraints, NULL, NULL, this);
	}

	CNM(m_pWebRTCPeerConnectionInterface.get(), "WebRTC Peer Connection failed to initialize");

Error:
	return r;
}

RESULT WebRTCPeerConnection::CreateOffer() {
	RESULT r = R_PASS;

	CN(m_pWebRTCPeerConnectionInterface);

	m_fOffer = true;
	m_pWebRTCPeerConnectionInterface->CreateOffer(this, NULL);

Error:
	return r;
}

RESULT WebRTCPeerConnection::CreateSDPOfferAnswer(std::string strSDPOffer) {
	RESULT r = R_PASS;

	m_fOffer = false;

	webrtc::SdpParseError sdpError;
	webrtc::SessionDescriptionInterface* sessionDescriptionInterface(webrtc::CreateSessionDescription("offer", strSDPOffer, &sdpError));
	std::string strSDPType;

	CNM((sessionDescriptionInterface),
		"Can't parse received session description message. SdpParseError was: %s", sdpError.description.c_str());

	strSDPType = sessionDescriptionInterface->type();

	DEBUG_LINEOUT(" Received %s session description: %s", strSDPType.c_str(), strSDPOffer.c_str());

	m_pWebRTCPeerConnectionInterface->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), sessionDescriptionInterface);

	//if (sessionDescriptionInterface->type() == webrtc::SessionDescriptionInterface::kOffer) {
	//if(strSDPType == webrtc::SessionDescriptionInterface::kOffer) {
	if (strSDPType == "offer") {
		m_pWebRTCPeerConnectionInterface->CreateAnswer(this, NULL);
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

RESULT WebRTCPeerConnection::SetSDPAnswer(std::string strSDPAnswer) {
	RESULT r = R_PASS;

	webrtc::SdpParseError sdpError;

	// TODO: Make this more generic (string)
	webrtc::SessionDescriptionInterface* sessionDescriptionInterface(webrtc::CreateSessionDescription("answer", strSDPAnswer, &sdpError));
	std::string strSDPType = sessionDescriptionInterface->type();

	CNM((sessionDescriptionInterface),
		"Can't parse received session description message. SdpParseError was: %s", sdpError.description.c_str());

	DEBUG_LINEOUT(" Received %s session description: %s", strSDPType.c_str(), strSDPAnswer.c_str());

	m_pWebRTCPeerConnectionInterface->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), sessionDescriptionInterface);

	// TODO: Save to string + type 

Error:
	return r;
}

RESULT WebRTCPeerConnection::AddIceCandidate(WebRTCICECandidate iceCandidate) {
	RESULT r = R_PASS;
	
	webrtc::SdpParseError sdpError;

	std::unique_ptr<webrtc::IceCandidateInterface> candidate(
		webrtc::CreateIceCandidate(iceCandidate.m_strSDPMediaID, iceCandidate.m_SDPMediateLineIndex,
			iceCandidate.m_strSDPCandidate, &sdpError));

	CBM((candidate.get()), "Can't parse received candidate message. SdpParseError was: %s", sdpError.description.c_str());
	CBM((m_pWebRTCPeerConnectionInterface->AddIceCandidate(candidate.get())), "Failed to apply the received candidate");

	DEBUG_LINEOUT("Received candidate : %s", iceCandidate.m_strSDPCandidate.c_str());
	LOG(INFO) << "Received candidate : " << iceCandidate.m_strSDPCandidate.c_str();
	
// Success:
	return r;

Error:
	LOG(INFO) << "Candidate " << iceCandidate.m_strSDPCandidate.c_str() << " failed with error: " << sdpError.description.c_str();
	return r;
}

RESULT WebRTCPeerConnection::SendDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	//m_SignalOnDataChannel

	auto pWebRTCDataChannel = m_WebRTCLocalActiveDataChannels[kDataLabel];
	//CN(pWebRTCDataChannel);
	CN(m_pDataChannelInterface);

	//CB(pWebRTCDataChannel->Send(webrtc::DataBuffer(strMessage)));
	//CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(strMessage)));
	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(rtc::CopyOnWriteBuffer(strMessage.c_str(), strMessage.length()), true)));

Error:
	return r;
}

RESULT WebRTCPeerConnection::SendDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;
	
	auto pWebRTCDataChannel = m_WebRTCLocalActiveDataChannels[kDataLabel];
	CN(m_pDataChannelInterface);

	if (m_pDataChannelInterface->buffered_amount() > 1000) {
		int a = 5;
	}

	CB(m_pDataChannelInterface->Send(webrtc::DataBuffer(rtc::CopyOnWriteBuffer(pDataChannelBuffer, pDataChannelBuffer_n), true)));
	
Error:
	return r;
}

// TODO: This is not ideal, should be replaced with more robust flag
bool WebRTCPeerConnection::IsPeerConnectionInitialized() {
	if (m_pWebRTCPeerConnectionInterface.get() == nullptr)
		return false;
	else
		return true;
}

std::string WebRTCPeerConnection::GetPeerConnectionString() {
	// Issues behind the NAT
	//return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:74.125.196.127:19302");
	//return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
	//return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.ekiga.net");

	return std::string("stun:stun.l.google.com:19302");
	//return std::string("stun:stun.ekiga.net");
}

RESULT WebRTCPeerConnection::PrintSDP() {
	RESULT r = R_PASS;

	CR(PrintLocalSDP());
	CR(PrintRemoteSDP());

Error:
	return r;
}

std::string WebRTCPeerConnection::GetSDPJSONString(std::string strSessionDescriptionType, std::string strSessionDescriptionProtocol) {
	Json::StyledWriter JSONWriter;
	Json::Value JSONMessage;

	JSONMessage[kSessionDescriptionTypeName] = strSessionDescriptionType;
	JSONMessage[kSessionDescriptionSdpName] = strSessionDescriptionProtocol;

	// Append Candidates
	for (auto &iceCandidate : m_webRTCICECandidates) {
		Json::Value JSONIceCandidate;

		JSONIceCandidate[kCandidateSdpName] = iceCandidate.m_strSDPCandidate;
		JSONIceCandidate[kCandidateSdpMidName] = iceCandidate.m_strSDPMediaID;
		JSONIceCandidate[kCandidateSdpMlineIndexName] = iceCandidate.m_SDPMediateLineIndex;

		JSONMessage["candidates"].append(JSONIceCandidate);
	}

	std::string strReturn = JSONWriter.write(JSONMessage);

	return strReturn;
}

RESULT WebRTCPeerConnection::PrintLocalSDP() {
	DEBUG_LINEOUT("WebRTCConductor: Local SDP:");
	LOG(INFO) << "WebRTCConductor: Local SDP:";

	DEBUG_LINEOUT("%s", m_strLocalSessionDescriptionProtocol.c_str());
	LOG(INFO) << m_strLocalSessionDescriptionProtocol.c_str();

	return R_PASS;
}

std::string WebRTCPeerConnection::GetLocalSDPTypeString() {
	return m_strLocalSessionDescriptionType;
}

std::string WebRTCPeerConnection::GetLocalSDPString() {
	return m_strLocalSessionDescriptionProtocol;
}

std::string WebRTCPeerConnection::GetLocalSDPJSONString() {
	return GetSDPJSONString(m_strLocalSessionDescriptionType, m_strLocalSessionDescriptionProtocol);
}

RESULT WebRTCPeerConnection::PrintRemoteSDP() {
	DEBUG_LINEOUT("WebRTCConductor: Remote SDP:");
	LOG(INFO) << "WebRTCConductor: Remote SDP:";

	DEBUG_LINEOUT("%s", m_strRemoteSessionDescriptionProtocol.c_str());
	LOG(INFO) << m_strRemoteSessionDescriptionProtocol.c_str();

	return R_PASS;
}

std::string WebRTCPeerConnection::GetRemoteSDPTypeString() {
	return m_strRemoteSessionDescriptionType;
}

std::string WebRTCPeerConnection::GetRemoteSDPString() {
	return m_strRemoteSessionDescriptionProtocol;
}

std::string WebRTCPeerConnection::GetRemoteSDPJSONString() {
	return GetSDPJSONString(m_strRemoteSessionDescriptionType, m_strRemoteSessionDescriptionProtocol);
}
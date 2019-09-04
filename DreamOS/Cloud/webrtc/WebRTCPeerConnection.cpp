#include "WebRTCPeerConnection.h"

// Logging is redefining macros due to CEF, Logging++ and WebRTC
// When we solve logging we need to solve this too
#pragma warning( disable : 4005)

#include "DreamLogger/DreamLogger.h"

#include "WebRTCConductor.h"

#include "WebRTCCommon.h"

#include <memory>
#include <utility>
#include <vector>

#include "api/test/fakeconstraints.h"

//#include "base/common.h"
#include "rtc_base/json.h"
#include "base/logging.h"
#include "examples/peerconnection/client/defaults.h"
#include "media/engine/webrtcvideocapturerfactory.h"
#include "api/mediastreaminterface.h"

#include "modules/video_capture/video_capture_factory.h"

#include "pc/test/fakertccertificategenerator.h"

#include "p2p/base/fakeportallocator.h"

#include "Cloud/User/TwilioNTSInformation.h"

#include "Core/Utilities.h"

#include "WebRTCCustomVideoCapturer.h"
#include "WebRTCLocalAudioTrack.h"

#include "Primitives/texture.h"

#include "common_video/libyuv/include/webrtc_libyuv.h"

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
		DOSLOG(INFO, "DummySetSessionDescriptionObserver On Success");
	}

	virtual void OnFailure(const std::string& strError) {
		DOSLOG(INFO, "[DummySetSessionDescriptionObserver] On Failure: %v", strError);
		DOSLOG(INFO, "DummySetSessionDescriptionObserver On Failure: %s", strError.c_str());
	}
};

WebRTCPeerConnection::WebRTCPeerConnection(WebRTCPeerConnectionObserver *pParentObserver, 
										   long peerConnectionID, 
										   rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory,
										   WebRTCConductor *pParentWebRTCConductor) :
	m_pParentObserver(pParentObserver),
	m_peerConnectionID(peerConnectionID),
	m_pWebRTCPeerConnectionFactory(nullptr),
	m_pWebRTCPeerConnectionInterface(nullptr),
	m_WebRTCPeerID(-1),
	m_pParentWebRTCConductor(pParentWebRTCConductor)
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

RESULT WebRTCPeerConnection::SetUserPeerConnectionFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pWebRTCPeerConnectionFactory) {
	m_pWebRTCPeerConnectionFactory = pWebRTCPeerConnectionFactory;
	return R_PASS;
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
	CR(AddStreams(fAddDataChannel));
#endif

Error:
	return r;
}

RESULT WebRTCPeerConnection::AddStreams(bool fAddDataChannel) {
	RESULT r = R_PASS;

	typedef std::pair<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> MediaStreamPair;

	// User Stream
	CR(AddLocalAudioSource(kUserAudioLabel, kUserStreamLabel));
	
	// Chrome Stream
	CR(AddLocalAudioSource(kChromeAudioLabel, kChromeStreamLabel));
	CR(AddVideoStream(kChromeCaptureDevice, kChromeVideoLabel, kChromeStreamLabel));

	// Virtual Camera Stream
	CR(AddVideoStream(kVCamCaptureDevice, kVCamVideoLabel, kVCamStreamLabel));
	CR(AddLocalAudioSource(kVCamAudiolabel, kVCamStreamLabel));

	// Data Channel
	// This is not in the media streaming interface
	if (fAddDataChannel) {
		CR(AddDataChannel());
	}

Error:
	return r;
}

// Video
std::unique_ptr<cricket::VideoCapturer> WebRTCPeerConnection::OpenVideoCaptureDevice() {
	std::vector<std::string> deviceNames;

	{
		std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> videoCaptureModuleDeviceInfo(webrtc::VideoCaptureFactory::CreateDeviceInfo());
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

	std::unique_ptr<cricket::VideoCapturer> pCricketVideoCapturer = nullptr;

	for (const auto& strDeviceName : deviceNames) {
		pCricketVideoCapturer = webRTCVideoDeviceCapturerFactory.Create(cricket::Device(strDeviceName, 0));

		if (pCricketVideoCapturer) {
			break;
		}
	}

	return pCricketVideoCapturer;
}



RESULT WebRTCPeerConnection::InitializeVideoCaptureDevice(std::string strDeviceName, std::string strVideoTrackLabel) {
	RESULT r = R_PASS;

	WebRTCCustomVideoCapturerFactory webrtcCustomVideoCapturerFactory;
	
	// First check such a device doesn't exist
	CBM((m_videoCaptureDevices.find(strVideoTrackLabel) == m_videoCaptureDevices.end()), "Video Track %s already exists", strVideoTrackLabel.c_str());

	m_videoCaptureDevices[strVideoTrackLabel] = webrtcCustomVideoCapturerFactory.Create(cricket::Device(strDeviceName, 0));
	CNM(m_videoCaptureDevices[strVideoTrackLabel], "Failed to create video capturer");

Error:
	return r;
}

cricket::VideoCapturer* WebRTCPeerConnection::GetVideoCaptureDeviceByTrackName(std::string strTrackName) {
	if(m_videoCaptureDevices.find(strTrackName) != m_videoCaptureDevices.end()) {
		return (m_videoCaptureDevices[strTrackName]).get();
	}
	else {
		return nullptr;
	}
}

RESULT WebRTCPeerConnection::AddVideoStream(const std::string &strVideoCaptureDevice, const std::string &strVideoTrackLabel, const std::string &strMediaStreamLabel) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::VideoTrackInterface> pVideoTrack = nullptr;
	rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> pVideoTrackSource = nullptr;
	cricket::VideoCapturer* pVideoCapturer = nullptr;

	// Set up constraints
	webrtc::FakeConstraints videoSourceConstraints;

	CR(InitializeVideoCaptureDevice(strVideoCaptureDevice, strVideoTrackLabel));

	pVideoCapturer = GetVideoCaptureDeviceByTrackName(strVideoTrackLabel);
	CN(pVideoCapturer);

	pVideoTrackSource = m_pWebRTCPeerConnectionFactory->CreateVideoSource(pVideoCapturer, &videoSourceConstraints);
	CN(pVideoTrackSource);

	pVideoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateVideoTrack(strVideoTrackLabel, pVideoTrackSource)
	);
	CN(pVideoTrack);

	pVideoTrack->AddRef();
	m_pWebRTCPeerConnectionInterface->AddTrack(pVideoTrack, {strMediaStreamLabel});

Error:
	return r;
}

// NOTE: This is not being used currently
RESULT WebRTCPeerConnection::SendAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CB((m_pWebRTCLocalAudioSources.find(strAudioTrackLabel) != m_pWebRTCLocalAudioSources.end()));

	CN(m_pWebRTCLocalAudioSources[strAudioTrackLabel]);
	CR(m_pWebRTCLocalAudioSources[strAudioTrackLabel]->SendAudioPacket(pendingAudioPacket));

Error:
	return r;
}

// TODO:
RESULT WebRTCPeerConnection::AddLocalAudioSource(const std::string &strAudioTrackLabel, const std::string &strMediaStreamLabel) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::AudioTrackInterface> pLocalAudioTrack = nullptr;

	// Set up constraints
	webrtc::FakeConstraints audioSourceConstraints;
	webrtc::PeerConnectionFactoryInterface::Options fakeOptions;

	cricket::AudioOptions fakeAudioOptions;

	// Ensure no duplicate names
	CB((m_pWebRTCLocalAudioSources.find(strAudioTrackLabel) == m_pWebRTCLocalAudioSources.end()));

	{
		/*
		audioSourceConstraints.AddMandatory(webrtc::MediaConstraintsInterface::kGoogEchoCancellation, false);
		audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kExtendedFilterEchoCancellation, true);
		audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kDAEchoCancellation, true);
		audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kAutoGainControl, true);
		audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl, true);
		audioSourceConstraints.AddMandatory(webrtc::MediaConstraintsInterface::kNoiseSuppression, false);
		audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kHighpassFilter, true);
		//audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, true);
		//*/

		//fakeAudioOptions.playout_sample_rate = rtc::Optional<uint32_t>(44100);
		//fakeAudioOptions.recording_sample_rate = rtc::Optional<uint32_t>(44100);
		
		fakeAudioOptions.echo_cancellation = rtc::Optional<bool>(false);
		fakeAudioOptions.auto_gain_control = rtc::Optional<bool>(false);
		fakeAudioOptions.noise_suppression = rtc::Optional<bool>(false);
		fakeAudioOptions.highpass_filter = rtc::Optional<bool>(false);
		fakeAudioOptions.typing_detection = rtc::Optional<bool>(false);

		auto pWebRTCLocalAudioSource = WebRTCLocalAudioSource::Create(strAudioTrackLabel, fakeAudioOptions);
		CN(pWebRTCLocalAudioSource);

		pWebRTCLocalAudioSource->SetAudioSourceName(strAudioTrackLabel);

		// Add to map
		m_pWebRTCLocalAudioSources[strAudioTrackLabel] = pWebRTCLocalAudioSource;

		///*
		pLocalAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
			m_pWebRTCPeerConnectionFactory->CreateAudioTrack(
				strAudioTrackLabel,
				pWebRTCLocalAudioSource)
			);
		CN(pLocalAudioTrack);

		pLocalAudioTrack->AddRef();

		auto pRTPSender = m_pWebRTCPeerConnectionInterface->AddTrack(pLocalAudioTrack, {strMediaStreamLabel});
		//std::string strTrackID = pRTPSender.value()->id();

	}


Error:
	return r;
}

RESULT WebRTCPeerConnection::AddAudioStream(const std::string &strAudioTrackLabel) {
	RESULT r = R_PASS;

	rtc::scoped_refptr<webrtc::AudioTrackInterface> pAudioTrack = nullptr;

	// Set up constraints
	webrtc::FakeConstraints audioSourceConstraints;

	///*
	audioSourceConstraints.AddMandatory(webrtc::MediaConstraintsInterface::kGoogEchoCancellation, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kExtendedFilterEchoCancellation, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kDAEchoCancellation, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kAutoGainControl, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl, false);
	audioSourceConstraints.AddMandatory(webrtc::MediaConstraintsInterface::kNoiseSuppression, false);
	audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kHighpassFilter, false);
	//*/

	//audioSourceConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, true);

	pAudioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
		m_pWebRTCPeerConnectionFactory->CreateAudioTrack(
			strAudioTrackLabel,
			m_pWebRTCPeerConnectionFactory->CreateAudioSource(&audioSourceConstraints))
		);

	pAudioTrack->AddRef();

	m_pWebRTCPeerConnectionInterface->AddTrack(pAudioTrack, {kUserStreamLabel});

Error:
	return r;
}

RESULT WebRTCPeerConnection::AddDataChannel() {
	RESULT r = R_PASS;

	DOSLOG(INFO, "WebRTCConductor::AddDataChannel");

	webrtc::DataChannelInit dataChannelInit;

	// Set max transmit time to 3 frames
	//dataChannelInit.maxRetransmitTime = ((int)(1000.0f / 90.0f) * 3);
	dataChannelInit.reliable = false;
	dataChannelInit.ordered = false;

	CB((m_WebRTCLocalActiveDataChannels.find(kUserDataLabel) == m_WebRTCLocalActiveDataChannels.end()));

	m_pDataChannelInterface = m_pWebRTCPeerConnectionInterface->CreateDataChannel(kUserDataLabel, &dataChannelInit);
	CN(m_pDataChannelInterface);

	m_pDataChannelInterface->AddRef();
	
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

	//CB((m_WebRTCLocalActiveStreams.size() > 0));

	{
		//auto pMediaStream = m_WebRTCRemoteActiveStreams[kUserStreamLabel];
		//CN(pMediaStream);
			
		auto pMediaStreamTrack = m_pWebRTCPeerConnectionInterface->local_streams()->FindAudioTrack(kUserAudioLabel);
		//auto pAudioTrack = pMediaStream->FindAudioTrack(kUserAudioLabel);
		CN(pMediaStreamTrack);

		auto pAudioTrack = dynamic_cast<webrtc::AudioTrackInterface*>(pMediaStreamTrack);
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

// TODO: remove this
#define _USE_TEST_APP

// TODO: Add multiple streams (video vector, audio vector etc)
// This is important if we want to multiple video streams as we will need to do soon (this is per peer connection)
void WebRTCPeerConnection::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	
	// TODO: do we add to a map like out going? Or check existing ?

	DOSLOG(INFO, "OnAddStream: %s", pMediaStreamInterface->id().c_str());
	
	if (!pMediaStreamInterface) {
		DOSLOG(INFO, "Cannot add stream");
		return;
	}
	
	auto audioTracks = pMediaStreamInterface->GetAudioTracks();

	// TODO: Turn this shit into a function bruh

	// User Audio track	
	auto pUserAudioTrack = pMediaStreamInterface->FindAudioTrack(kUserAudioLabel);
	if (pUserAudioTrack != nullptr) {
		if (pUserAudioTrack->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {

			std::string strTrackName = pUserAudioTrack->id();

			DOSLOG(INFO, "OnAddStream: %s", strTrackName.c_str());

			webrtc::AudioSourceInterface* pUserAudioTrackSource = pUserAudioTrack->GetSource();

			if (pUserAudioTrackSource != nullptr) {
				std::shared_ptr<WebRTCAudioTrackSink> pWebRTCAudioTrackSink = std::make_shared<WebRTCAudioTrackSink>(strTrackName);
				
				pWebRTCAudioTrackSink->RegisterObserver(this);

				m_webRTCAudioTrackSinks[strTrackName] = pWebRTCAudioTrackSink;
				
				pUserAudioTrackSource->AddSink(pWebRTCAudioTrackSink.get());
				
			}
		}
	}

	// Chrome Audio Track
	auto pChromeAudioTrack = pMediaStreamInterface->FindAudioTrack(kChromeAudioLabel);
	if (pChromeAudioTrack != nullptr && pChromeAudioTrack->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {

		std::string strTrackName = pChromeAudioTrack->id();

		DOSLOG(INFO, "OnAddStream: %s", strTrackName.c_str());

		webrtc::AudioSourceInterface* pChromeAudioTrackSource = pChromeAudioTrack->GetSource();

		if (pChromeAudioTrackSource != nullptr) {

			std::shared_ptr<WebRTCAudioTrackSink> pWebRTCAudioTrackSink = std::make_shared<WebRTCAudioTrackSink>(strTrackName);
			pWebRTCAudioTrackSink->RegisterObserver(this);
			m_webRTCAudioTrackSinks[strTrackName] = pWebRTCAudioTrackSink;

			pChromeAudioTrackSource->AddSink(pWebRTCAudioTrackSink.get());
			
		}
	}
	
	// VCam Audio Track
	auto pVCamAudioTrack = pMediaStreamInterface->FindAudioTrack(kVCamAudiolabel);
	if (pVCamAudioTrack != nullptr && pVCamAudioTrack->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {

		std::string strTrackName = pVCamAudioTrack->id();

		DOSLOG(INFO, "OnAddStream: %s", strTrackName.c_str());

		webrtc::AudioSourceInterface* pVCamAudioTrackSource = pVCamAudioTrack->GetSource();

		if (pVCamAudioTrackSource != nullptr) {

			std::shared_ptr<WebRTCAudioTrackSink> pWebRTCAudioTrackSink = std::make_shared<WebRTCAudioTrackSink>(strTrackName);
			pWebRTCAudioTrackSink->RegisterObserver(this);
			m_webRTCAudioTrackSinks[strTrackName] = pWebRTCAudioTrackSink;

			pVCamAudioTrackSource->AddSink(pWebRTCAudioTrackSink.get());

		}
	}

	// Chrome Video track
	auto pVideoTrack = pMediaStreamInterface->FindVideoTrack(kChromeVideoLabel);
	if (pVideoTrack != nullptr) {
		DOSLOG(INFO, "Found VideoTrackSourceInterface");
	
		auto pVideoTrackSource = pVideoTrack->GetSource();
	
		if (pVideoTrackSource != nullptr) {
			if (R_PASS != InitializeVideoSink(kChromeVideoLabel, pVideoTrackSource)) {
				DEBUG_LINEOUT("Failed to initialize video sink for vcam");
			}
	
			///*
			if (m_videoCaptureDevices[kChromeCaptureDevice] != nullptr) {
				cricket::VideoFormat videoCaptureFormat(1280, 960, cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_ARGB);
				m_videoCaptureDevices[kChromeCaptureDevice]->Start(videoCaptureFormat);
			}
			//*/
	
		}
		else {
			DOSLOG(INFO, "Cannot VideoTrackInterface::GetSource");
		}
	}	

	// VCam Video track
	pVideoTrack = pMediaStreamInterface->FindVideoTrack(kVCamVideoLabel);
	if (pVideoTrack != nullptr) {
		DOSLOG(INFO, "Found VideoTrackSourceInterface");

		auto pVideoTrackSource = pVideoTrack->GetSource();

		if (pVideoTrackSource != nullptr) {

			if (R_PASS != InitializeVideoSink(kVCamVideoLabel, pVideoTrackSource)) {
				DEBUG_LINEOUT("Failed to initialize video sink for vcam");
			}

			///*
			// TODO: Put into function?
			if (m_videoCaptureDevices[kVCamCaptureDevice] != nullptr) {
				cricket::VideoFormat videoCaptureFormat(1280, 960, cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_ARGB);
				m_videoCaptureDevices[kVCamCaptureDevice]->Start(videoCaptureFormat);
			}
			//*/

		}
		else {
			DOSLOG(INFO, "Cannot VideoTrackInterface::GetSource");
		}
	}
	
	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnAddStream(m_peerConnectionID, pMediaStreamInterface);
	}
}

RESULT WebRTCPeerConnection::InitializeVideoSink(std::string strTrackName, webrtc::VideoTrackSourceInterface* pVideoTrackSource) {
	RESULT r = R_PASS;

	// First check such a device doesn't exist
	CBM((m_videoSinks.find(strTrackName) == m_videoSinks.end()), "Video Sink %s already exists", strTrackName.c_str());

	m_videoSinks[strTrackName] = std::unique_ptr<WebRTCVideoSink>(WebRTCVideoSink::MakeWebRTCVideoSink(strTrackName, this, pVideoTrackSource));
	CNM(m_videoSinks[strTrackName], "Failed to create video capturer");

Error:
	return r;
}

WebRTCVideoSink* WebRTCPeerConnection::GetVideoSink(std::string strTrackName) {
	if (m_videoSinks.find(strTrackName) != m_videoSinks.end()) {
		return (m_videoSinks[strTrackName]).get();
	}
	else {
		return nullptr;
	}
}

void WebRTCPeerConnection::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> pMediaStreamInterface) {
	DOSLOG(INFO, "OnRemoveStream: %s", pMediaStreamInterface->id().c_str());

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnRemoveStream(m_peerConnectionID, pMediaStreamInterface);
	}
}

// TODO: We want to move to these when unified is fully baked
void WebRTCPeerConnection::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> pReceiver,
	const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&streams)
{
	rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> pTrack = pReceiver->track();
	
	std::string strMediaStreamName = streams[0]->id();
	
	if (pTrack->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {
		
		webrtc::AudioTrackInterface* pAudioTrack = static_cast<webrtc::AudioTrackInterface*>(pTrack.get());
		std::string strAudioTrackName = pAudioTrack->id();
	
		DOSLOG(INFO, "OnAddTrack: %s", strAudioTrackName.c_str());
	//
	//	// TODO: get the actual stupid name
	//	//m_webRTCAudioTrackSinks
	//	std::shared_ptr<WebRTCAudioTrackSink> pWebRTCAudioTrackSink = std::make_shared<WebRTCAudioTrackSink>(strAudioTrackName);
	//	m_webRTCAudioTrackSinks[strAudioTrackName] = pWebRTCAudioTrackSink;
	//
	//	pAudioTrack->AddSink(pWebRTCAudioTrackSink.get());
	//
	//	//auto pAudioTrackSource = pAudioTrack->GetSource();
	//	//pAudioTrackSource->AddSink(this);
	}
}

void WebRTCPeerConnection::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) {
	DOSLOG(INFO, "OnRemoveTrack: %s", receiver->track()->id().c_str());
}

void WebRTCPeerConnection::OnRenegotiationNeeded() {
	DOSLOG(INFO, "OnRenegotiationNeeded");

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnRenegotiationNeeded(m_peerConnectionID);
	}

	return;
}

void WebRTCPeerConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> pDataChannelInterface) {
	DOSLOG(INFO, "OnDataChannel: %s", pDataChannelInterface->label().c_str());

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

void WebRTCPeerConnection::OnAudioTrackSinkData(std::string strAudioTrackLabel, const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {	
	//DOSLOG(INFO, "OnAudioData: %s %d samples", strAudioTrackLabel.c_str(), (int)frames);
	
	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnAudioData(strAudioTrackLabel, m_peerConnectionID, pAudioBuffer, bitsPerSample, samplingRate, channels, frames);
	}

}

/*
// TODO: Update WebRTC version and move to webrtc::video_frame since 
// I'm not sure what the hell cricket is all about

// TODO: Create a per-stream video sink! 
// TODO: Need to be wary of memory stuff
// Might want to give observer the handle for the memory and 
// they will deallocate it
void WebRTCPeerConnection::OnFrame(const webrtc::VideoFrame& cricketVideoFrame) {
	RESULT r = R_PASS;

	int videoFrameWidth = cricketVideoFrame.width();
	int videoFrameHeight = cricketVideoFrame.height();

	uint8_t *pVideoFrameDataBuffer = (uint8_t*)malloc(sizeof(uint8_t) * videoFrameWidth * videoFrameHeight * 4);
	//size_t res = frame.ConvertToRgbBuffer(webrtc::VideoType::kARGB, dst_frame, frame.height()*frame.width() * 4, frame.width() * 4);
	
	const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& webRTCVideoFrameBuffer = cricketVideoFrame.video_frame_buffer();
	webrtc::VideoFrame webRTCVideoFrame(webRTCVideoFrameBuffer, 0, 0, webrtc::VideoRotation::kVideoRotation_0);
	
	//VideoFrame(const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
	//uint32_t timestamp,
	//int64_t render_time_ms,
	//VideoRotation rotation);

	int convertFromI420Result = webrtc::ConvertFromI420(webRTCVideoFrame, webrtc::VideoType::kARGB, 0, pVideoFrameDataBuffer);
	CBM((convertFromI420Result == 0), "YUV I420 conversion failed");
	
	//{
	//	std::lock_guard<std::mutex> lock(g_UpdateTextureMutex);
	//	memcpy_s(m_pRecieveBuffer, m_ScreenWidth * m_ScreenHeight * 4, dst_frame, frame.height()*frame.width() * 4);
	//	g_updateTexture = true;
	//}

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
*/

RESULT WebRTCPeerConnection::OnVideoFrame(std::string strVideoTrackName, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnVideoFrame(strVideoTrackName, m_peerConnectionID, pVideoFrameDataBuffer, pxWidth, pxHeight);
	}

Error:
	return r;
}


// TODO: Add callbacks
void WebRTCPeerConnection::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) {
	DEBUG_OUT("WebRTC Connection Signaling Changed: ");

	switch (new_state) {
	case webrtc::PeerConnectionInterface::kStable: {
		DOSLOG(INFO, "WebRTC Connection Stable");
		DOSLOG(INFO, "%v WebRTC Connection Stable", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnWebRTCConnectionStable(m_peerConnectionID);
		}
		else {
			DOSLOG(INFO, "No WebRTCPeerConnection Observer registered");
		}
	} break;

	case webrtc::PeerConnectionInterface::kHaveLocalOffer: {
		DOSLOG(INFO, "WebRTC Connection Has Local Offer");
		DOSLOG(INFO, "%v WebRTC Connection Has Local Offer", GetLogSignature());
	} break;

	case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer: {
		DOSLOG(INFO, "WebRTC Connection Has Local Answer");
		DOSLOG(INFO, "%v WebRTC Connection Has Local Answer", GetLogSignature());
	} break;

	case webrtc::PeerConnectionInterface::kHaveRemoteOffer: {
		DOSLOG(INFO, "WebRTC Connection has remote offer");
		DOSLOG(INFO, "%v WebRTC Connection Has remote Offer", GetLogSignature());
	} break;

	case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer: {
		DOSLOG(INFO, "WebRTC Connection has remote answer");
		DOSLOG(INFO, "%v WebRTC Connection Has remote answer", GetLogSignature());
	} break;

	case webrtc::PeerConnectionInterface::kClosed: {
		DOSLOG(INFO, "WebRTC Connection closed");
		DOSLOG(INFO, "%v WebRTC Connection closed", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnWebRTCConnectionClosed(m_peerConnectionID);
		}
		else {
			DOSLOG(INFO, "No WebRTC Peer Connection Observer registered");
		}
	} break;
	}
}

void WebRTCPeerConnection::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {
	DEBUG_OUT("ICE Connection Change: ");

	switch (new_state) {
	case webrtc::PeerConnectionInterface::kIceConnectionNew: {
		DOSLOG(INFO, "ICE Connection New");
		DOSLOG(INFO, "%v ICE Connection New", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::NEW);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionChecking: {
		DOSLOG(INFO, "ICE Connection Checking");
		DOSLOG(INFO, "%v ICE Connection Checking", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::CHECKING);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionConnected: {
		DOSLOG(INFO, "ICE Connection Connected");
		DOSLOG(INFO, "%v ICE Connection Connected", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::CONNECTED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionCompleted: {
		DOSLOG(INFO, "ICE Connection Completed");
		DOSLOG(INFO, "%v ICE Connection Completed", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::COMPLETED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionFailed: {
		DOSLOG(INFO, "ICE Connection Failed");
		DOSLOG(INFO, "%v ICE Connection Failed", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::FAILED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionDisconnected: {
		DOSLOG(INFO, "ICE Connection Disconnected");
		DOSLOG(INFO, "%v ICE Connection Disconnected", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::DISCONNECTED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionClosed: {
		DOSLOG(INFO, "ICE Connection Closed");
		DOSLOG(INFO, "%v ICE Connection Closed", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::CLOSED);
		}
	} break;

	case webrtc::PeerConnectionInterface::kIceConnectionMax: {
		DOSLOG(INFO, "ICE Connection Max");
		DOSLOG(INFO, "%v ICE Connection Max", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			m_pParentObserver->OnIceConnectionChange(m_peerConnectionID, WebRTCIceConnection::state::MAX);
		}
	} break;

	}
}

void WebRTCPeerConnection::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) {
	DEBUG_OUT("ICE Gathering Change: ");

	switch (new_state) {
	case webrtc::PeerConnectionInterface::kIceGatheringNew: {
		DOSLOG(INFO, "ICE Gathering New");
		DOSLOG(INFO, "%v ICE Gathering New", GetLogSignature());
	} break;

	case webrtc::PeerConnectionInterface::kIceGatheringGathering: {
		DOSLOG(INFO, "ICE Garthering");
		DOSLOG(INFO, "%v ICE Gathering", GetLogSignature());
	} break;

	case webrtc::PeerConnectionInterface::kIceGatheringComplete: {
		DOSLOG(INFO, "ICE Gathering Complete");
		DOSLOG(INFO, "%v ICE Gathering Complete", GetLogSignature());

		if (m_pParentObserver != nullptr) {
			//m_pParentObserver->OnICECandidatesGatheringDone(m_peerConnectionID);
		}
		else {
			DOSLOG(INFO, "No WebRTC Peer Connection Observer registered");
		}
	}break;
	}
}

void WebRTCPeerConnection::OnIceConnectionReceivingChange(bool fReceiving) {
	DOSLOG(INFO, "ICE Receiving %s", (fReceiving) ? "true" : "false");
	DOSLOG(INFO, "[WebRTCPeerConnection] OnIceConnectionReceivingChange: %v", ((fReceiving) ? "true" : "false"));
}

void WebRTCPeerConnection::OnIceCandidate(const webrtc::IceCandidateInterface* pICECandidate) {
	DOSLOG(INFO, "OnIceCandidate: %s %d", pICECandidate->sdp_mid().c_str(), pICECandidate->sdp_mline_index());
	DOSLOG(INFO, "[WebRTCPeerConnection] OnIceCandidate: %v %v", pICECandidate->sdp_mid(), pICECandidate->sdp_mline_index());
	DOSLOG(INFO, "Using: %s %s %s %s", pICECandidate->candidate().protocol(), pICECandidate->candidate().priority(), pICECandidate->candidate().type(), pICECandidate->candidate().url());
	//Json::StyledWriter writer;
	//Json::Value jmessage;

	WebRTCICECandidate iceCandidate;
	iceCandidate.m_SDPMediateLineIndex = pICECandidate->sdp_mline_index();
	iceCandidate.m_strSDPMediaID = pICECandidate->sdp_mid();

	if (!pICECandidate->ToString(&(iceCandidate.m_strSDPCandidate))) {
		DOSLOG(ERR, "Failed to serialize candidate");
		return;
	}

	m_webRTCICECandidates.push_back(iceCandidate);

	// we can just use the last value instead of creating another copy
	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnICECandidateGathered(&m_webRTCICECandidates.back(), m_peerConnectionID);
	}
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

	//DOSLOG(INFO, "WebRTCConductor::OnStateChange %d", pWebRTCDataChannel->state());
	auto dataChannelState = m_pDataChannelInterface->state();
	DOSLOG(INFO, "WebRTCConductor::OnStateChange %s", GetDataStateString(dataChannelState).c_str());

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
			DOSLOG(INFO, "WebRTCConductor::OnMessage (Binary Databuffer %d bytes)", (int)buffer.size());
		}

		delete[] pDataBuffer;
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
			DOSLOG(INFO, "WebRTCConductor::OnMessage: %s (String Databuffer)", strData.c_str());
		}

		delete[] pszBufferString;
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
			DOSLOG(INFO, "SDP Offer Success");
			DOSLOG(INFO, "%v SDP Offer Success", GetLogSignature());
		}
	}
	else {
		if (m_pParentObserver != nullptr) {
			CR(m_pParentObserver->OnSDPAnswerSuccess(m_peerConnectionID));
		}
		else {
			DOSLOG(INFO, "SDP Answer Success");
			DOSLOG(INFO, "%v SDP Answer Success", GetLogSignature());
		}
	}

	// TODO: Add a better thing than DummySetSessionDescriptionObserver 
	m_pWebRTCPeerConnectionInterface->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), sessionDescription);
	m_fSDPSet = true;

	DOSLOG(INFO, "[WebRTCPeerConnection] set local description for %v", (m_fOffer ? "offer" : "answer"));

	CR(PrintSDP());	

	CR(ClearLocalSessionDescriptionProtocol());

Error:
	return;
}


void WebRTCPeerConnection::OnFailure(const std::string& strError) {
	RESULT r = R_PASS;

	DOSLOG(INFO, "WebRTC Error: %s", strError.c_str());
	DOSLOG(INFO, "[WebRTCPeerConnection] WebRTC Error: %v", strError.c_str());

	if (m_pParentObserver != nullptr) {
		CR(m_pParentObserver->OnSDPFailure(m_peerConnectionID, m_fOffer));
	}
	else {
		DOSLOG(INFO, "SDP %s Failure", m_fOffer ? "offer" : "answer");
		DOSLOG(INFO, "[WebRTCPeerConnection] SDP %v failure", (m_fOffer ? "offer" : "answer")); 
	}

Error:
	return;
}

#include "p2p/client/basicportallocator.h"

RESULT WebRTCPeerConnection::CreatePeerConnection(bool dtls) {
	RESULT r = R_PASS;

	webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;
	rtcConfiguration.enable_dtls_srtp = dtls;
	//rtcConfiguration.ice_connection_receiving_timeout = 1000;
	
	// Not really working?
	//rtcConfiguration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

	webrtc::PeerConnectionInterface::IceServer iceServer;
	webrtc::FakeConstraints webrtcConstraints;
	std::unique_ptr<rtc::RTCCertificateGeneratorInterface> pCertificateGenerator = nullptr;
	TwilioNTSInformation twilioNTSInformation = m_pParentObserver->GetTwilioNTSInformation();

	std::unique_ptr<cricket::BasicPortAllocator> pPortAllocator = nullptr;

	CN(m_pWebRTCPeerConnectionFactory.get());		// ensure factory is valid
	CB((m_pWebRTCPeerConnectionInterface.get() == nullptr));	// ensure peer connection is nullptr

	for (int i = 0; i < twilioNTSInformation.m_ICEServerURIs.size(); i++) {
		
		iceServer.urls.emplace_back(twilioNTSInformation.m_ICEServerURIs[i]);
		//DOSLOG(INFO, "Adding ice server: %s", twilioNTSInformation.m_ICEServerURIs[i]);

		iceServer.username = twilioNTSInformation.m_ICEServerUsernames[i];
		//DOSLOG(INFO, "Username: %s", twilioNTSInformation.m_ICEServerUsernames[i]);

		iceServer.password = twilioNTSInformation.m_ICEServerPasswords[i];
		//DOSLOG(INFO, "Password: %s", twilioNTSInformation.m_ICEServerPasswords[i]);

		//iceServer.tls_cert_policy = webrtc::PeerConnectionInterface::kTlsCertPolicyInsecureNoCheck;

		rtcConfiguration.servers.push_back(iceServer);
	}

	//iceServer.uri = "turn:w1.xirsys.com:80?transport=tcp";
	//iceServer.username = "e1fa02b0-c151-11e8-8acc-4963be209ae3";
	//iceServer.password = "e1fa0332-c151-11e8-80e4-3b6f3523fb32";
	//rtcConfiguration.servers.push_back(iceServer);

	//// Testing
	//{
	//	webrtc::PeerConnectionInterface::IceServer testGoogleSTUNServer;
	//	testGoogleSTUNServer.uri = "stun:stun.l.google.com:19302";
	//	rtcConfiguration.servers.push_back(testGoogleSTUNServer);
	//}

	if (dtls) {
		//if (rtc::SSLStreamAdapter::HaveDtlsSrtp()) {
		//pCertificateGenerator = std::unique_ptr<rtc::RTCCertificateGeneratorInterface>(new FakeRTCCertificateGenerator());
		pCertificateGenerator = std::unique_ptr<rtc::RTCCertificateGeneratorInterface>(
			new rtc::RTCCertificateGenerator(m_pParentWebRTCConductor->m_signalingThread.get(), 
											 m_pParentWebRTCConductor->m_workerThread.get())
		);
		//}

		//webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
		//webrtcConstraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, "true");

		webrtcConstraints.SetAllowDtlsSctpDataChannels();
		//rtcConfiguration.enable_dtls_srtp = rtc::Optional<bool>(true);

		//rtcConfiguration.enable_rtp_data_channel = true;

		m_pWebRTCPeerConnectionInterface = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(
			rtcConfiguration,
			&webrtcConstraints,
			nullptr,
			std::move(pCertificateGenerator),
			this
		);

		//webrtc::PeerConnectionDependencies peerConnectionDependencies;
		//peerConnectionDependencies.observer = this;
		//peerConnectionDependencies.cert_generator = nullptr;
		//peerConnectionDependencies.tls_cert_verifier = nullptr;
		//peerConnectionDependencies.allocator = nullptr;
		//
		//m_pWebRTCPeerConnectionInterface = m_pWebRTCPeerConnectionFactory->CreatePeerConnection(rtcConfiguration, peerConnectionDependencies);
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
	m_pWebRTCPeerConnectionInterface->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());

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

	DOSLOG(INFO, " Received %s session description: %s", strSDPType.c_str(), strSDPOffer.c_str());

	m_pWebRTCPeerConnectionInterface->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), sessionDescriptionInterface);

	//if (sessionDescriptionInterface->type() == webrtc::SessionDescriptionInterface::kOffer) {
	//if(strSDPType == webrtc::SessionDescriptionInterface::kOffer) {
	if (strSDPType == "offer") {
		m_pWebRTCPeerConnectionInterface->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
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

	DOSLOG(INFO, " Received %s session description: %s", strSDPType.c_str(), strSDPAnswer.c_str());

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
			iceCandidate.m_strSDPCandidate, &sdpError)
	);

	CBM((candidate.get()), "Can't parse received candidate message. SdpParseError was: %s", sdpError.description.c_str());
	CBM((m_pWebRTCPeerConnectionInterface->AddIceCandidate(candidate.get())), "Failed to apply the received candidate");

	DOSLOG(INFO, "Received candidate : %s", iceCandidate.m_strSDPCandidate.c_str());
	DOSLOG(INFO, "%v Received candidate: %v", GetLogSignature(), iceCandidate.m_strSDPCandidate);
	
// Success:
	return r;

Error:
	DOSLOG(INFO, "%v Candidate %v failed with error: %v", GetLogSignature(), iceCandidate.m_strSDPCandidate, sdpError.description);
	return r;
}

RESULT WebRTCPeerConnection::SendDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	//m_SignalOnDataChannel

	auto pWebRTCDataChannel = m_WebRTCLocalActiveDataChannels[kUserDataLabel];
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
	
	auto pWebRTCDataChannel = m_WebRTCLocalActiveDataChannels[kUserDataLabel];
	CN(m_pDataChannelInterface);

	if (m_pDataChannelInterface->buffered_amount() > 1000) {
		int a = 5;
	}

	// Since UpdateHead and Hands from DreamGarage is unprotected - it should check if peers exist before trying to broadcast the message
	CBR(m_pDataChannelInterface->Send(webrtc::DataBuffer(rtc::CopyOnWriteBuffer(pDataChannelBuffer, pDataChannelBuffer_n), true)), R_SKIPPED);
	
Error:
	return r;
}

RESULT WebRTCPeerConnection::SendVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	CN(m_videoCaptureDevices[strVideoTrackLabel]);

	WebRTCCustomVideoCapturer* pWebRTCCustomVideoCapturer = (WebRTCCustomVideoCapturer*)(m_videoCaptureDevices[strVideoTrackLabel].get());

	CR(pWebRTCCustomVideoCapturer->SubmitNewFrameBuffer(pVideoFrameBuffer, pxWidth, pxHeight, channels));

Error:
	return r;
}

uint32_t GetCricketVideoFormatColorSpace(PIXEL_FORMAT pixelFormat) {
	switch (pixelFormat) {
		case PIXEL_FORMAT::RGB: {
			cricket::FOURCC_RGB3;
		} break;

		case PIXEL_FORMAT::RGBA: {
			cricket::FOURCC_RGBA;
		} break;

		case PIXEL_FORMAT::BGR: {
			cricket::FOURCC_BGR3;
		} break;

		case PIXEL_FORMAT::BGRA: {
			cricket::FOURCC_BGRA;
		} break;
	}

	return cricket::FOURCC_24BG;
}

// TODO: The start / stop and IsRunning pathways are being maintained as pathways, 
// but currently don't do anything (although the IsRunning is working correctly)
// this is because WebRTC doesn't really seem to work this way.  Not providing frames
// to the video source is the same as not streaming. 

RESULT WebRTCPeerConnection::StartVideoStreaming(const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	return R_DEPRECATED;

	CN(m_videoCaptureDevices[strVideoTrackLabel]);

	{
		cricket::VideoFormat desiredVideoFormat;

		cricket::VideoFormat videoCaptureFormat(
			pxDesiredWidth,
			pxDesiredHeight,
			cricket::VideoFormat::FpsToInterval(desiredFPS),
			GetCricketVideoFormatColorSpace(pixelFormat)
		);

		cricket::VideoFormat streamingVideoFormat;
		CB(m_videoCaptureDevices[strVideoTrackLabel]->GetBestCaptureFormat(desiredVideoFormat, &streamingVideoFormat));

		m_videoCaptureDevices[strVideoTrackLabel]->Start(streamingVideoFormat);

		CB((IsVideoStreamingRunning(strVideoTrackLabel)));
	}

Error:
	return r;
}

RESULT WebRTCPeerConnection::StopVideoStreaming(const std::string &strVideoTrackLabel) {
	RESULT r = R_PASS;

	return R_DEPRECATED;

	CN(m_videoCaptureDevices[strVideoTrackLabel]);

	{
		m_videoCaptureDevices[strVideoTrackLabel]->Stop();

		CB((IsVideoStreamingRunning(strVideoTrackLabel) == false));
	}

Error:
	return r;
}

bool WebRTCPeerConnection::IsVideoStreamingRunning(const std::string &strVideoTrackLabel) {
	RESULT r = R_PASS;

	CN(m_videoCaptureDevices[strVideoTrackLabel]);

	return m_videoCaptureDevices[strVideoTrackLabel]->IsRunning();

Error:
	return false; 
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
	DOSLOG(INFO, "WebRTCConductor: Local SDP:");
	DOSLOG(INFO, "%s", m_strLocalSessionDescriptionProtocol.c_str());

	DOSLOG(INFO, "[WebRTCPeerConnection] WebRTCConductor: Local SDP: %v", m_strLocalSessionDescriptionProtocol);

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
	DOSLOG(INFO, "WebRTCPeerConnection: Remote SDP:");
	DOSLOG(INFO, "%s", m_strRemoteSessionDescriptionProtocol.c_str());

	DOSLOG(INFO, "[WebRTCPeerConnection] WebRTCConductor: Remote SDP: %v", m_strRemoteSessionDescriptionProtocol);

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
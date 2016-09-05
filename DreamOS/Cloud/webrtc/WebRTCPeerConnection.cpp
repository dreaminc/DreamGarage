#include "WebRTCPeerConnection.h"

RESULT WebRTCPeerConnection::AddStreams() {
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

RESULT WebRTCPeerConnection::ClearSessionDescriptionProtocol() {
	m_strSessionDescriptionProtocol.clear();
	m_strSessionDescriptionType.clear();
	return R_PASS;
}
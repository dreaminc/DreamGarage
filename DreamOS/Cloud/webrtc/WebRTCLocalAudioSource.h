#ifndef WEBRTC_LOCAL_AUDIO_SOURCE_H_
#define WEBRTC_LOCAL_AUDIO_SOURCE_H_	
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTC.h
// A customer WebRTC Audio Capture Device

#include "webrtc/api/localaudiosource.h"

class AudioPacket;

class WebRTCLocalAudioSource : public webrtc::LocalAudioSource {
public:
	WebRTCLocalAudioSource() {
		// empty
	}

	~WebRTCLocalAudioSource() {
		// empty
	}

public:
	static rtc::scoped_refptr<WebRTCLocalAudioSource> Create(
		const webrtc::PeerConnectionFactoryInterface::Options& options,
		const webrtc::MediaConstraintsInterface* mediaConstraints
	);

public:
	virtual void AddSink(webrtc::AudioTrackSinkInterface* pLocalAudioTrackSink) override;

	RESULT SendAudioPacket(const AudioPacket &pendingAudioPacket);

	RESULT SetAudioSourceName(const std::string &strAudioTrackLabel) {
		m_strAudioTrackLabel = strAudioTrackLabel;
		return R_PASS;
	}

private:
	webrtc::AudioTrackSinkInterface* m_pLocalAudioTrackSink = nullptr;

private:
	std::string m_strAudioTrackLabel;
};

#endif // WEBRTC_LOCAL_AUDIO_SOURCE_H_

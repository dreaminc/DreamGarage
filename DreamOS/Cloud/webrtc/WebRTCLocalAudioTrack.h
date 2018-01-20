#ifndef WEBRTC_LOCAL_AUDIO_TRACK_H_
#define WEBRTC_LOCAL_AUDIO_TRACK_H_	
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTC.h
// A customer WebRTC Audio Capture Device

#include "pc/audiotrack.h"

class AudioPacket;

class WebRTCLocalAudioTrack : public webrtc::AudioTrack {
public:
	WebRTCLocalAudioTrack(const std::string& label,
		const rtc::scoped_refptr<webrtc::AudioSourceInterface>& source) :
		AudioTrack(label, source)
	{
		// empty
	}

	~WebRTCLocalAudioTrack() {
		// empty
	}

public:
	static rtc::scoped_refptr<WebRTCLocalAudioTrack> Create(
		const std::string& id, 
		const rtc::scoped_refptr<webrtc::AudioSourceInterface>& source
	);

public:
	virtual void AddSink(webrtc::AudioTrackSinkInterface* pLocalAudioTrackSink) override;

	RESULT SendAudioPacket(const AudioPacket &pendingAudioPacket);

private:
	webrtc::AudioTrackSinkInterface* m_pLocalAudioTrackSink = nullptr;
};

#endif // WEBRTC_LOCAL_AUDIO_SOURCE_H_

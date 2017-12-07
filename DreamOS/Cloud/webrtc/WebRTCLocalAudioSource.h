#ifndef WEBRTC_LOCAL_AUDIO_SOURCE_H_
#define WEBRTC_LOCAL_AUDIO_SOURCE_H_	
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTC.h
// A customer WebRTC Audio Capture Device

#include "webrtc/api/localaudiosource.h"
#include "webrtc/api/mediaconstraintsinterface.h"

class AudioPacket;

class WebRTCLocalAudioSource : public webrtc::LocalAudioSource {
public:
	WebRTCLocalAudioSource() {
		// empty
	}

	WebRTCLocalAudioSource(const std::string& strTrackName, const webrtc::MediaConstraintsInterface* constraints)
		: m_strAudioTrackLabel(strTrackName)
	{
		//webrtc::CopyConstraintsIntoAudioOptions(constraints, &m_audioOptions);
	}

	WebRTCLocalAudioSource(const std::string& strTrackName, const cricket::AudioOptions& audio_options) : 
		m_strAudioTrackLabel(strTrackName), 
		m_audioOptions(audio_options)
	{
		// empty
	}

	~WebRTCLocalAudioSource() override {
		// empty
	}


public:
	static rtc::scoped_refptr<WebRTCLocalAudioSource> Create(const std::string& sTrackName, const webrtc::MediaConstraintsInterface* constraints);
	static rtc::scoped_refptr<WebRTCLocalAudioSource> Create(const std::string& sTrackName, const cricket::AudioOptions& audio_options);

public:
	virtual void AddSink(webrtc::AudioTrackSinkInterface* pLocalAudioTrackSink) override;
	virtual void RemoveSink(webrtc::AudioTrackSinkInterface* sink) override;

	RESULT SendAudioPacket(const AudioPacket &pendingAudioPacket);

	RESULT SetAudioSourceName(const std::string &strAudioTrackLabel) {
		m_strAudioTrackLabel = strAudioTrackLabel;
		return R_PASS;
	}

private:
	webrtc::AudioTrackSinkInterface* m_pLocalAudioTrackSink = nullptr;

private:
	std::string m_strAudioTrackLabel;
	cricket::AudioOptions m_audioOptions;
};

#endif // WEBRTC_LOCAL_AUDIO_SOURCE_H_

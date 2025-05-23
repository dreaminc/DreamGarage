#ifndef WEBRTC_AUDIO_TRACK_SINK_H_
#define WEBRTC_AUDIO_TRACK_SINK_H_

#include "core/ehm/EHM.h"

// Dream Cloud WebRTC
// dos/src/cloud/webrtc/WebRTCAudioTrackSink.h

// The container for the WebRTCAudioTrackSink per our implementation

#include <memory>

#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"

#include "pc/localaudiosource.h"

class WebRTCAudioTrackSink : public webrtc::AudioTrackSinkInterface {
public:
	class observer {
	public:
		virtual void OnAudioTrackSinkData(std::string strAudioTrackLabel, const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	};

public:
	WebRTCAudioTrackSink(std::string strAudioTrackLabel);
	~WebRTCAudioTrackSink();

	// webrtc::AudioTrackSinkInterface
	virtual void OnData(const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;

	RESULT RegisterObserver(WebRTCAudioTrackSink::observer *pParentObserver);
	RESULT UnregisterObserver();

private:
	WebRTCAudioTrackSink::observer *m_pParentObserver = nullptr;

	std::string m_strAudioTrackLabel;
};

#endif // ! WEBRTC_AUDIO_TRACK_SINK_H_
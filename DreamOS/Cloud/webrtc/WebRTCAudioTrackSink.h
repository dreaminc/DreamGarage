#ifndef WEBRTC_AUDIO_TRACK_SINK_H_
#define WEBRTC_AUDIO_TRACK_SINK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCAudioTrackSink.h
// The container for the WebRTCAudioTrackSink per our implementation

#include <memory>

#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"

#include "pc/localaudiosource.h"

class WebRTCAudioTrackSink : public webrtc::AudioTrackSinkInterface {

public:
	WebRTCAudioTrackSink(std::string strAudioTrackLabel);
	~WebRTCAudioTrackSink();

	// webrtc::AudioTrackSinkInterface
	virtual void OnData(const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;

private:
	std::string m_strAudioTrackLabel;
};

#endif // ! WEBRTC_AUDIO_TRACK_SINK_H_
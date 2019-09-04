#include "WebRTCAudioTrackSink.h"

WebRTCAudioTrackSink::WebRTCAudioTrackSink(std::string strAudioTrackLabel) :
	m_strAudioTrackLabel(strAudioTrackLabel)
{
	// empty
}

WebRTCAudioTrackSink::~WebRTCAudioTrackSink() {
	// empty
}

// webrtc::AudioTrackSinkInterface
void WebRTCAudioTrackSink::OnData(const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {

	//DEBUG_LINEOUT("OnAudioData: %s %d samples", m_strAudioTrackLabel.c_str(), (int)frames);

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnAudioTrackSinkData(m_strAudioTrackLabel, pAudioBuffer, bitsPerSample, samplingRate, channels, frames);
	}
}

RESULT WebRTCAudioTrackSink::RegisterObserver(WebRTCAudioTrackSink::observer *pParentObserver) {
	RESULT r = R_PASS;

	CNM(pParentObserver, "Can't register null observer");
	CBM((m_pParentObserver == nullptr), "Can't register observer already registered");

	m_pParentObserver = pParentObserver;

Error:
	return r;
}

RESULT WebRTCAudioTrackSink::UnregisterObserver() {
	RESULT r = R_PASS;

	CNM(m_pParentObserver, "Can't unregister null observer");

	m_pParentObserver = nullptr;

Error:
	return r;
}
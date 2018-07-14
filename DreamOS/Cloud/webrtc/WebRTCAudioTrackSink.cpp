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

	DEBUG_LINEOUT("OnAudioData: %s %d samples", m_strAudioTrackLabel.c_str(), (int)frames);

	//if (m_pParentObserver != nullptr) {
	//	m_pParentObserver->OnAudioData(strAudioTrackLabel, m_peerConnectionID, pAudioBuffer, bitsPerSample, samplingRate, channels, frames);
	//}
}
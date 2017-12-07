#include "WebRTCLocalAudioTrack.h"

#include "Sound/AudioPacket.h"

rtc::scoped_refptr<WebRTCLocalAudioTrack> WebRTCLocalAudioTrack::Create(
	const std::string& id,
	const rtc::scoped_refptr<webrtc::AudioSourceInterface>& source) 
{
	return new rtc::RefCountedObject<WebRTCLocalAudioTrack>(id, source);
}

void WebRTCLocalAudioTrack::AddSink(webrtc::AudioTrackSinkInterface* pLocalAudioTrackSink) {
	m_pLocalAudioTrackSink = pLocalAudioTrackSink;
}

RESULT WebRTCLocalAudioTrack::SendAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pLocalAudioTrackSink);

	//m_pLocalAudioSourceSink->OnData(
	//	pendingAudioPacket.GetDataBuffer(),
	//	pendingAudioPacket.GetBitsPerSample(),
	//	pendingAudioPacket.GetSamplingRate(),
	//	pendingAudioPacket.GetNumChannels(),
	//	pendingAudioPacket.GetNumFrames()
	//);

	int samples_per_sec = 44100;
	int nSamples = pendingAudioPacket.GetNumFrames();
	int channels = 1;

	static int count = 0;
	static int16_t *pDataBuffer = nullptr;

	if (pDataBuffer == nullptr) {
		pDataBuffer = new int16_t[nSamples];

		for (int i = 0; i < nSamples; i++) {
			pDataBuffer[i] = sin((count * 4200.0f) / samples_per_sec) * 10000;
			count++;
		}
	}

	m_pLocalAudioTrackSink->OnData(
		pDataBuffer,
		16,
		samples_per_sec,
		channels,
		nSamples
	);

Error:
	return r;
}
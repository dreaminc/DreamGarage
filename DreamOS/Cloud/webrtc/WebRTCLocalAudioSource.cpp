#include "WebRTCLocalAudioSource.h"

#include "Sound/AudioPacket.h"

rtc::scoped_refptr<WebRTCLocalAudioSource> WebRTCLocalAudioSource::Create(
	const webrtc::PeerConnectionFactoryInterface::Options& options,
	const webrtc::MediaConstraintsInterface* mediaConstraints) 
{
	rtc::scoped_refptr<WebRTCLocalAudioSource> pWebRTCLocalAudioSource(
		//new rtc::RefCountedObject<WebRTCLocalAudioSource>(options, mediaConstraints)
		new rtc::RefCountedObject<WebRTCLocalAudioSource>()
	);

	//pWebRTCLocalAudioSource->options() = options;

	return pWebRTCLocalAudioSource;
}

// Save sink
void WebRTCLocalAudioSource::AddSink(webrtc::AudioTrackSinkInterface* pLocalAudioTrackSink) {
	m_pLocalAudioTrackSink = pLocalAudioTrackSink;
	//RemoveSink(pLocalAudioTrackSink);
}


RESULT WebRTCLocalAudioSource::SendAudioPacket(const AudioPacket &pendingAudioPacket) {
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
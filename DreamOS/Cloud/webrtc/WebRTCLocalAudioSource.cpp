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
	//int nSamples = pendingAudioPacket.GetNumFrames();
	int nSamples = pendingAudioPacket.GetNumFrames();
	int channels = 1;
	
	static double theta = 0.0f;
	double freq = 440.0f;

	if (m_strAudioTrackLabel == "chrome_audio_label")
		freq *= 2.0f;

	int16_t *pDataBuffer = nullptr;  
	
	if (pDataBuffer == nullptr) {
		pDataBuffer = new int16_t[nSamples * channels];

		for (int i = 0; i < nSamples * channels; i++) {
			float val = sin(theta);
			//val *= 0.25f;

			for (int j = 0; j < channels; j++) {
				pDataBuffer[i + j] = (int16_t)(val * 10000.0f);
			}

			// increment theta
			theta += ((2.0f * M_PI) / 44100.0f) * freq;
			if (theta >= 2.0f * M_PI) {
				theta = theta - (2.0f * M_PI);
			}
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
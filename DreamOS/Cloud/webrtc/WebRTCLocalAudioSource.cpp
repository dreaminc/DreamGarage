#include "WebRTCLocalAudioSource.h"

#include "Sound/AudioPacket.h"

/*
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
*/


rtc::scoped_refptr<WebRTCLocalAudioSource> WebRTCLocalAudioSource::Create(
	const std::string& strTrackName, 
	const webrtc::MediaConstraintsInterface* constraints)
{
	rtc::scoped_refptr<WebRTCLocalAudioSource> pWebRTCLocalAudioSource(
		new rtc::RefCountedObject<WebRTCLocalAudioSource>(strTrackName, constraints)
	);

	return pWebRTCLocalAudioSource;
}

rtc::scoped_refptr<WebRTCLocalAudioSource> WebRTCLocalAudioSource::Create(
	const std::string& strTrackName, 
	const cricket::AudioOptions& audio_options)
{
	rtc::scoped_refptr<WebRTCLocalAudioSource> pWebRTCLocalAudioSource(
		new rtc::RefCountedObject<WebRTCLocalAudioSource>(strTrackName, audio_options)
	);

	return pWebRTCLocalAudioSource;
}

// Save sink
void WebRTCLocalAudioSource::AddSink(webrtc::AudioTrackSinkInterface* pLocalAudioTrackSink) {
	m_pLocalAudioTrackSink = pLocalAudioTrackSink;
	//RemoveSink(pLocalAudioTrackSink);
}

void WebRTCLocalAudioSource::RemoveSink(webrtc::AudioTrackSinkInterface* sink) { 
	m_pLocalAudioTrackSink = nullptr;
}


RESULT WebRTCLocalAudioSource::SendAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pLocalAudioTrackSink);
	
	///*
	m_pLocalAudioTrackSink->OnData(
		pendingAudioPacket.GetDataBuffer(),
		pendingAudioPacket.GetBitsPerSample(),
		pendingAudioPacket.GetSamplingRate(),
		pendingAudioPacket.GetNumChannels(),
		pendingAudioPacket.GetNumFrames()
	);
	//*/

	/* DEBUG:
	CN(m_pLocalAudioTrackSink);
	
	if (m_strAudioTrackLabel != "user_audio_label") {
	
		m_pLocalAudioTrackSink->OnData(
			pendingAudioPacket.GetDataBuffer(),
			pendingAudioPacket.GetBitsPerSample(),
			pendingAudioPacket.GetSamplingRate(),
			pendingAudioPacket.GetNumChannels(),
			pendingAudioPacket.GetSamplingRate() / 100		// 10 ms of frames
		);
	}
	else {

		//void *pBufferWithOffset = pendingAudioPacket.GetDataBuffer();
		//pBufferWithOffset = (uint8_t*)(pBufferWithOffset) + ((pendingAudioPacket.GetSamplingRate() / 100) * pendingAudioPacket.GetBytesPerSample() * pendingAudioPacket.GetNumChannels());
		//
		//m_pLocalAudioTrackSink->OnData(
		//	pBufferWithOffset,
		//	pendingAudioPacket.GetBitsPerSample(),
		//	pendingAudioPacket.GetSamplingRate(),
		//	pendingAudioPacket.GetNumChannels(),
		//	pendingAudioPacket.GetSamplingRate() / 100		// 10 ms of frames
		//);

		///* DEBUG
		int samples_per_sec = pendingAudioPacket.GetSamplingRate();
		int nSamples = pendingAudioPacket.GetNumFrames();
		int channels = pendingAudioPacket.GetNumChannels();

		static int16_t *pStaticDataBufferSine = nullptr;
		static int16_t *pStaticDataBufferSine2 = nullptr;
		static int16_t *pStaticDataBufferEmpty = nullptr;

		if (pStaticDataBufferSine == nullptr) {

			pStaticDataBufferSine = new int16_t[nSamples * channels];
			pStaticDataBufferSine2 = new int16_t[nSamples * channels];

			double theta = 0.0f;
			//double theta2 = 0.0f;

			for (int i = 0; i < nSamples * channels; i++) {
				// sine
				float val = sin(theta);
				float val2 = sin(theta * 1.5f);

				// saw
				//float val = (fmod(theta / (2.0f * M_PI), 1.0f) * 2.0f) - 1.0f;
				//float val2 = (fmod((theta * 2.0f) / (2.0f * M_PI), 1.0f) * 2.0f) - 1.0f;

				//float val2 = 0.0f;

				val *= 0.5f;
				val2 *= 0.5f;

				for (int j = 0; j < channels; j++) {
					pStaticDataBufferSine[i + j] = (int16_t)(val * 10000.0f);
					pStaticDataBufferSine2[i + j] = (int16_t)(val2 * 10000.0f);
				}

				// increment theta
				theta += ((2.0f * M_PI) / nSamples) * 3.0f;

				//if (theta >= (2.0f * M_PI)) {
				//	theta = theta - (2.0f * M_PI);
				//}
			}
		}

		if (pStaticDataBufferEmpty == nullptr) {
			pStaticDataBufferEmpty = new int16_t[nSamples * channels];
			memset(pStaticDataBufferEmpty, 0, sizeof(int16_t) * nSamples * channels);
		}

		//if (m_strAudioTrackLabel == "user_audio_label") {
		m_pLocalAudioTrackSink->OnData(
			pStaticDataBufferSine,
			16,
			samples_per_sec,
			channels,
			nSamples
		);
		/*}
		else {
			m_pLocalAudioTrackSink->OnData(
				//pStaticDataBufferEmpty,
				pStaticDataBufferSine2,
				16,
				samples_per_sec,
				channels,
				nSamples
			);
		}
		
	}
	//*/

Error:
	return r;
}
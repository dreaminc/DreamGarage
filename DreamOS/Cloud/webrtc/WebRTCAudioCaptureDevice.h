#ifndef WEBRTC_AUDIO_CAPTURE_DEVICE_H_
#define WEBRTC_AUDIO_CAPTURE_DEVICE_H_	
#pragma once

// The above had to be changed due to an already existing WEBRTC_COMMON_H_ in the WebRTC code namespace

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCAudioCaptureDevice.h
// A customer WebRTC Audio Capture Device

#include <queue>

#include "webrtc/modules/audio_device/include/audio_device_defines.h"
#include "webrtc/modules/audio_device/include/audio_device.h"
//#include "webrtc/rtc_base/scoped_ref_ptr.h"

#include "Sound/AudioPacket.h"

// This interface will capture the raw PCM data of both the local captured as
// well as the mixed/rendered remote audio.
class AudioDeviceDataCapturer {
public:
	/*
	virtual void OnCaptureData(const void* audio_samples,
		const size_t num_samples,
		const size_t bytes_per_sample,
		const size_t num_channels,
		const uint32_t samples_per_sec) = 0;

	virtual void OnRenderData(const void* audio_samples,
		const size_t num_samples,
		const size_t bytes_per_sample,
		const size_t num_channels,
		const uint32_t samples_per_sec) = 0;
		*/

	/*
	virtual void PushCaptureData(int voe_channel,
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames) = 0;

	virtual void PullRenderData(int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames,
		void* audio_data,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms) = 0;
		*/

	AudioDeviceDataCapturer() = default;
	virtual ~AudioDeviceDataCapturer() = default;

	RESULT Initialize();

	RESULT PushAudioPacket(const AudioPacket audioPacket);

	RESULT SetAudioTransport(webrtc::AudioTransport* pAudioTransport);

	/*
	bool IsAudioPacketPending() {
		return (m_pendingAudioPackets.size() > 0) ? true : false;
	}

	size_t NumberPendingAudioPackets() {
		return m_pendingAudioPackets.size();
	}

	AudioPacket PopPendingAudioPacket() {
		AudioPacket pendingAudioPacket = m_pendingAudioPackets.front();
		m_pendingAudioPackets.pop();
		return pendingAudioPacket;
	}

	RESULT CleapPEndingAudioPacket() {
		while (m_pendingAudioPackets.size() > 0)
			m_pendingAudioPackets.pop();

		return R_PASS;
	}

private:
	std::queue<AudioPacket> m_pendingAudioPackets;
	*/

	webrtc::AudioTransport* m_pAudioTransport = nullptr;
};

// Creates an ADM instance with AudioDeviceDataObserver registered.
rtc::scoped_refptr<webrtc::AudioDeviceModule> CreateAudioDeviceWithDataCapturer(
	const int32_t id,
	const webrtc::AudioDeviceModule::AudioLayer audio_layer,
	AudioDeviceDataCapturer* pAudioDeviceDataCapturer);

#endif // WEBRTC_AUDIO_CAPTURE_DEVICE_H_
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
#include "Sound/SoundBuffer.h"

// TODO: Shouldn't actually need capturer 
// but lets get this to work first
class WebRTCAudioDeviceModule;

// This interface will capture the raw PCM data of both the local captured as
// well as the mixed/rendered remote audio.
class AudioDeviceDataCapturer {
public:
	AudioDeviceDataCapturer() = default;
	virtual ~AudioDeviceDataCapturer() = default;

	RESULT Initialize();

	RESULT BroadcastAudioPacket(const AudioPacket &audioPacket);

	RESULT SetAudioTransport(webrtc::AudioTransport* pAudioTransport);

public:
	webrtc::AudioTransport* m_pAudioTransport = nullptr;
	WebRTCAudioDeviceModule *m_pWebRTCAudioDeviceModule = nullptr;
};

// A wrapper over AudioDeviceModule that registers itself as AudioTransport
// callback and redirects the PCM data to AudioDeviceDataObserver callback.
class WebRTCAudioDeviceModule : 
	public webrtc::AudioDeviceModule, 
	public webrtc::AudioTransport
{

public:
	WebRTCAudioDeviceModule(const int32_t id, const AudioLayer audioLayer, AudioDeviceDataCapturer* pAudioDeviceCapturer);
	virtual ~WebRTCAudioDeviceModule();

	// Make sure we have a valid ADM before returning it to user.
	bool IsValid();
	RESULT Initialize();

	// RefCountedModule methods overrides.
	int64_t TimeUntilNextProcess() override;

	void Process() override;

	RESULT BroadcastAudioPacket(const AudioPacket &audioPacket);

	// AudioTransport methods overrides.
	int32_t RecordedDataIsAvailable(const void* audioSamples,
		const size_t nSamples,
		const size_t nBytesPerSample,
		const size_t nChannels,
		const uint32_t samples_per_sec,
		const uint32_t total_delay_ms,
		const int32_t clockDrift,
		const uint32_t currentMicLevel,
		const bool keyPressed,
		uint32_t& newMicLevel) override;

	int32_t NeedMorePlayData(const size_t nSamples,
		const size_t nBytesPerSample,
		const size_t nChannels,
		const uint32_t samples_per_sec,
		void* audioSamples,
		size_t& nSamplesOut,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms) override;

	void PushCaptureData(int voe_channel,
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames) override;

	void PullRenderData(int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames,
		void* audio_data,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms) override;

	// Override AudioDeviceModule's RegisterAudioCallback method to remember the
	// actual audio transport (e.g.: voice engine).
	int32_t RegisterAudioCallback(AudioTransport* pAudioCallback) override;

	// AudioDeviceModule pass through method overrides.
	int32_t ActiveAudioLayer(AudioLayer* audio_layer) const override;

	ErrorCode LastError() const override {
		return m_pAudioDeviceModuleImp->LastError();
	}

	int32_t RegisterEventObserver(webrtc::AudioDeviceObserver* event_callback) override;

	int32_t Init() override;

	int32_t Terminate() override;

	bool Initialized() const override;

	int16_t PlayoutDevices() override;

	int16_t RecordingDevices() override;

	int32_t PlayoutDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) override;

	int32_t RecordingDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) override;

	int32_t SetPlayoutDevice(uint16_t index) override;
	int32_t SetPlayoutDevice(WindowsDeviceType device) override;

	int32_t SetRecordingDevice(uint16_t index) override;
	int32_t SetRecordingDevice(WindowsDeviceType device) override;

	int32_t PlayoutIsAvailable(bool* available) override;
	int32_t InitPlayout() override;
	bool PlayoutIsInitialized() const override;

	int32_t RecordingIsAvailable(bool* available) override;
	int32_t InitRecording() override;
	bool RecordingIsInitialized() const override;

	int32_t StartPlayout() override;
	int32_t StopPlayout() override;
	bool Playing() const override;

	int32_t StartRecording() override;
	int32_t StopRecording() override;
	bool Recording() const override;

	int32_t SetAGC(bool enable) override;
	bool AGC() const override;

	int32_t SetWaveOutVolume(uint16_t volume_left, uint16_t volume_right) override;
	int32_t WaveOutVolume(uint16_t* volume_left, uint16_t* volume_right) const override;

	int32_t InitSpeaker() override;
	bool SpeakerIsInitialized() const override;

	int32_t InitMicrophone() override;
	bool MicrophoneIsInitialized() const override;

	int32_t SpeakerVolumeIsAvailable(bool* available) override;
	int32_t SetSpeakerVolume(uint32_t volume) override;
	int32_t SpeakerVolume(uint32_t* volume) const override;
	int32_t MaxSpeakerVolume(uint32_t* max_volume) const override;
	int32_t MinSpeakerVolume(uint32_t* min_volume) const override;
	int32_t SpeakerVolumeStepSize(uint16_t* step_size) const override;

	int32_t MicrophoneVolumeIsAvailable(bool* available) override;
	int32_t SetMicrophoneVolume(uint32_t volume) override;
	int32_t MicrophoneVolume(uint32_t* volume) const override;
	int32_t MaxMicrophoneVolume(uint32_t* max_volume) const override;
	int32_t MinMicrophoneVolume(uint32_t* min_volume) const override;
	int32_t MicrophoneVolumeStepSize(uint16_t* step_size) const override;

	int32_t SpeakerMuteIsAvailable(bool* available) override;
	int32_t SetSpeakerMute(bool enable) override;
	int32_t SpeakerMute(bool* enabled) const override;

	int32_t MicrophoneMuteIsAvailable(bool* available) override;
	int32_t SetMicrophoneMute(bool enable) override;
	int32_t MicrophoneMute(bool* enabled) const override;
	int32_t MicrophoneBoostIsAvailable(bool* available) override;
	int32_t SetMicrophoneBoost(bool enable) override;
	int32_t MicrophoneBoost(bool* enabled) const override;

	int32_t StereoPlayoutIsAvailable(bool* available) const override;
	int32_t SetStereoPlayout(bool enable) override;
	int32_t StereoPlayout(bool* enabled) const override;
	int32_t StereoRecordingIsAvailable(bool* available) const override;
	int32_t SetStereoRecording(bool enable) override;
	int32_t StereoRecording(bool* enabled) const override;

	int32_t SetRecordingChannel(const ChannelType channel) override;
	int32_t RecordingChannel(ChannelType* channel) const override;

	int32_t SetPlayoutBuffer(const BufferType type, uint16_t size_ms) override;
	int32_t PlayoutBuffer(BufferType* type, uint16_t* size_ms) const override;

	int32_t PlayoutDelay(uint16_t* delay_ms) const override;
	int32_t RecordingDelay(uint16_t* delay_ms) const override;

	int32_t CPULoad(uint16_t* load) const override;

	int32_t StartRawOutputFileRecording(const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) override;
	int32_t StopRawOutputFileRecording() override;

	int32_t StartRawInputFileRecording(const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) override;
	int32_t StopRawInputFileRecording() override;

	int32_t SetRecordingSampleRate(const uint32_t samples_per_sec) override;
	int32_t RecordingSampleRate(uint32_t* samples_per_sec) const override;

	int32_t SetPlayoutSampleRate(const uint32_t samples_per_sec) override;
	int32_t PlayoutSampleRate(uint32_t* samples_per_sec) const override;

	int32_t ResetAudioDevice() override;

	int32_t SetLoudspeakerStatus(bool enable) override;
	int32_t GetLoudspeakerStatus(bool* enabled) const override;

	bool BuiltInAECIsAvailable() const override;
	bool BuiltInAGCIsAvailable() const override;
	bool BuiltInNSIsAvailable() const override;

	int32_t EnableBuiltInAEC(bool enable) override;
	int32_t EnableBuiltInAGC(bool enable) override;
	int32_t EnableBuiltInNS(bool enable) override;

	// Only supported on iOS.
#if defined(WEBRTC_IOS)
	int GetPlayoutAudioParameters(AudioParameters* params) const override;
	int GetRecordAudioParameters(AudioParameters* params) const override;
#endif  // WEBRTC_IOS

protected:
	rtc::scoped_refptr<AudioDeviceModule> m_pAudioDeviceModuleImp = nullptr;
	AudioDeviceDataCapturer* m_pAudioDeviceCapturer = nullptr;
	AudioTransport* m_pAudioTransport = nullptr;
	bool m_fValid = false;

	SoundBuffer *m_pPendingSoundBuffer = nullptr;
};

// Creates an ADM instance with AudioDeviceDataObserver registered.
rtc::scoped_refptr<webrtc::AudioDeviceModule> CreateAudioDeviceWithDataCapturer(
	const int32_t id,
	const webrtc::AudioDeviceModule::AudioLayer audio_layer,
	AudioDeviceDataCapturer* pAudioDeviceDataCapturer);



#endif // WEBRTC_AUDIO_CAPTURE_DEVICE_H_
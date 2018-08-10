#ifndef WEBRTC_AUDIO_CAPTURE_DEVICE_H_
#define WEBRTC_AUDIO_CAPTURE_DEVICE_H_	
#pragma once

// The above had to be changed due to an already existing WEBRTC_COMMON_H_ in the WebRTC code namespace

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCAudioCaptureDevice.h
// A customer WebRTC Audio Capture Device

#include <queue>

#include "modules/audio_device/include/audio_device_defines.h"
#include "modules/audio_device/include/audio_device.h"
//#include "webrtc/rtc_base/scoped_ref_ptr.h"

#include "Sound/AudioPacket.h"
#include "Sound/SoundBuffer.h"

// A wrapper over AudioDeviceModule that registers itself as AudioTransport
// callback and redirects the PCM data to AudioDeviceDataObserver callback.
class WebRTCAudioDeviceModule :  public webrtc::AudioDeviceModule {

public:

	WebRTCAudioDeviceModule();
	~WebRTCAudioDeviceModule();

	// Make sure we have a valid ADM before returning it to user.
	bool IsValid();

	RESULT Initialize();

	float GetRunTimeMicAverage();

	int32_t RegisterAudioCallback(webrtc::AudioTransport* pAudioCallback) override;

	// AudioDeviceModule pass through method overrides.
	int32_t ActiveAudioLayer(AudioLayer* audio_layer) const override;

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

	int32_t InitSpeaker() override;
	bool SpeakerIsInitialized() const override;

	int32_t InitMicrophone() override;
	bool MicrophoneIsInitialized() const override;

	int32_t SpeakerVolumeIsAvailable(bool* available) override;
	int32_t SetSpeakerVolume(uint32_t volume) override;
	int32_t SpeakerVolume(uint32_t* volume) const override;
	int32_t MaxSpeakerVolume(uint32_t* max_volume) const override;
	int32_t MinSpeakerVolume(uint32_t* min_volume) const override;

	int32_t MicrophoneVolumeIsAvailable(bool* available) override;
	int32_t SetMicrophoneVolume(uint32_t volume) override;
	int32_t MicrophoneVolume(uint32_t* volume) const override;
	int32_t MaxMicrophoneVolume(uint32_t* max_volume) const override;
	int32_t MinMicrophoneVolume(uint32_t* min_volume) const override;

	int32_t SpeakerMuteIsAvailable(bool* available) override;
	int32_t SetSpeakerMute(bool enable) override;
	int32_t SpeakerMute(bool* enabled) const override;

	int32_t MicrophoneMuteIsAvailable(bool* available) override;
	int32_t SetMicrophoneMute(bool enable) override;
	int32_t MicrophoneMute(bool* enabled) const override;

	int32_t StereoPlayoutIsAvailable(bool* available) const override;
	int32_t SetStereoPlayout(bool enable) override;
	int32_t StereoPlayout(bool* enabled) const override;
	int32_t StereoRecordingIsAvailable(bool* available) const override;
	int32_t SetStereoRecording(bool enable) override;
	int32_t StereoRecording(bool* enabled) const override;

	int32_t PlayoutDelay(uint16_t* delay_ms) const override;

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

private:
	// Audio Process
	RESULT WebRTCADMProcess();
	std::thread	m_webrtcADMThread;
	bool m_fRunning = false;

protected:
	webrtc::AudioTransport* m_pAudioTransport = nullptr;
	

	bool m_fValid = false;
	bool m_fInitialized = false;
	bool m_fPlaying = true;
	bool m_fRecording = true;
	int m_msOutputDelay = 0;
	
	//std::queue<AudioPacket> m_pendingAudioPackets;
	//SoundBuffer *m_pPendingSoundBuffer = nullptr;

	std::mutex m_pendingBufferLock;
	CircularBuffer<int16_t> m_pendingAudioCircularBuffer;
	
	float m_runTimeAvgMicValue = 0.0f;
	float m_runTimeAvgFilterRatio = 0.95f;
};

#endif // WEBRTC_AUDIO_CAPTURE_DEVICE_H_
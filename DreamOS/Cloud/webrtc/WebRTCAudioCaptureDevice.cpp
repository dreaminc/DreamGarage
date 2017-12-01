#include "WebRTCAudioCaptureDevice.h"

#include "webrtc/base/refcount.h"
#include "webrtc/base/checks.h"

#define WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
#include "webrtc/modules/audio_device/audio_device_impl.h"

RESULT AudioDeviceDataCapturer::Initialize() {
	//m_pendingAudioPackets = std::queue<AudioPacket>();
	return R_PASS;
}

RESULT AudioDeviceDataCapturer::BroadcastAudioPacket(const AudioPacket audioPacket) {
	RESULT r = R_PASS;

	if (m_pAudioTransport != nullptr) {
		DEBUG_LINEOUT("ADDC: PushCaptureData: %d", (int)(audioPacket.GetNumFrames()));


		//m_pAudioTransport->PushCaptureData(
		//	0,
		//	audioPacket.GetDataBuffer(),
		//	audioPacket.GetBitsPerSample(),
		//	audioPacket.GetSamplingRate(),
		//	audioPacket.GetNumChannels(),
		//	audioPacket.GetNumFrames()
		//);

		int kClockDriftMs = 0;
		uint32_t newMicLevel = 0;
		
		///*
		int samples_per_sec = 44100;
		int nSamples = 441;
		static int count = 0;
		int16_t *pDataBuffer = new int16_t[nSamples];

		for (int i = 0; i < nSamples; i++) {
			pDataBuffer[i] = sin((count*4200.0f) / samples_per_sec) * 30000;
			count++;
		}
		
		int32_t res = m_pAudioTransport->RecordedDataIsAvailable(
			pDataBuffer, 
			nSamples,
			sizeof(int16_t), 
			1, 
			samples_per_sec,
			kClockDriftMs, 0,
			255, 
			false, 
			newMicLevel);
		
		//*/

		/*
		int32_t res = m_pAudioTransport->RecordedDataIsAvailable(
			audioPacket.GetDataBuffer(),
			audioPacket.GetNumFrames(),
			audioPacket.GetBytesPerSample(),
			audioPacket.GetNumChannels(),
			audioPacket.GetSamplingRate(),
			kClockDriftMs, 0,
			255,
			false,
			newMicLevel);
			*/
	}
	

	///Error:
	return r;
}

RESULT AudioDeviceDataCapturer::SetAudioTransport(webrtc::AudioTransport* pAudioTransport) {
	m_pAudioTransport = pAudioTransport;
	return R_PASS;
}

// A wrapper over AudioDeviceModule that registers itself as AudioTransport
// callback and redirects the PCM data to AudioDeviceDataObserver callback.
class ADMWrapper : 
	public webrtc::AudioDeviceModule, 
	public webrtc::AudioTransport 
{

public:
	ADMWrapper(const int32_t id, const AudioLayer audioLayer, AudioDeviceDataCapturer* pAudioDeviceCapturer) : 
		m_pAudioDeviceModuleImp(AudioDeviceModule::Create(id, audioLayer)), 
		m_pAudioDeviceCapturer(pAudioDeviceCapturer) 
	{
		pAudioDeviceCapturer->Initialize();

		// Register self as the audio transport callback for underlying ADM impl.
		auto res = m_pAudioDeviceModuleImp->RegisterAudioCallback(this);

		m_fValid = (m_pAudioDeviceModuleImp.get() != nullptr) && (res == 0);
	}

	virtual ~ADMWrapper() {
		m_pAudioTransport = nullptr;
		m_pAudioDeviceCapturer = nullptr;
	}

	// Make sure we have a valid ADM before returning it to user.
	bool IsValid() { 
		return m_fValid; 
	}

	// RefCountedModule methods overrides.
	int64_t TimeUntilNextProcess() override {
		return m_pAudioDeviceModuleImp->TimeUntilNextProcess();
	}

	void Process() override { 
		m_pAudioDeviceModuleImp->Process();
	}

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
		uint32_t& newMicLevel) override
	{
		int32_t res = 0;
		
		//DEBUG_LINEOUT("Recorded Data is Avail: %d", (int)(nSamples));

		//static int count = 0;
		//int16_t *pDataBuffer = new int16_t[nSamples];
		//for (int i = 0; i < nSamples; i++) {
		//	pDataBuffer[i] = sin((count*4200.0f) / samples_per_sec) * 30000;
		//	count++;
		//}
		//
		//// Send to the actual audio transport (this part makes soundz)
		//if (m_pAudioTransport) {
		//	res = m_pAudioTransport->RecordedDataIsAvailable(
		//		pDataBuffer, nSamples, sizeof(int16_t), 1, samples_per_sec,
		//		0, clockDrift, currentMicLevel, keyPressed, newMicLevel);
		//}

		//// Send to the actual audio transport (this part makes soundz)

		// Test mixing in a signal
		///*
		static double theta = 0.0f;
		static double freq = 440.0f;

		int16_t *pDataBuffer = (int16_t*)audioSamples;
		if (pDataBuffer != nullptr) {
			for (int i = 0; i < nSamples * nChannels; i++) {
				float val = sin(theta);
				//val *= 0.25f;

				for (int j = 0; j < nChannels; j++) {
					pDataBuffer[i + j] += (int16_t)(val * 10000.0f);
				}

				// increment theta
				theta += ((2.0f * M_PI) / (double)(samples_per_sec)) * freq;
				if (theta >= 2.0f * M_PI) {
					theta = theta - (2.0f * M_PI);
				}
			}
		}

		if (m_pAudioTransport) {
			res = m_pAudioTransport->RecordedDataIsAvailable(
				audioSamples, 
				nSamples, 
				nBytesPerSample, 
				nChannels, 
				samples_per_sec,
				total_delay_ms, 
				clockDrift, 
				currentMicLevel, 
				keyPressed, 
				newMicLevel);
		}
		//*/

		return res;
	}

	int32_t NeedMorePlayData(const size_t nSamples,
		const size_t nBytesPerSample,
		const size_t nChannels,
		const uint32_t samples_per_sec,
		void* audioSamples,
		size_t& nSamplesOut,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms) override 
	{
		int32_t res = 0;

		DEBUG_LINEOUT("NeedMorePlayData: %d", (int)(nSamples));

		// Request data from audio transport (this part gets sound)
		if (m_pAudioTransport) {
			res = m_pAudioTransport->NeedMorePlayData(
				nSamples, nBytesPerSample, nChannels, samples_per_sec, audioSamples,
				nSamplesOut, elapsed_time_ms, ntp_time_ms);
		}

		return res;
	}

	void PushCaptureData(int voe_channel,
						 const void* audio_data,
						 int bits_per_sample,
						 int sample_rate,
						 size_t number_of_channels,
						 size_t number_of_frames) override
	{
		//int32_t res = 0;

		DEBUG_LINEOUT("PushCaptureData: %d", (int)(number_of_frames));

		// Capture PCM data of locally captured audio.
		//if (m_pAudioDeviceCapturer) {
		//	m_pAudioDeviceCapturer->PushCaptureData(voe_channel, 
		//											audio_data, 
		//											bits_per_sample, 
		//											sample_rate,
		//											number_of_channels, 
		//											number_of_frames);
		//}
		
		RTC_NOTREACHED();

		//if (m_pAudioTransport) {
		//	m_pAudioTransport->PushCaptureData(voe_channel,
		//											 audio_data,
		//											 bits_per_sample, 
		//											 sample_rate, 
		//											 number_of_channels, 
		//											 number_of_frames);
		//}
	}

	void PullRenderData(int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames,
		void* audio_data,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms) override
	{
		DEBUG_LINEOUT("PullRenderData: %d", (int)(number_of_frames));

		RTC_NOTREACHED();

	}

	// Override AudioDeviceModule's RegisterAudioCallback method to remember the
	// actual audio transport (e.g.: voice engine).
	int32_t RegisterAudioCallback(AudioTransport* pAudioCallback) override {
		// Remember the audio callback to forward PCM data
		m_pAudioTransport = pAudioCallback;
		m_pAudioDeviceCapturer->SetAudioTransport(m_pAudioTransport);

		return 0;
	}

	// AudioDeviceModule pass through method overrides.
	int32_t ActiveAudioLayer(AudioLayer* audio_layer) const override {
		return m_pAudioDeviceModuleImp->ActiveAudioLayer(audio_layer);
	}

	ErrorCode LastError() const override { 
		return m_pAudioDeviceModuleImp->LastError(); 
	}

	int32_t RegisterEventObserver(webrtc::AudioDeviceObserver* event_callback) override {
		return m_pAudioDeviceModuleImp->RegisterEventObserver(event_callback);
	}

	int32_t Init() override { 
		return m_pAudioDeviceModuleImp->Init(); 
	}

	int32_t Terminate() override { 
		return m_pAudioDeviceModuleImp->Terminate(); 
	}
	
	bool Initialized() const override { 
		return m_pAudioDeviceModuleImp->Initialized(); 
	}
	
	int16_t PlayoutDevices() override { 
		return m_pAudioDeviceModuleImp->PlayoutDevices(); 
	}
	
	int16_t RecordingDevices() override { 
		return m_pAudioDeviceModuleImp->RecordingDevices(); 
	}
	
	int32_t PlayoutDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) override {
		return m_pAudioDeviceModuleImp->PlayoutDeviceName(index, name, guid);
	}
	
	int32_t RecordingDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) override {
		return m_pAudioDeviceModuleImp->RecordingDeviceName(index, name, guid);
	}
	
	int32_t SetPlayoutDevice(uint16_t index) override {
		return m_pAudioDeviceModuleImp->SetPlayoutDevice(index);
	}
	
	int32_t SetPlayoutDevice(WindowsDeviceType device) override {
		return m_pAudioDeviceModuleImp->SetPlayoutDevice(device);
	}
	
	int32_t SetRecordingDevice(uint16_t index) override {
		return m_pAudioDeviceModuleImp->SetRecordingDevice(index);
	}
	
	int32_t SetRecordingDevice(WindowsDeviceType device) override {
		return m_pAudioDeviceModuleImp->SetRecordingDevice(device);
	}
	
	int32_t PlayoutIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->PlayoutIsAvailable(available);
	}
	
	int32_t InitPlayout() override { 
		return m_pAudioDeviceModuleImp->InitPlayout(); 
	}
	
	bool PlayoutIsInitialized() const override {
		return m_pAudioDeviceModuleImp->PlayoutIsInitialized();
	}
	
	int32_t RecordingIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->RecordingIsAvailable(available);
	}
	
	int32_t InitRecording() override { return m_pAudioDeviceModuleImp->InitRecording(); }
	
	bool RecordingIsInitialized() const override {
		return m_pAudioDeviceModuleImp->RecordingIsInitialized();
	}
	
	int32_t StartPlayout() override { 
		return m_pAudioDeviceModuleImp->StartPlayout(); 
	}
	
	int32_t StopPlayout() override { 
		return m_pAudioDeviceModuleImp->StopPlayout(); 
	}
	
	bool Playing() const override { 
		return m_pAudioDeviceModuleImp->Playing(); 
	}
	
	int32_t StartRecording() override { 
		return m_pAudioDeviceModuleImp->StartRecording(); 
	}
	
	int32_t StopRecording() override { 
		return m_pAudioDeviceModuleImp->StopRecording(); 
	}
	
	bool Recording() const override { 
		return m_pAudioDeviceModuleImp->Recording(); 
	}
	
	int32_t SetAGC(bool enable) override { 
		return m_pAudioDeviceModuleImp->SetAGC(enable); 
	}
	
	bool AGC() const override { 
		return m_pAudioDeviceModuleImp->AGC(); 
	}
	
	int32_t SetWaveOutVolume(uint16_t volume_left, uint16_t volume_right) override {
		return m_pAudioDeviceModuleImp->SetWaveOutVolume(volume_left, volume_right);
	}
	
	int32_t WaveOutVolume(uint16_t* volume_left, uint16_t* volume_right) const override {
		return m_pAudioDeviceModuleImp->WaveOutVolume(volume_left, volume_right);
	}
	
	int32_t InitSpeaker() override { 
		return m_pAudioDeviceModuleImp->InitSpeaker(); 
	}
	
	bool SpeakerIsInitialized() const override {
		return m_pAudioDeviceModuleImp->SpeakerIsInitialized();
	}
	
	int32_t InitMicrophone() override { 
		return m_pAudioDeviceModuleImp->InitMicrophone(); 
	}
	
	bool MicrophoneIsInitialized() const override {
		return m_pAudioDeviceModuleImp->MicrophoneIsInitialized();
	}
	
	int32_t SpeakerVolumeIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->SpeakerVolumeIsAvailable(available);
	}
	
	int32_t SetSpeakerVolume(uint32_t volume) override {
		return m_pAudioDeviceModuleImp->SetSpeakerVolume(volume);
	}
	
	int32_t SpeakerVolume(uint32_t* volume) const override {
		return m_pAudioDeviceModuleImp->SpeakerVolume(volume);
	}
	
	int32_t MaxSpeakerVolume(uint32_t* max_volume) const override {
		return m_pAudioDeviceModuleImp->MaxSpeakerVolume(max_volume);
	}
	
	int32_t MinSpeakerVolume(uint32_t* min_volume) const override {
		return m_pAudioDeviceModuleImp->MinSpeakerVolume(min_volume);
	}
	
	int32_t SpeakerVolumeStepSize(uint16_t* step_size) const override {
		return m_pAudioDeviceModuleImp->SpeakerVolumeStepSize(step_size);
	}
	
	int32_t MicrophoneVolumeIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->MicrophoneVolumeIsAvailable(available);
	}
	
	int32_t SetMicrophoneVolume(uint32_t volume) override {
		return m_pAudioDeviceModuleImp->SetMicrophoneVolume(volume);
	}
	
	int32_t MicrophoneVolume(uint32_t* volume) const override {
		return m_pAudioDeviceModuleImp->MicrophoneVolume(volume);
	}
	
	int32_t MaxMicrophoneVolume(uint32_t* max_volume) const override {
		return m_pAudioDeviceModuleImp->MaxMicrophoneVolume(max_volume);
	}
	
	int32_t MinMicrophoneVolume(uint32_t* min_volume) const override {
		return m_pAudioDeviceModuleImp->MinMicrophoneVolume(min_volume);
	}
	
	int32_t MicrophoneVolumeStepSize(uint16_t* step_size) const override {
		return m_pAudioDeviceModuleImp->MicrophoneVolumeStepSize(step_size);
	}
	
	int32_t SpeakerMuteIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->SpeakerMuteIsAvailable(available);
	}
	
	int32_t SetSpeakerMute(bool enable) override {
		return m_pAudioDeviceModuleImp->SetSpeakerMute(enable);
	}
	
	int32_t SpeakerMute(bool* enabled) const override {
		return m_pAudioDeviceModuleImp->SpeakerMute(enabled);
	}
	
	int32_t MicrophoneMuteIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->MicrophoneMuteIsAvailable(available);
	}
	
	int32_t SetMicrophoneMute(bool enable) override {
		return m_pAudioDeviceModuleImp->SetMicrophoneMute(enable);
	}
	
	int32_t MicrophoneMute(bool* enabled) const override {
		return m_pAudioDeviceModuleImp->MicrophoneMute(enabled);
	}
	
	int32_t MicrophoneBoostIsAvailable(bool* available) override {
		return m_pAudioDeviceModuleImp->MicrophoneBoostIsAvailable(available);
	}
	
	int32_t SetMicrophoneBoost(bool enable) override {
		return m_pAudioDeviceModuleImp->SetMicrophoneBoost(enable);
	}
	
	int32_t MicrophoneBoost(bool* enabled) const override {
		return m_pAudioDeviceModuleImp->MicrophoneBoost(enabled);
	}
	
	int32_t StereoPlayoutIsAvailable(bool* available) const override {
		return m_pAudioDeviceModuleImp->StereoPlayoutIsAvailable(available);
	}
	
	int32_t SetStereoPlayout(bool enable) override {
		return m_pAudioDeviceModuleImp->SetStereoPlayout(enable);
	}
	
	int32_t StereoPlayout(bool* enabled) const override {
		return m_pAudioDeviceModuleImp->StereoPlayout(enabled);
	}
	
	int32_t StereoRecordingIsAvailable(bool* available) const override {
		return m_pAudioDeviceModuleImp->StereoRecordingIsAvailable(available);
	}
	
	int32_t SetStereoRecording(bool enable) override {
		return m_pAudioDeviceModuleImp->SetStereoRecording(enable);
	}
	
	int32_t StereoRecording(bool* enabled) const override {
		return m_pAudioDeviceModuleImp->StereoRecording(enabled);
	}
	
	int32_t SetRecordingChannel(const ChannelType channel) override {
		return m_pAudioDeviceModuleImp->SetRecordingChannel(channel);
	}
	
	int32_t RecordingChannel(ChannelType* channel) const override {
		return m_pAudioDeviceModuleImp->RecordingChannel(channel);
	}
	
	int32_t SetPlayoutBuffer(const BufferType type, uint16_t size_ms) override {
		return m_pAudioDeviceModuleImp->SetPlayoutBuffer(type, size_ms);
	}
	
	int32_t PlayoutBuffer(BufferType* type, uint16_t* size_ms) const override {
		return m_pAudioDeviceModuleImp->PlayoutBuffer(type, size_ms);
	}
	
	int32_t PlayoutDelay(uint16_t* delay_ms) const override {
		return m_pAudioDeviceModuleImp->PlayoutDelay(delay_ms);
	}
	
	int32_t RecordingDelay(uint16_t* delay_ms) const override {
		return m_pAudioDeviceModuleImp->RecordingDelay(delay_ms);
	}
	
	int32_t CPULoad(uint16_t* load) const override {
		return m_pAudioDeviceModuleImp->CPULoad(load);
	}
	
	int32_t StartRawOutputFileRecording(const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) override { 
		return m_pAudioDeviceModuleImp->StartRawOutputFileRecording(pcm_file_name_utf8);
	}
	
	int32_t StopRawOutputFileRecording() override {
		return m_pAudioDeviceModuleImp->StopRawOutputFileRecording();
	}
	
	int32_t StartRawInputFileRecording(const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) override {
		return m_pAudioDeviceModuleImp->StartRawInputFileRecording(pcm_file_name_utf8);
	}
	
	int32_t StopRawInputFileRecording() override {
		return m_pAudioDeviceModuleImp->StopRawInputFileRecording();
	}
	
	int32_t SetRecordingSampleRate(const uint32_t samples_per_sec) override {
		return m_pAudioDeviceModuleImp->SetRecordingSampleRate(samples_per_sec);
	}
	
	int32_t RecordingSampleRate(uint32_t* samples_per_sec) const override {
		return m_pAudioDeviceModuleImp->RecordingSampleRate(samples_per_sec);
	}
	
	int32_t SetPlayoutSampleRate(const uint32_t samples_per_sec) override {
		return m_pAudioDeviceModuleImp->SetPlayoutSampleRate(samples_per_sec);
	}
	
	int32_t PlayoutSampleRate(uint32_t* samples_per_sec) const override {
		return m_pAudioDeviceModuleImp->PlayoutSampleRate(samples_per_sec);
	}
	
	int32_t ResetAudioDevice() override { 
		return m_pAudioDeviceModuleImp->ResetAudioDevice(); 
	}
	
	int32_t SetLoudspeakerStatus(bool enable) override {
		return m_pAudioDeviceModuleImp->SetLoudspeakerStatus(enable);
	}
	
	int32_t GetLoudspeakerStatus(bool* enabled) const override {
		return m_pAudioDeviceModuleImp->GetLoudspeakerStatus(enabled);
	}
	
	bool BuiltInAECIsAvailable() const override {
		return m_pAudioDeviceModuleImp->BuiltInAECIsAvailable();
	}
	
	bool BuiltInAGCIsAvailable() const override {
		return m_pAudioDeviceModuleImp->BuiltInAGCIsAvailable();
	}
	
	bool BuiltInNSIsAvailable() const override {
		return m_pAudioDeviceModuleImp->BuiltInNSIsAvailable();
	}
	
	int32_t EnableBuiltInAEC(bool enable) override {
		return m_pAudioDeviceModuleImp->EnableBuiltInAEC(enable);
	}
	
	int32_t EnableBuiltInAGC(bool enable) override {
		return m_pAudioDeviceModuleImp->EnableBuiltInAGC(enable);
	}
	
	int32_t EnableBuiltInNS(bool enable) override {
		return m_pAudioDeviceModuleImp->EnableBuiltInNS(enable);
	}
	
	// Only supported on iOS.
#if defined(WEBRTC_IOS)
	int GetPlayoutAudioParameters(AudioParameters* params) const override {
		return m_pAudioDeviceModuleImp->GetPlayoutAudioParameters(params);
	}
	int GetRecordAudioParameters(AudioParameters* params) const override {
		return m_pAudioDeviceModuleImp->GetRecordAudioParameters(params);
	}
#endif  // WEBRTC_IOS

protected:
	rtc::scoped_refptr<AudioDeviceModule> m_pAudioDeviceModuleImp = nullptr;
	AudioDeviceDataCapturer* m_pAudioDeviceCapturer = nullptr;
	AudioTransport* m_pAudioTransport = nullptr;
	bool m_fValid = false;
};


rtc::scoped_refptr<webrtc::AudioDeviceModule> 
	CreateAudioDeviceWithDataCapturer(const int32_t id,
									  const webrtc::AudioDeviceModule::AudioLayer audio_layer, 
									  AudioDeviceDataCapturer* pAudioDeviceCapturer) 
{
	rtc::scoped_refptr<ADMWrapper> pAudioDeviceWrapper(new rtc::RefCountedObject<ADMWrapper>(id, audio_layer, pAudioDeviceCapturer));

	if (!pAudioDeviceWrapper->IsValid()) {
		return nullptr;
	}

	return pAudioDeviceWrapper;
}
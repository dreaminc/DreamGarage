#include "WebRTCAudioDeviceModule.h"

#include "webrtc/base/refcount.h"
#include "webrtc/base/checks.h"

#define WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
#include "webrtc/modules/audio_device/audio_device_impl.h"

RESULT AudioDeviceDataCapturer::Initialize() {
	//m_pendingAudioPackets = std::queue<AudioPacket>();
	return R_PASS;
}

// TODO: AudioDeviceDataCapturer should be ixnays
RESULT AudioDeviceDataCapturer::BroadcastAudioPacket(const AudioPacket &audioPacket) {
	RESULT r = R_PASS;

	CR(m_pWebRTCAudioDeviceModule->BroadcastAudioPacket(audioPacket));

Error:
	return r;
}

RESULT AudioDeviceDataCapturer::SetAudioTransport(webrtc::AudioTransport* pAudioTransport) {
	m_pAudioTransport = pAudioTransport;
	return R_PASS;
}

rtc::scoped_refptr<webrtc::AudioDeviceModule> 
	CreateAudioDeviceWithDataCapturer(const int32_t id,
									  const webrtc::AudioDeviceModule::AudioLayer audio_layer, 
									  AudioDeviceDataCapturer* pAudioDeviceCapturer) 
{
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCAudioDeviceModule> pWebRTCAudioDeviceModule(new rtc::RefCountedObject<WebRTCAudioDeviceModule>(id, audio_layer, pAudioDeviceCapturer));
	CN(pWebRTCAudioDeviceModule);

	CBM((pWebRTCAudioDeviceModule->IsValid()), "ADM not valid");
	CRM(pWebRTCAudioDeviceModule->Initialize(), "Failed to initialize WebRTCAudioDeviceModule");

	// TODO: Temp
	// This whole flow should be removed, capturer is not necessary (just this factory method)
	pAudioDeviceCapturer->m_pWebRTCAudioDeviceModule = pWebRTCAudioDeviceModule.get();

	return pWebRTCAudioDeviceModule;

Error:
	if (pWebRTCAudioDeviceModule) {
		pWebRTCAudioDeviceModule = nullptr;
	}

	return nullptr;
}

// WebRTCAudioDeviceModule

WebRTCAudioDeviceModule::WebRTCAudioDeviceModule(const int32_t id, const AudioLayer audioLayer, AudioDeviceDataCapturer* pAudioDeviceCapturer) :
	m_pAudioDeviceModuleImp(AudioDeviceModule::Create(id, audioLayer)),
	m_pAudioDeviceCapturer(pAudioDeviceCapturer)
{
	pAudioDeviceCapturer->Initialize();

	// Register self as the audio transport callback for underlying ADM impl.
	auto res = m_pAudioDeviceModuleImp->RegisterAudioCallback(this);

	m_fValid = (m_pAudioDeviceModuleImp.get() != nullptr) && (res == 0);

	
}

WebRTCAudioDeviceModule::~WebRTCAudioDeviceModule() {
	m_pAudioTransport = nullptr;
	m_pAudioDeviceCapturer = nullptr;
}

RESULT WebRTCAudioDeviceModule::Initialize() {
	RESULT r = R_PASS;

	// Sound Buffer
	m_pPendingSoundBuffer = SoundBuffer::Make(2, SoundBuffer::type::SIGNED_16_BIT);
	CN(m_pPendingSoundBuffer);

Error:
	return r;
}

// Make sure we have a valid ADM before returning it to user.
bool WebRTCAudioDeviceModule::IsValid() {
	return m_fValid;
}

// RefCountedModule methods overrides.
int64_t WebRTCAudioDeviceModule::TimeUntilNextProcess()  {
	return m_pAudioDeviceModuleImp->TimeUntilNextProcess();
}

// TODO: Perhaps we want to jump in here?
void WebRTCAudioDeviceModule::Process()  {
	m_pAudioDeviceModuleImp->Process();
}

RESULT WebRTCAudioDeviceModule::BroadcastAudioPacket(const AudioPacket &audioPacket) {
	RESULT r = R_PASS;

	///*
	// TEST: Fake audio output

	int samples_per_sec = 44100;
	int nFrames = audioPacket.GetNumFrames();
	int channels = 1;

	static double theta = 0.0f;
	double freq = 440.0f;

	int16_t *pDataBuffer = nullptr;

	if (pDataBuffer == nullptr) {
		pDataBuffer = new int16_t[nFrames * channels];

		for (int i = 0; i < nFrames * channels; i++) {
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
	//*/

	m_pPendingSoundBuffer->LockBuffer();
	{
		if (m_pPendingSoundBuffer->IsFull() == false) {
			CR(m_pPendingSoundBuffer->PushData(pDataBuffer, nFrames));
		}
		else {
			DEBUG_LINEOUT("Pending buffer is full");
		}
	}
	m_pPendingSoundBuffer->UnlockBuffer();

Error:
	return r;
}

// AudioTransport methods overrides.
int32_t WebRTCAudioDeviceModule::RecordedDataIsAvailable(const void* audioSamples,
	const size_t nSamples,
	const size_t nBytesPerSample,
	const size_t nChannels,
	const uint32_t samples_per_sec,
	const uint32_t total_delay_ms,
	const int32_t clockDrift,
	const uint32_t currentMicLevel,
	const bool keyPressed,
	uint32_t& newMicLevel) 
{
	int32_t res = 0;

	//// Send to the actual audio transport (this part makes soundz)

	// Test mixing in a signal
	/*
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
	//*/

	int readBytes = 0;
	m_pPendingSoundBuffer->LockBuffer();
	{
		if ((readBytes = (int)m_pPendingSoundBuffer->NumPendingBytes()) > 0) {
			int16_t *pDataBuffer = (int16_t*)(audioSamples);
			RESULT r = m_pPendingSoundBuffer->MixIntoInterlacedTargetBuffer(pDataBuffer, (int)nSamples);
			if(r < 0) DEBUG_LINEOUT("Failed to mix in pending values");
		}
	}
	m_pPendingSoundBuffer->UnlockBuffer();

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

	return res;
}

int32_t WebRTCAudioDeviceModule::NeedMorePlayData(const size_t nSamples,
	const size_t nBytesPerSample,
	const size_t nChannels,
	const uint32_t samples_per_sec,
	void* audioSamples,
	size_t& nSamplesOut,
	int64_t* elapsed_time_ms,
	int64_t* ntp_time_ms) 
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

void WebRTCAudioDeviceModule::PushCaptureData(int voe_channel,
	const void* audio_data,
	int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames) 
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

void WebRTCAudioDeviceModule::PullRenderData(int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames,
	void* audio_data,
	int64_t* elapsed_time_ms,
	int64_t* ntp_time_ms) 
{
	DEBUG_LINEOUT("PullRenderData: %d", (int)(number_of_frames));

	RTC_NOTREACHED();

}

// Override AudioDeviceModule's RegisterAudioCallback method to remember the
// actual audio transport (e.g.: voice engine).
int32_t WebRTCAudioDeviceModule::RegisterAudioCallback(AudioTransport* pAudioCallback)  {
	// Remember the audio callback to forward PCM data
	m_pAudioTransport = pAudioCallback;
	m_pAudioDeviceCapturer->SetAudioTransport(m_pAudioTransport);

	return 0;
}

// AudioDeviceModule pass through method overrides.
int32_t WebRTCAudioDeviceModule::ActiveAudioLayer(AudioLayer* audio_layer) const  {
	return m_pAudioDeviceModuleImp->ActiveAudioLayer(audio_layer);
}

int32_t WebRTCAudioDeviceModule::RegisterEventObserver(webrtc::AudioDeviceObserver* event_callback)  {
	return m_pAudioDeviceModuleImp->RegisterEventObserver(event_callback);
}

int32_t WebRTCAudioDeviceModule::Init()  {
	return m_pAudioDeviceModuleImp->Init();
}

int32_t WebRTCAudioDeviceModule::Terminate()  {
	return m_pAudioDeviceModuleImp->Terminate();
}

bool WebRTCAudioDeviceModule::Initialized() const  {
	return m_pAudioDeviceModuleImp->Initialized();
}

int16_t WebRTCAudioDeviceModule::PlayoutDevices()  {
	return m_pAudioDeviceModuleImp->PlayoutDevices();
}

int16_t WebRTCAudioDeviceModule::RecordingDevices()  {
	return m_pAudioDeviceModuleImp->RecordingDevices();
}

int32_t WebRTCAudioDeviceModule::PlayoutDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize])  {
	return m_pAudioDeviceModuleImp->PlayoutDeviceName(index, name, guid);
}

int32_t WebRTCAudioDeviceModule::RecordingDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize])  {
	return m_pAudioDeviceModuleImp->RecordingDeviceName(index, name, guid);
}

int32_t WebRTCAudioDeviceModule::SetPlayoutDevice(uint16_t index)  {
	return m_pAudioDeviceModuleImp->SetPlayoutDevice(index);
}

int32_t WebRTCAudioDeviceModule::SetPlayoutDevice(WindowsDeviceType device)  {
	return m_pAudioDeviceModuleImp->SetPlayoutDevice(device);
}

int32_t WebRTCAudioDeviceModule::SetRecordingDevice(uint16_t index)  {
	return m_pAudioDeviceModuleImp->SetRecordingDevice(index);
}

int32_t WebRTCAudioDeviceModule::SetRecordingDevice(WindowsDeviceType device)  {
	return m_pAudioDeviceModuleImp->SetRecordingDevice(device);
}

int32_t WebRTCAudioDeviceModule::PlayoutIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->PlayoutIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::InitPlayout()  {
	return m_pAudioDeviceModuleImp->InitPlayout();
}

bool WebRTCAudioDeviceModule::PlayoutIsInitialized() const  {
	return m_pAudioDeviceModuleImp->PlayoutIsInitialized();
}

int32_t WebRTCAudioDeviceModule::RecordingIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->RecordingIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::InitRecording()  { 
	return m_pAudioDeviceModuleImp->InitRecording(); 
}

bool WebRTCAudioDeviceModule::RecordingIsInitialized() const  {
	return m_pAudioDeviceModuleImp->RecordingIsInitialized();
}

int32_t WebRTCAudioDeviceModule::StartPlayout()  {
	return m_pAudioDeviceModuleImp->StartPlayout();
}

int32_t WebRTCAudioDeviceModule::StopPlayout()  {
	return m_pAudioDeviceModuleImp->StopPlayout();
}

bool WebRTCAudioDeviceModule::Playing() const  {
	return m_pAudioDeviceModuleImp->Playing();
}

int32_t WebRTCAudioDeviceModule::StartRecording()  {
	return m_pAudioDeviceModuleImp->StartRecording();
}

int32_t WebRTCAudioDeviceModule::StopRecording()  {
	return m_pAudioDeviceModuleImp->StopRecording();
}

bool WebRTCAudioDeviceModule::Recording() const  {
	return m_pAudioDeviceModuleImp->Recording();
}

int32_t WebRTCAudioDeviceModule::SetAGC(bool enable)  {
	return m_pAudioDeviceModuleImp->SetAGC(enable);
}

bool WebRTCAudioDeviceModule::AGC() const  {
	return m_pAudioDeviceModuleImp->AGC();
}

int32_t WebRTCAudioDeviceModule::SetWaveOutVolume(uint16_t volume_left, uint16_t volume_right)  {
	return m_pAudioDeviceModuleImp->SetWaveOutVolume(volume_left, volume_right);
}

int32_t WebRTCAudioDeviceModule::WaveOutVolume(uint16_t* volume_left, uint16_t* volume_right) const  {
	return m_pAudioDeviceModuleImp->WaveOutVolume(volume_left, volume_right);
}

int32_t WebRTCAudioDeviceModule::InitSpeaker()  {
	return m_pAudioDeviceModuleImp->InitSpeaker();
}

bool WebRTCAudioDeviceModule::SpeakerIsInitialized() const  {
	return m_pAudioDeviceModuleImp->SpeakerIsInitialized();
}

int32_t WebRTCAudioDeviceModule::InitMicrophone()  {
	return m_pAudioDeviceModuleImp->InitMicrophone();
}

bool WebRTCAudioDeviceModule::MicrophoneIsInitialized() const  {
	return m_pAudioDeviceModuleImp->MicrophoneIsInitialized();
}

int32_t WebRTCAudioDeviceModule::SpeakerVolumeIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->SpeakerVolumeIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetSpeakerVolume(uint32_t volume)  {
	return m_pAudioDeviceModuleImp->SetSpeakerVolume(volume);
}

int32_t WebRTCAudioDeviceModule::SpeakerVolume(uint32_t* volume) const  {
	return m_pAudioDeviceModuleImp->SpeakerVolume(volume);
}

int32_t WebRTCAudioDeviceModule::MaxSpeakerVolume(uint32_t* max_volume) const  {
	return m_pAudioDeviceModuleImp->MaxSpeakerVolume(max_volume);
}

int32_t WebRTCAudioDeviceModule::MinSpeakerVolume(uint32_t* min_volume) const  {
	return m_pAudioDeviceModuleImp->MinSpeakerVolume(min_volume);
}

int32_t WebRTCAudioDeviceModule::SpeakerVolumeStepSize(uint16_t* step_size) const  {
	return m_pAudioDeviceModuleImp->SpeakerVolumeStepSize(step_size);
}

int32_t WebRTCAudioDeviceModule::MicrophoneVolumeIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->MicrophoneVolumeIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetMicrophoneVolume(uint32_t volume)  {
	return m_pAudioDeviceModuleImp->SetMicrophoneVolume(volume);
}

int32_t WebRTCAudioDeviceModule::MicrophoneVolume(uint32_t* volume) const  {
	return m_pAudioDeviceModuleImp->MicrophoneVolume(volume);
}

int32_t WebRTCAudioDeviceModule::MaxMicrophoneVolume(uint32_t* max_volume) const  {
	return m_pAudioDeviceModuleImp->MaxMicrophoneVolume(max_volume);
}

int32_t WebRTCAudioDeviceModule::MinMicrophoneVolume(uint32_t* min_volume) const  {
	return m_pAudioDeviceModuleImp->MinMicrophoneVolume(min_volume);
}

int32_t WebRTCAudioDeviceModule::MicrophoneVolumeStepSize(uint16_t* step_size) const  {
	return m_pAudioDeviceModuleImp->MicrophoneVolumeStepSize(step_size);
}

int32_t WebRTCAudioDeviceModule::SpeakerMuteIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->SpeakerMuteIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetSpeakerMute(bool enable)  {
	return m_pAudioDeviceModuleImp->SetSpeakerMute(enable);
}

int32_t WebRTCAudioDeviceModule::SpeakerMute(bool* enabled) const  {
	return m_pAudioDeviceModuleImp->SpeakerMute(enabled);
}

int32_t WebRTCAudioDeviceModule::MicrophoneMuteIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->MicrophoneMuteIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetMicrophoneMute(bool enable)  {
	return m_pAudioDeviceModuleImp->SetMicrophoneMute(enable);
}

int32_t WebRTCAudioDeviceModule::MicrophoneMute(bool* enabled) const  {
	return m_pAudioDeviceModuleImp->MicrophoneMute(enabled);
}

int32_t WebRTCAudioDeviceModule::MicrophoneBoostIsAvailable(bool* available)  {
	return m_pAudioDeviceModuleImp->MicrophoneBoostIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetMicrophoneBoost(bool enable)  {
	return m_pAudioDeviceModuleImp->SetMicrophoneBoost(enable);
}

int32_t WebRTCAudioDeviceModule::MicrophoneBoost(bool* enabled) const  {
	return m_pAudioDeviceModuleImp->MicrophoneBoost(enabled);
}

int32_t WebRTCAudioDeviceModule::StereoPlayoutIsAvailable(bool* available) const  {
	return m_pAudioDeviceModuleImp->StereoPlayoutIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetStereoPlayout(bool enable)  {
	return m_pAudioDeviceModuleImp->SetStereoPlayout(enable);
}

int32_t WebRTCAudioDeviceModule::StereoPlayout(bool* enabled) const  {
	return m_pAudioDeviceModuleImp->StereoPlayout(enabled);
}

int32_t WebRTCAudioDeviceModule::StereoRecordingIsAvailable(bool* available) const  {
	return m_pAudioDeviceModuleImp->StereoRecordingIsAvailable(available);
}

int32_t WebRTCAudioDeviceModule::SetStereoRecording(bool enable)  {
	return m_pAudioDeviceModuleImp->SetStereoRecording(enable);
}

int32_t WebRTCAudioDeviceModule::StereoRecording(bool* enabled) const  {
	return m_pAudioDeviceModuleImp->StereoRecording(enabled);
}

int32_t WebRTCAudioDeviceModule::SetRecordingChannel(const ChannelType channel)  {
	return m_pAudioDeviceModuleImp->SetRecordingChannel(channel);
}

int32_t WebRTCAudioDeviceModule::RecordingChannel(ChannelType* channel) const  {
	return m_pAudioDeviceModuleImp->RecordingChannel(channel);
}

int32_t WebRTCAudioDeviceModule::SetPlayoutBuffer(const BufferType type, uint16_t size_ms)  {
	return m_pAudioDeviceModuleImp->SetPlayoutBuffer(type, size_ms);
}

int32_t WebRTCAudioDeviceModule::PlayoutBuffer(BufferType* type, uint16_t* size_ms) const  {
	return m_pAudioDeviceModuleImp->PlayoutBuffer(type, size_ms);
}

int32_t WebRTCAudioDeviceModule::PlayoutDelay(uint16_t* delay_ms) const  {
	return m_pAudioDeviceModuleImp->PlayoutDelay(delay_ms);
}

int32_t WebRTCAudioDeviceModule::RecordingDelay(uint16_t* delay_ms) const  {
	return m_pAudioDeviceModuleImp->RecordingDelay(delay_ms);
}

int32_t WebRTCAudioDeviceModule::CPULoad(uint16_t* load) const  {
	return m_pAudioDeviceModuleImp->CPULoad(load);
}

int32_t WebRTCAudioDeviceModule::StartRawOutputFileRecording(const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize])  {
	return m_pAudioDeviceModuleImp->StartRawOutputFileRecording(pcm_file_name_utf8);
}

int32_t WebRTCAudioDeviceModule::StopRawOutputFileRecording()  {
	return m_pAudioDeviceModuleImp->StopRawOutputFileRecording();
}

int32_t WebRTCAudioDeviceModule::StartRawInputFileRecording(const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize])  {
	return m_pAudioDeviceModuleImp->StartRawInputFileRecording(pcm_file_name_utf8);
}

int32_t WebRTCAudioDeviceModule::StopRawInputFileRecording()  {
	return m_pAudioDeviceModuleImp->StopRawInputFileRecording();
}

int32_t WebRTCAudioDeviceModule::SetRecordingSampleRate(const uint32_t samples_per_sec)  {
	return m_pAudioDeviceModuleImp->SetRecordingSampleRate(samples_per_sec);
}

int32_t WebRTCAudioDeviceModule::RecordingSampleRate(uint32_t* samples_per_sec) const  {
	return m_pAudioDeviceModuleImp->RecordingSampleRate(samples_per_sec);
}

int32_t WebRTCAudioDeviceModule::SetPlayoutSampleRate(const uint32_t samples_per_sec)  {
	return m_pAudioDeviceModuleImp->SetPlayoutSampleRate(samples_per_sec);
}

int32_t WebRTCAudioDeviceModule::PlayoutSampleRate(uint32_t* samples_per_sec) const  {
	return m_pAudioDeviceModuleImp->PlayoutSampleRate(samples_per_sec);
}

int32_t WebRTCAudioDeviceModule::ResetAudioDevice()  {
	return m_pAudioDeviceModuleImp->ResetAudioDevice();
}

int32_t WebRTCAudioDeviceModule::SetLoudspeakerStatus(bool enable)  {
	return m_pAudioDeviceModuleImp->SetLoudspeakerStatus(enable);
}

int32_t WebRTCAudioDeviceModule::GetLoudspeakerStatus(bool* enabled) const  {
	return m_pAudioDeviceModuleImp->GetLoudspeakerStatus(enabled);
}

bool WebRTCAudioDeviceModule::BuiltInAECIsAvailable() const  {
	return m_pAudioDeviceModuleImp->BuiltInAECIsAvailable();
}

bool WebRTCAudioDeviceModule::BuiltInAGCIsAvailable() const  {
	return m_pAudioDeviceModuleImp->BuiltInAGCIsAvailable();
}

bool WebRTCAudioDeviceModule::BuiltInNSIsAvailable() const  {
	return m_pAudioDeviceModuleImp->BuiltInNSIsAvailable();
}

int32_t WebRTCAudioDeviceModule::EnableBuiltInAEC(bool enable)  {
	return m_pAudioDeviceModuleImp->EnableBuiltInAEC(enable);
}

int32_t WebRTCAudioDeviceModule::EnableBuiltInAGC(bool enable)  {
	return m_pAudioDeviceModuleImp->EnableBuiltInAGC(enable);
}

int32_t WebRTCAudioDeviceModule::EnableBuiltInNS(bool enable)  {
	return m_pAudioDeviceModuleImp->EnableBuiltInNS(enable);
}

// Only supported on iOS.
#if defined(WEBRTC_IOS)
int WebRTCAudioDeviceModule::GetPlayoutAudioParameters(AudioParameters* params) const  {
	return m_pAudioDeviceModuleImp->GetPlayoutAudioParameters(params);
}
int WebRTCAudioDeviceModule::GetRecordAudioParameters(AudioParameters* params) const  {
	return m_pAudioDeviceModuleImp->GetRecordAudioParameters(params);
}
#endif  // WEBRTC_IOS
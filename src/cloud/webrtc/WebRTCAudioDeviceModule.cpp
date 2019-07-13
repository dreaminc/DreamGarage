#include "WebRTCAudioDeviceModule.h"

//#include "Primitives/CircularBuffer.h"

#include "rtc_base/refcount.h"
#include "rtc_base/refcountedobject.h"
#include "rtc_base/checks.h"

#define WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
#include "modules/audio_device/audio_device_impl.h"

// WebRTCAudioDeviceModule

WebRTCAudioDeviceModule::WebRTCAudioDeviceModule() :
	m_fInitialized(false),
	m_fValid(false),
	m_pAudioTransport(nullptr),
	m_msOutputDelay(0),
	m_fPlaying(false),
	m_fRecording(false)
{
	m_fValid = true;
}

WebRTCAudioDeviceModule::~WebRTCAudioDeviceModule() {
	m_pAudioTransport = nullptr;
}

#include <avrt.h>

RESULT WebRTCAudioDeviceModule::WebRTCADMProcess() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCADMProcess start");

	m_fRunning = true;
	int samplingRate = 48000;
	int numSamples = samplingRate / 100;
	int numChannels = 1;
	int bitsPerSample = 16;

	int16_t *pAudioBuffer = new int16_t[numSamples];

	DWORD taskIndex = 0;
	HANDLE hAudioRenderProcessTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	CNM(hAudioRenderProcessTask, "Failed to set up audio process task");

	while (m_fRunning) {

		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
		static bool fSlept = false;

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		auto diffVal = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();

		if (m_pAudioTransport != nullptr && diffVal > 9) {

			lastUpdateTime = timeNow - std::chrono::microseconds(diffVal - 10);

			int64_t msElapsedTime = 0;
			int64_t msNTPTime = 0;

			//int numSamples = (int)(44100.0f * ((float)diffVal/1000.0f));

			//memset(pAudioBuffer, 0, sizeof(int16_t) * numSamples);

			//int32_t retVal = m_pAudioTransport->NeedMorePlayData(
			//	441,
			//	sizeof(uint16_t),
			//	1,
			//	44100,
			//	pAudioBuffer,
			//	nSamplesOut,  
			//	&msElapsedTime,
			//	&msNTPTime);

			m_pAudioTransport->PullRenderData(
				bitsPerSample,
				samplingRate,
				numChannels,
				numSamples,
				pAudioBuffer,
				&msElapsedTime,
				&msNTPTime
			);

			if (diffVal > 10) {
				m_msOutputDelay = diffVal - 10;
			}

			fSlept = false;
		}

		//std::chrono::system_clock::time_point timeNow2 = std::chrono::system_clock::now();
		//auto diffVal2 = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow2 - lastUpdateTime).count();
		
		if (!fSlept) {
			Sleep(8);
			fSlept = true;
		}

		//Sleep(1);
	}

Error:

	DEBUG_LINEOUT("WebRTCADMProcess End");

	return r;
}

RESULT WebRTCAudioDeviceModule::Initialize() {
	RESULT r = R_PASS;

	// Sound Buffer
	//m_pPendingSoundBuffer = SoundBuffer::Make(2, SoundBuffer::type::SIGNED_16_BIT);
	//CN(m_pPendingSoundBuffer);

	m_pendingAudioCircularBuffer.InitializePendingBuffer();

	m_fInitialized = true;

	// Kick off thread
	m_webrtcADMThread = std::thread(&WebRTCAudioDeviceModule::WebRTCADMProcess, this);

//Error:
	return r;
}

// Make sure we have a valid ADM before returning it to user.
bool WebRTCAudioDeviceModule::IsValid() {
	return m_fValid;
}

float WebRTCAudioDeviceModule::GetRunTimeMicAverage() {
	return m_runTimeAvgMicValue;
}

// Override AudioDeviceModule's RegisterAudioCallback method to remember the
// actual audio transport (e.g.: voice engine).
int32_t WebRTCAudioDeviceModule::RegisterAudioCallback(webrtc::AudioTransport* pAudioCallback)  {
	
	// Remember the audio callback to forward PCM data
	m_pAudioTransport = pAudioCallback;

	return 0;
}

// AudioDeviceModule pass through method overrides.
int32_t WebRTCAudioDeviceModule::ActiveAudioLayer(AudioLayer* audio_layer) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::Init()  {

	Initialize();

	return 0;
}

int32_t WebRTCAudioDeviceModule::Terminate()  {
	return 0;
}

bool WebRTCAudioDeviceModule::Initialized() const  {
	return m_fInitialized;
}

int16_t WebRTCAudioDeviceModule::PlayoutDevices()  {
	return 0;
}

int16_t WebRTCAudioDeviceModule::RecordingDevices()  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::PlayoutDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize])  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::RecordingDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize])  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetPlayoutDevice(uint16_t index)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetPlayoutDevice(WindowsDeviceType device)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetRecordingDevice(uint16_t index)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetRecordingDevice(WindowsDeviceType device)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::PlayoutIsAvailable(bool* available)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::InitPlayout()  {
	return 0;
}

bool WebRTCAudioDeviceModule::PlayoutIsInitialized() const  {
	return false;
}

int32_t WebRTCAudioDeviceModule::RecordingIsAvailable(bool* available)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::InitRecording()  { 
	return 0;
}

bool WebRTCAudioDeviceModule::RecordingIsInitialized() const  {
	return false;
}

int32_t WebRTCAudioDeviceModule::StartPlayout()  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::StopPlayout()  {
	return 0;
}

bool WebRTCAudioDeviceModule::Playing() const  {
	return m_fPlaying;
}

int32_t WebRTCAudioDeviceModule::StartRecording()  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::StopRecording()  {
	return 0;
}

bool WebRTCAudioDeviceModule::Recording() const  {
	return m_fRecording;
}

int32_t WebRTCAudioDeviceModule::InitSpeaker()  {
	return 0;
}

bool WebRTCAudioDeviceModule::SpeakerIsInitialized() const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::InitMicrophone()  {
	return 0;
}

bool WebRTCAudioDeviceModule::MicrophoneIsInitialized() const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SpeakerVolumeIsAvailable(bool* available)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetSpeakerVolume(uint32_t volume)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SpeakerVolume(uint32_t* volume) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MaxSpeakerVolume(uint32_t* max_volume) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MinSpeakerVolume(uint32_t* min_volume) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MicrophoneVolumeIsAvailable(bool* available)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetMicrophoneVolume(uint32_t volume)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MicrophoneVolume(uint32_t* volume) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MaxMicrophoneVolume(uint32_t* max_volume) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MinMicrophoneVolume(uint32_t* min_volume) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SpeakerMuteIsAvailable(bool* available)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetSpeakerMute(bool enable)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SpeakerMute(bool* enabled) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MicrophoneMuteIsAvailable(bool* available)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetMicrophoneMute(bool enable)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::MicrophoneMute(bool* enabled) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::StereoPlayoutIsAvailable(bool* available) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetStereoPlayout(bool enable)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::StereoPlayout(bool* enabled) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::StereoRecordingIsAvailable(bool* available) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::SetStereoRecording(bool enable)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::StereoRecording(bool* enabled) const  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::PlayoutDelay(uint16_t* delay_ms) const  {
	//DCHECK(worker_thread_checker_.CalledOnValidThread());
	//base::AutoLock auto_lock(lock_);

	*delay_ms = static_cast<uint16_t>(m_msOutputDelay);

	return 0;
}

bool WebRTCAudioDeviceModule::BuiltInAECIsAvailable() const  {
	return false;
}

bool WebRTCAudioDeviceModule::BuiltInAGCIsAvailable() const  {
	return false;
}

bool WebRTCAudioDeviceModule::BuiltInNSIsAvailable() const  {
	return false;
}

int32_t WebRTCAudioDeviceModule::EnableBuiltInAEC(bool enable)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::EnableBuiltInAGC(bool enable)  {
	return 0;
}

int32_t WebRTCAudioDeviceModule::EnableBuiltInNS(bool enable)  {
	return 0;
}

// Only supported on iOS.
#if defined(WEBRTC_IOS)
int WebRTCAudioDeviceModule::GetPlayoutAudioParameters(AudioParameters* params) const  {
	return 0;
}
int WebRTCAudioDeviceModule::GetRecordAudioParameters(AudioParameters* params) const  {
	return 0;
}
#endif  // WEBRTC_IOS
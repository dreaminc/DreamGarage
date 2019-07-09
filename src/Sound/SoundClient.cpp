#include "SoundClient.h"

#include "SoundFile.h"
#include "Primitives/point.h"
#include "Primitives/vector.h"

#include "AudioPacket.h"

SoundClient::SoundClient(std::wstring *pwstrOptAudioOutputGUID) {
	if (pwstrOptAudioOutputGUID != nullptr) {
		m_wstrAudioOutputDeviceGUID.assign(*pwstrOptAudioOutputGUID);
	}
	else {
		m_wstrAudioOutputDeviceGUID = L"default";
	}

	//m_wstrAudioInputDeviceGUID = L"default";
}

SoundClient::~SoundClient() {
	// empty
}


RESULT SoundClient::InitializeCaptureSoundBuffer(int numChannels, int samplingRate, sound::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pCaptureSoundBuffer == nullptr));

	m_pCaptureSoundBuffer = SoundBuffer::Make(numChannels, samplingRate, bufferType);
	CN(m_pCaptureSoundBuffer);

	DEBUG_LINEOUT("Initialized Capture Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}

RESULT SoundClient::InitializeRenderSoundBuffer(int numChannels, int samplingRate, sound::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pRenderSoundBuffer == nullptr));

	m_pRenderSoundBuffer = SoundBuffer::Make(numChannels, samplingRate, bufferType);
	CN(m_pRenderSoundBuffer);

	DEBUG_LINEOUT("Initialized Render Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}

// TODO: This might not be needed 
RESULT SoundClient::InitializeSpatialSoundBuffer(int numChannels, int samplingRate, sound::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pSpatialSoundBuffer == nullptr));

	m_pSpatialSoundBuffer = SoundBuffer::Make(numChannels, samplingRate, bufferType);
	CN(m_pSpatialSoundBuffer);

	DEBUG_LINEOUT("Initialized Spatial Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}

bool SoundClient::IsRunning() {
	return (m_renderState == sound::state::RUNNING);
}

RESULT SoundClient::StartCapture() {
	DEBUG_LINEOUT("SoundClient::StartCapture");

	// This will kick off the audio capture process defined in the sound client implementation
	m_captureState = sound::state::RUNNING;
	m_audioCaptureProcessingThread = std::thread(&SoundClient::AudioCaptureProcess, this);

	return R_PASS;
}

RESULT SoundClient::StopCapture() {
	DEBUG_LINEOUT("SoundClient::StopCapture");

	m_captureState = sound::state::STOPPED;

	// Join thread
	if (m_audioCaptureProcessingThread.joinable()) {
		m_audioCaptureProcessingThread.join();
	}

	return R_PASS;
}

float SoundClient::GetRunTimeCaptureAverage() {
	return m_runTimeCaptureAverage;
}

RESULT SoundClient::StartRender() {
	DEBUG_LINEOUT("SoundClient::StartRender");

	// This will kick off the audio render process defined in the sound client implementation
	m_renderState = sound::state::RUNNING;
	m_audioRenderProcessingThread = std::thread(&SoundClient::AudioRenderProcess, this);

	return R_PASS;
}

RESULT SoundClient::StopRender() {
	DEBUG_LINEOUT("SoundClient::StopRender");

	m_renderState = sound::state::STOPPED;

	// Join thread
	if (m_audioRenderProcessingThread.joinable()) {
		m_audioRenderProcessingThread.join();
	}

	return R_PASS;
}

RESULT SoundClient::StartSpatial() {
	DEBUG_LINEOUT("SoundClient::StartSpatial");

	// This will kick off the audio spatial process defined in the sound client implementation
	m_spatialState = sound::state::RUNNING;
	m_audioSpatialProcessingThread = std::thread(&SoundClient::AudioSpatialProcess, this);

	return R_PASS;
}

RESULT SoundClient::StopSpatial() {
	DEBUG_LINEOUT("SoundClient::StopSpatial");

	m_spatialState = sound::state::STOPPED;

	// Join thread
	if (m_audioSpatialProcessingThread.joinable()) {
		m_audioSpatialProcessingThread.join();
	}

	return R_PASS;
}

RESULT SoundClient::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SoundClient::Start");

	CR(StartRender());

	CR(StartCapture());

Error:
	return r;
}

RESULT SoundClient::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SoundClient::Stop");

	CR(StopRender());

	CR(StopCapture());

Error:
	return r;
}

RESULT SoundClient::RegisterObserver(SoundClient::observer *pObserver) {
	RESULT r = R_PASS;

	CN(pObserver);
	CBM((m_pSoundClientObserver == nullptr), "SoundClient Observer already registered");

	m_pSoundClientObserver = pObserver;

Error:
	return r;
}

RESULT SoundClient::UnregisterObserver(SoundClient::observer *pObserver) {
	RESULT r = R_PASS;

	CN(pObserver);
	CNM(m_pSoundClientObserver, "SoundClient observer not registered");

	m_pSoundClientObserver = nullptr;

Error:
	return r;
}

RESULT SoundClient::HandleAudioDataCaptured(int numFrames) {
	RESULT r = R_PASS;

	// TODO: Handle other stuff if needed
	if (m_pSoundClientObserver != nullptr) {
		CR(m_pSoundClientObserver->OnAudioDataCaptured(numFrames, m_pCaptureSoundBuffer));
	}

Error:
	return r;
}

RESULT SoundClient::PushMonoAudioBufferToRenderBuffer(int numFrames, SoundBuffer *pSourceBuffer) {
	RESULT r = R_PASS;

	CBM((pSourceBuffer->NumChannels() == 1), "Source buffer is not mono");

	m_pRenderSoundBuffer->LockBuffer();
	{
		if (m_pRenderSoundBuffer->IsFull() == false) {
			//int numChannels = m_pRenderSoundBuffer->NumChannels();

			//for (int i = 0; i < numChannels; i++) {
			//	CR(m_pRenderSoundBuffer->PushDataToChannel(i, pDataBuffer, numFrames));
			//
			m_pRenderSoundBuffer->PushMonoAudioBuffer(numFrames, pSourceBuffer);

		}
		else {
			DEBUG_LINEOUT("Render buffer is full");
		}
	}
	m_pRenderSoundBuffer->UnlockBuffer();

Error:
	return r;
}

RESULT SoundClient::LoopSoundFile(SoundFile *pSoundFile) {
	return R_NOT_IMPLEMENTED;
}

RESULT SoundClient::PlaySoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;
	float *pFloatAudioBuffer = nullptr;

	CN(pSoundFile);
	CBM((pSoundFile->NumChannels() == m_pRenderSoundBuffer->NumChannels()), 
		"Don't currently support playing files that don't match render buffer channel count");

	m_pRenderSoundBuffer->LockBuffer();
	{
		if (m_pRenderSoundBuffer->IsFull() == false) {
			CR(pSoundFile->GetAudioBuffer(pFloatAudioBuffer));
			
			// TODO: Add sampling rate to sound files!
			CR(m_pRenderSoundBuffer->PushData(pFloatAudioBuffer, pSoundFile->GetNumFrames(), 44100));
		}
	}
	m_pRenderSoundBuffer->UnlockBuffer();
	
Error:
	if (pFloatAudioBuffer != nullptr) {
		delete [] pFloatAudioBuffer;
		pFloatAudioBuffer = nullptr;
	}
	return r;
}

RESULT SoundClient::PushAudioPacket(const AudioPacket &pendingAudioPacket) {
	return R_NOT_HANDLED;
}

RESULT SoundClient::PlayAudioPacketSigned16Bit(const AudioPacket &pendingAudioPacket, std::string strAudioTrackLabel, int channel) {
	return R_NOT_HANDLED;
}

// Spatial Sound Objects
std::shared_ptr<SpatialSoundObject> SoundClient::AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) {
	RESULT r = R_PASS;

	std::shared_ptr<SpatialSoundObject>	pSpatialSoundObject = nullptr;

	CBM((m_spatialSoundObjects.size() < m_maxSpatialSoundObjects), "Cannot add another spatial audio object");

	pSpatialSoundObject = MakeSpatialAudioObject(ptPosition, vEmitterDirection, vListenerDirection);
	CNM(pSpatialSoundObject, "Failed to create spatial sound object");

	// Add to our storage

	m_spatialSoundObjects.push_back(pSpatialSoundObject);

	return pSpatialSoundObject;

Error:
	if (pSpatialSoundObject != nullptr) {
		pSpatialSoundObject = nullptr;
	}

	return nullptr;
}

bool SoundClient::FindSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject) {
	auto it = std::find(m_spatialSoundObjects.begin(), m_spatialSoundObjects.end(), pSpatialSoundObject);
	
	if (it == m_spatialSoundObjects.end()) 
		return false;
	else 
		return true;
}

RESULT SoundClient::ClearSpatialSoundObjects() {
	
	m_spatialSoundObjects.clear();

	return R_PASS;
}
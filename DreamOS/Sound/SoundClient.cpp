#include "SoundClient.h"

SoundClient::SoundClient() {
	// empty
}

SoundClient::~SoundClient() {
	// empty
}


RESULT SoundClient::InitializeCaptureSoundBuffer(int numChannels, SoundBuffer::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pCaptureSoundBuffer == nullptr));

	m_pCaptureSoundBuffer = SoundBuffer::Make(numChannels, bufferType);
	CN(m_pCaptureSoundBuffer);

	DEBUG_LINEOUT("Initialized Capture Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}

RESULT SoundClient::InitializeRenderSoundBuffer(int numChannels, SoundBuffer::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pRenderSoundBuffer == nullptr));

	m_pRenderSoundBuffer = SoundBuffer::Make(numChannels, bufferType);
	CN(m_pRenderSoundBuffer);

	DEBUG_LINEOUT("Initialized Render Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}


bool SoundClient::IsRunning() {
	return (m_renderState == state::RUNNING);
}

RESULT SoundClient::StartCapture() {
	DEBUG_LINEOUT("SoundClient::StartCapture");

	// This will kick off the audio capture process defined in the sound client implementation
	m_captureState = state::RUNNING;
	m_audioCaptureProcessingThread = std::thread(&SoundClient::AudioCaptureProcess, this);

	return R_PASS;
}

RESULT SoundClient::StopCapture() {
	DEBUG_LINEOUT("SoundClient::StopCapture");

	m_captureState = state::STOPPED;

	// Join thread
	if (m_audioCaptureProcessingThread.joinable()) {
		m_audioCaptureProcessingThread.join();
	}

	return R_PASS;
}

RESULT SoundClient::StartRender() {
	DEBUG_LINEOUT("SoundClient::StartRender");

	// This will kick off the audio render process defined in the sound client implementation
	m_renderState = state::RUNNING;
	m_audioRenderProcessingThread = std::thread(&SoundClient::AudioRenderProcess, this);

	return R_PASS;
}

RESULT SoundClient::StopRender() {
	DEBUG_LINEOUT("SoundClient::StopRender");

	m_renderState = state::STOPPED;

	// Join thread
	if (m_audioRenderProcessingThread.joinable()) {
		m_audioRenderProcessingThread.join();
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
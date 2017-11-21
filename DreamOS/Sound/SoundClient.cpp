#include "SoundClient.h"

SoundClient::SoundClient() {
	// empty
}

SoundClient::~SoundClient() {
	// empty
}

//m_cloudThread = std::thread(&CloudController::CloudThreadProcess, this);

bool SoundClient::IsRunning() {
	return (m_state == state::RUNNING);
}

RESULT SoundClient::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SoundClient::Start");

	m_state = state::RUNNING;

	// This will kick off the audio process defined in the sound client implementation
	m_audioProcessingThread = std::thread(&SoundClient::AudioProcess, this);

//Error:
	return r;
}

RESULT SoundClient::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("SoundClient::Stop");

	m_state = state::STOPPED;
	
	/*
#if (defined(_WIN32) || defined(_WIN64))
	#include "Sandbox/Windows/Win32Helper.h"
	Win32Helper::PostQuitMessage(m_audioProcessingThread);
#else
#pragma message ("not implemented post quit to thread")
	while (m_state == state::RUNNING) {

	}
#endif
	*/

	// Join thread
	if (m_audioProcessingThread.joinable()) {
		m_audioProcessingThread.join();
	}

//Error:
	return r;
}
#ifndef SOUND_CLIENT_H_
#define SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundClient.h

#include <thread>

class SoundClient {
public:
	enum class state {
		UNINITIALIZED,
		STOPPED,
		RUNNING,
		INVALID
	};

public:
	SoundClient();
	~SoundClient();

	virtual RESULT Initialize() = 0;
	
protected:
	virtual RESULT AudioRenderProcess() = 0;
	//virtual RESULT AudioCaptureProcess() = 0;

	SoundClient::state m_renderState = SoundClient::state::UNINITIALIZED;
	//SoundClient::state m_captureState = SoundClient::state::UNINITIALIZED;

public:
	bool IsRunning();

	RESULT Start();
	RESULT Stop();

	//RESULT StartCapture();
	//RESULT StopCapture();

	RESULT StartRender();
	RESULT StopRender();

private:
	std::thread	m_audioRenderProcessingThread;
	//std::thread	m_audioCaptureProcessingThread;
};

#endif SOUND_CLIENT_H_
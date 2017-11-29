#ifndef SOUND_CLIENT_H_
#define SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundClient.h

#include <thread>

#include "SoundBuffer.h"

class SoundClient {
public:
	enum class state {
		UNINITIALIZED,
		STOPPED,
		RUNNING,
		INVALID
	};

public:
	class observer {
	public:
		virtual RESULT OnAudioDataCaptured(int numFrames, const SoundBuffer *pCaptureBuffer) = 0;
	};

	RESULT RegisterObserver(SoundClient::observer *pObserver);
	RESULT UnregisterObserver(SoundClient::observer *pObserver);

private:
	SoundClient::observer *m_pSoundClientObserver = nullptr;

public:
	SoundClient();
	~SoundClient();

	virtual RESULT Initialize() = 0;
	
protected:
	virtual RESULT AudioRenderProcess() = 0;
	virtual RESULT AudioCaptureProcess() = 0;

	SoundClient::state m_renderState = SoundClient::state::UNINITIALIZED;
	SoundClient::state m_captureState = SoundClient::state::UNINITIALIZED;

	RESULT InitializeCaptureSoundBuffer(int numChannels, SoundBuffer::type bufferType);
	RESULT InitializeRenderSoundBuffer(int numChannels, SoundBuffer::type bufferType);

	RESULT HandleAudioDataCaptured(int numFrames);

public:
	bool IsRunning();

	RESULT Start();
	RESULT Stop();

	RESULT StartCapture();
	RESULT StopCapture();

	RESULT StartRender();
	RESULT StopRender();

private:
	std::thread	m_audioRenderProcessingThread;
	std::thread	m_audioCaptureProcessingThread;

protected:
	SoundBuffer *m_pCaptureSoundBuffer = nullptr;
	SoundBuffer *m_pRenderSoundBuffer = nullptr;

};

#endif SOUND_CLIENT_H_
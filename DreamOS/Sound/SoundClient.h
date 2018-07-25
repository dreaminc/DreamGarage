#ifndef SOUND_CLIENT_H_
#define SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundClient.h

#include <thread>

#include "SoundBuffer.h"


class SoundFile;
class SpatialSoundObject;
class point;
class vector;

#define DEFAULT_MAX_SPATIAL_AUDIO_OBJECTS 20 

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
		virtual RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) = 0;
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
	virtual RESULT AudioSpatialProcess() = 0;

	SoundClient::state m_renderState = SoundClient::state::UNINITIALIZED;
	SoundClient::state m_captureState = SoundClient::state::UNINITIALIZED;
	SoundClient::state m_spatialState = SoundClient::state::UNINITIALIZED;

	RESULT InitializeCaptureSoundBuffer(int numChannels, SoundBuffer::type bufferType);
	RESULT InitializeRenderSoundBuffer(int numChannels, SoundBuffer::type bufferType);
	RESULT InitializeSpatialSoundBuffer(int numChannels, SoundBuffer::type bufferType);

	RESULT HandleAudioDataCaptured(int numFrames);

	virtual std::shared_ptr<SpatialSoundObject> MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) = 0;

	std::shared_ptr<SpatialSoundObject> AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection);
	bool FindSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject);
	RESULT ClearSpatialSoundObjects();

	int GetMaxSpatialAudioObjets() { return m_maxSpatialSoundObjects; };

private:
	std::vector<std::shared_ptr<SpatialSoundObject>> m_spatialSoundObjects;

public:
	bool IsRunning();

	RESULT Start();
	RESULT Stop();

	RESULT StartCapture();
	RESULT StopCapture();

	RESULT StartRender();
	RESULT StopRender();

	RESULT StartSpatial();
	RESULT StopSpatial();

	RESULT PushMonoAudioBufferToRenderBuffer(int numFrames, SoundBuffer *pSourceBuffer);

	RESULT PlaySoundFile(SoundFile *pSoundFile);

private:
	std::thread	m_audioRenderProcessingThread;
	std::thread	m_audioCaptureProcessingThread;
	std::thread	m_audioSpatialProcessingThread;

protected:
	SoundBuffer *m_pCaptureSoundBuffer = nullptr;
	SoundBuffer *m_pRenderSoundBuffer = nullptr;
	SoundBuffer *m_pSpatialSoundBuffer = nullptr;

protected:
	uint32_t m_maxSpatialSoundObjects = DEFAULT_MAX_SPATIAL_AUDIO_OBJECTS;
};

#endif SOUND_CLIENT_H_
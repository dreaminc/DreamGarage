#ifndef SOUND_CLIENT_H_
#define SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundClient.h

#include <thread>

#include "SoundBuffer.h"
#include "SoundCommon.h"

class SoundBuffer;
class SoundFile;
class AudioPacket;
class SpatialSoundObject;
class point;
class vector;

#define DEFAULT_MAX_SPATIAL_AUDIO_OBJECTS 20 

class SoundClient {

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
	SoundClient(std::wstring *pwstrOptAudioOutputGUID = nullptr);
	~SoundClient();

	virtual RESULT Initialize() = 0;
	
protected:
	virtual RESULT AudioRenderProcess() = 0;
	virtual RESULT AudioCaptureProcess() = 0;
	virtual RESULT AudioSpatialProcess() = 0;

	int GetRenderSamplingRate() { return m_renderSamplingRate; }
	int GetCaptureSamplingRate() { return m_captureSamplingRate; }
	int GetSpaitalSamplingRate() { return m_spatialSamplingRate; }

	sound::state m_renderState = sound::state::UNINITIALIZED;
	sound::state m_captureState = sound::state::UNINITIALIZED;
	sound::state m_spatialState = sound::state::UNINITIALIZED;

	RESULT InitializeCaptureSoundBuffer(int numChannels, int samplingRate, sound::type bufferType);
	RESULT InitializeRenderSoundBuffer(int numChannels, int samplingRate, sound::type bufferType);
	RESULT InitializeSpatialSoundBuffer(int numChannels, int samplingRate, sound::type bufferType);

	RESULT HandleAudioDataCaptured(int numFrames);

	virtual std::shared_ptr<SpatialSoundObject> MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) = 0;

public:
	std::shared_ptr<SpatialSoundObject> AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection);
	bool FindSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject);
	RESULT ClearSpatialSoundObjects();

	int GetMaxSpatialAudioObjets() { return m_maxSpatialSoundObjects; };

protected:
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

	virtual RESULT PlaySoundFile(SoundFile *pSoundFile);

	virtual RESULT PushAudioPacket(const AudioPacket &pendingAudioPacket);

	virtual std::wstring GetDeviceIDFromDeviceID(std::wstring wstrDeviceOutGUID) { return L"default"; }

private:
	std::thread	m_audioRenderProcessingThread;
	std::thread	m_audioCaptureProcessingThread;
	std::thread	m_audioSpatialProcessingThread;

protected:
	SoundBuffer *m_pCaptureSoundBuffer = nullptr;
	SoundBuffer *m_pRenderSoundBuffer = nullptr;
	SoundBuffer *m_pSpatialSoundBuffer = nullptr;

	int m_renderSamplingRate = DEFAULT_SAMPLING_RATE;
	int m_captureSamplingRate = DEFAULT_SAMPLING_RATE;
	int m_spatialSamplingRate = DEFAULT_SAMPLING_RATE;

	std::wstring m_wstrAudioOutputDeviceGUID;
	//std::wstring m_wstrAudioInputDeviceGUID;

protected:
	uint32_t m_maxSpatialSoundObjects = DEFAULT_MAX_SPATIAL_AUDIO_OBJECTS;
	bool m_fHRTFEnabled = false;
};

#endif SOUND_CLIENT_H_
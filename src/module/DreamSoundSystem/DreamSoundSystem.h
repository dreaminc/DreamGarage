#ifndef DREAM_SOUND_SYSTEM_H_
#define DREAM_SOUND_SYSTEM_H_

#include "core/ehm/EHM.h"

// DREAM OS
// dos/src/module/DreamSoundSystem/DreamSoundSystem.h

// The Dream Sound System module
// Initializes, manages and maintains all things sound / audio related

#include <memory>

#include "module/DreamModule.h"

#include "sound/SoundCommon.h"
#include "sound/SoundClient.h"
#include "sound/SoundFile.h"

#include "sandbox/NamedPipeServer.h"

class SpatialSoundObject;
class SoundFile;
class HMD;
//class SoundBuffer;

class DreamSoundSystem : 
	public DreamModule<DreamSoundSystem>, 
	public SoundClient::observer ,
	public NamedPipeServer::observer
{
	friend class DreamModuleManager;

public:
	class observer {
	public:
		virtual RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) = 0;
	};

	typedef enum {
		LOCAL_BROWSER_0,
		LOCAL_MIC,
		PEER_1,
		PEER_2,
		PEER_3,
		PEER_4,
		PEER_5,
		REMOTE_BROWSER_MONO_0,
		INVALID
	} MIXDOWN_TARGET;

public:
	DreamSoundSystem(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamSoundSystem();

	virtual RESULT InitializeModule(void *pContext = nullptr) override;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	RESULT RegisterObserver(DreamSoundSystem::observer *pObserver);
	RESULT UnregisterObserver();

public:
	// SoundClient::observer
	virtual RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) override;

public:
	std::shared_ptr<SpatialSoundObject> AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection);
	std::shared_ptr<SoundFile> LoadSoundFile(const std::wstring &wstrFilename, SoundFile::type soundFileType);
	RESULT PlaySoundFile(std::shared_ptr<SoundFile> pSoundFile);
	RESULT LoopSoundFile(std::shared_ptr<SoundFile> pSoundFile);
	RESULT PlayAudioPacket(const AudioPacket &pendingAudioPacket);
	RESULT PlayAudioPacketSigned16Bit(const AudioPacket &pendingAudioPacket, std::string strAudioTrackLabel, int channel);

	float GetRunTimeCaptureAverage();

protected:
	static DreamSoundSystem* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::vector<std::shared_ptr<SpatialSoundObject>> m_pSpatialSoundObjects;

	RESULT AddSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject);
	RESULT RemoveSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject);
	RESULT ClearSpatialSoundObjects();

private:
	//std::shared_ptr<SoundClient> m_pWASAPICaptureClient = nullptr;
	//std::shared_ptr<SoundClient> m_pXAudio2AudioClient = nullptr;
	SoundClient *m_pWASAPICaptureClient = nullptr;
	SoundClient *m_pXAudio2AudioClient = nullptr;

	HMD *m_pHMD = nullptr;
	DreamSoundSystem::observer *m_pObserver = nullptr;

private:	
	std::shared_ptr<SpatialSoundObject> m_pTestSpatialSoundObject = nullptr;
	std::shared_ptr<SoundFile> m_pSoundFile = nullptr;

private:
	RESULT InitializeNamedPipeServer();
	RESULT HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n);

	std::shared_ptr<NamedPipeServer> m_pNamedPipeServer = nullptr;

public:
	// NamedPipeServerObserver
	virtual RESULT OnClientConnect() override;
	virtual RESULT OnClientDisconnect() override;

private:
	RESULT MixdownProcess();
	RESULT StartMixdownServer();
	RESULT InitalizeMixdownSendBuffer();
	RESULT TeardownMixdownSendBuffer();
	AudioPacket GetPendingMixdownAudioPacket(int numFrames);
	int64_t GetNumPendingFrames();
	
	std::vector<SoundBuffer*> m_pMixdownBuffers;

	std::chrono::system_clock::time_point m_lastMixdownReadTime;

	sound::state m_mixdownState = sound::state::UNINITIALIZED;

	std::thread	m_mixdownBufferProcessThread;

public:
	RESULT PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET mixdownTarget, int numFrames, const AudioPacket &pendingAudioPacket);
};

#endif // ! DREAM_SOUND_SYSTEM_H_
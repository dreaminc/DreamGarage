#ifndef DREAM_SOUND_SYSTEM_H_
#define DREAM_SOUND_SYSTEM_H_

#include "RESULT/EHM.h"

// DREAM OS
// Sound/DreamSoundSystem.h
// The Dream Sound System module
// Initializes, manages and maintains all things sound / audio related

#include "DreamModule.h"
#include "SoundCommon.h"

#include "SoundClient.h"

#include "SoundFile.h"

#include <memory>

class SpatialSoundObject;
class SoundFile;
class HMD;
//class SoundBuffer;

class DreamSoundSystem : 
	public DreamModule<DreamSoundSystem>, 
	public SoundClient::observer 
{
	friend class DreamModuleManager;

public:
	class observer {
	public:
		virtual RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) = 0;
	};

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

protected:
	static DreamSoundSystem* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::vector<std::shared_ptr<SpatialSoundObject>> m_pSpatialSoundObjects;

	RESULT AddSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject);
	RESULT RemoveSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject);
	RESULT ClearSpatialSoundObjects();

private:
	std::shared_ptr<SoundClient> m_pWASAPICaptureClient = nullptr;
	std::shared_ptr<SoundClient> m_pXAudio2AudioClient = nullptr;

	HMD *m_pHMD = nullptr;
	DreamSoundSystem::observer *m_pObserver = nullptr;
};

#endif // ! DREAM_SOUND_SYSTEM_H_
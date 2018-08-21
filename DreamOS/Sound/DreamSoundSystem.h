#ifndef DREAM_SOUND_SYSTEM_H_
#define DREAM_SOUND_SYSTEM_H_

#include "RESULT/EHM.h"

// DREAM OS
// Sound/DreamSoundSystem.h
// The Dream Sound System module
// Initializes, manages and maintains all things sound / audio related

#include "DreamModule.h"
#include "SoundCommon.h"

#include <memory>

class SoundClient;

class DreamSoundSystem : public DreamModule<DreamSoundSystem> {
public:
	DreamSoundSystem(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamSoundSystem();

	virtual RESULT InitializeModule(void *pContext = nullptr) override;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamSoundSystem* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<SoundClient> m_pWASAPICaptureClient = nullptr;
	std::shared_ptr<SoundClient> m_pXAudio2AudioClient = nullptr;
};

// The Self Construct
DreamSoundSystem* DreamSoundSystem::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamSoundSystem *pDreamModule = new DreamSoundSystem(pDreamOS, pContext);
	return pDreamModule;
}

#endif // ! DREAM_SOUND_SYSTEM_H_
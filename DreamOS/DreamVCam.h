#ifndef DREAM_VCAM_SYSTEM_H_
#define DREAM_VCAM_SYSTEM_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamVCam.h
// The Dream Virtual Camera Module

#include "DreamModule.h"

#include <memory>

class SpatialSoundObject;
class SoundFile;
class HMD;
class texture;
//class SoundBuffer;

class DreamVCam : public DreamModule<DreamVCam> {
	friend class DreamModuleManager;

public:
	DreamVCam(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamVCam();

	virtual RESULT InitializeModule(void *pContext = nullptr) override;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	RESULT ModuleProcess(void *pContext) override;

	RESULT SetSourceTexture(texture *pTexture);
	RESULT UnsetSourceTexture();

protected:
	static DreamVCam* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	texture * m_pSourceTexture = nullptr;
};

#endif // ! DREAM_VCAM_SYSTEM_H_
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
class NamedPipeServer;
class ProgramNode;
class OGLProgram;
class CameraNode;
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

	RESULT HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n);

	CameraNode *GetCameraNode();

protected:
	static DreamVCam* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<NamedPipeServer> m_pNamedPipeServer = nullptr;
	texture * m_pSourceTexture = nullptr;

	unsigned char *m_pLoadBuffer = nullptr;
	size_t m_pLoadBuffer_n = 0;

	CameraNode* m_pCamera = nullptr;

	// This node is used for the render texture
	OGLProgram *m_pOGLRenderNode = nullptr;

	// This node is used to run the render
	OGLProgram *m_pOGLEndNode = nullptr;
};

#endif // ! DREAM_VCAM_SYSTEM_H_
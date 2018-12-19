#ifndef DREAM_VCAM_SYSTEM_H_
#define DREAM_VCAM_SYSTEM_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamVCam.h
// The Dream Virtual Camera Module

#include "DreamModule.h"
#include "DreamUserControlArea/DreamContentSource.h"
#include "Sandbox/NamedPipeServer.h"
#include "DreamGarage/DreamGamepadCameraApp.h"

#include <memory>

class SpatialSoundObject;
class SoundFile;
class HMD;

class ProgramNode;
class OGLProgram;
class CameraNode;

class EnvironmentShare;

class texture;
class model;
class quad;

//class SoundBuffer;

class DreamUserControlArea;
class EnvironmentAsset;

class DreamVCam : 
	public DreamModule<DreamVCam>,
	public DreamContentSource,
	public NamedPipeServer::observer,
	public DreamGamepadCameraApp::observer
{
	friend class DreamModuleManager;

public:
	DreamVCam(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamVCam();

	virtual RESULT InitializeModule(void *pContext = nullptr) override;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	RESULT InitializePipeline();

	RESULT ModuleProcess(void *pContext) override;

	RESULT SetSourceTexture(texture* pTexture);
	RESULT UnsetSourceTexture();

	RESULT HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n);

	CameraNode *GetCameraNode();

	// Menu integration
	RESULT InitializeWithParent(DreamUserControlArea *pParentApp);
	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);

	// DreamContentSource
	virtual RESULT OnClick(point ptDiff, bool fMouseDown) override;
	virtual RESULT OnMouseMove(point mousePoint) override;
	virtual RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) override;

	virtual RESULT OnKeyPress(char chkey, bool fkeyDown) override;

	virtual texture* GetSourceTexture() override;

	virtual RESULT SetScope(std::string strScope) override;
	virtual RESULT SetPath(std::string strPath) override;
	virtual long GetCurrentAssetID() override;

	virtual RESULT SendFirstFrame() override;
	virtual RESULT CloseSource() override;

	virtual int GetWidth() override;
	virtual int GetHeight() override;

	virtual std::string GetTitle() override;
	virtual std::string GetContentType() override;

	// NamedPipeServerObserver
	virtual RESULT OnClientConnect() override;
	virtual RESULT OnClientDisconnect() override;

	// DreamGamepadCameraApp
	virtual RESULT OnCameraMoved() override;

	// Sharing Camera Placement
	RESULT HandleSettings(point ptPosition, quaternion qOrientation);

	RESULT SetIsSendingCameraPlacement(bool fSendingCameraPlacement);
	RESULT SetIsReceivingCameraPlacement(bool fReceivingCameraPlacement);
	bool IsSendingCameraPlacement();
	bool IsReceivingCameraPlacement();

	RESULT HideCameraSource();

	RESULT BroadcastVCamMessage();
	RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

protected:
	static DreamVCam* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<NamedPipeServer> m_pNamedPipeServer = nullptr;
	texture* m_pSourceTexture = nullptr;

	unsigned char *m_pLoadBuffer = nullptr;
	size_t m_pLoadBuffer_n = 0;

	std::shared_ptr<DreamGamepadCameraApp> m_pDreamGamepadCamera = nullptr;
	CameraNode* m_pCamera = nullptr;

	composite *m_pCameraComposite = nullptr;
	std::shared_ptr<model> m_pCameraModel = nullptr;
	std::shared_ptr<quad> m_pCameraQuad = nullptr;
	std::shared_ptr<quad> m_pCameraQuadBackground = nullptr;
	texture *m_pCameraQuadBackgroundTexture = nullptr;
	texture *m_pShareTexture = nullptr;

	// This node is used for the render texture
	OGLProgram *m_pOGLRenderNode = nullptr;

	// This node is used to run the render
	OGLProgram *m_pOGLEndNode = nullptr;

	std::shared_ptr<EnvironmentShare> m_pCurrentCameraShare = nullptr;

	bool m_fIsRunning = false;

	long m_assetID = -1;
	std::string m_strPath;
	std::string m_strScope;
	std::string m_strContentType;
	std::string m_strTitle = "Dream Virtual Camera";
	DreamUserControlArea* m_pParentApp = nullptr;

	// flags for sending/receiving data
	bool m_fSendingCameraPlacement = false;
	bool m_fReceivingCameraPlacement = false;
};

#endif // ! DREAM_VCAM_SYSTEM_H_

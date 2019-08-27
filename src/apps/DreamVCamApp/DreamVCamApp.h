#ifndef DREAM_VCAM_SYSTEM_H_
#define DREAM_VCAM_SYSTEM_H_

#include "core/ehm/EHM.h"

// Dream VCam App
// dos/src/app/DreamVCamApp/DreamVCamApp.h

// TODO: This is a module!!
// The Dream Virtual Camera Module

#include <memory>
#include <chrono>

#include "os/module/DreamModule.h"

#include "sandbox/NamedPipeServer.h"

#include "apps/DreamUserControlAreaApp/DreamContentSource.h"
#include "apps/DreamGamepadCameraApp/DreamGamepadCameraApp.h"

#include "os/DreamVideoStreamSubscriber.h"

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

class DreamUserControlAreaApp;
class EnvironmentAsset;

class DreamVCamApp : 
	public DreamModule<DreamVCamApp>,
	public DreamContentSource,
	public NamedPipeServer::observer,
	public DreamGamepadCameraApp::observer,
	public DreamVideoStreamSubscriber
{
	friend class DreamModuleManager;

public:
	enum class SourceType {
		CAMERA,
		SHARE_SCREEN,
		CLOSED,
		IN_USE,
		INVALID
	};

public:
	DreamVCamApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamVCamApp();

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
	RESULT InitializeWithParent(DreamUserControlAreaApp *pParentApp);
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

	virtual std::string GetScheme() override;
	virtual std::string GetURL() override;

	// NamedPipeServerObserver
	virtual RESULT OnClientConnect() override;
	virtual RESULT OnClientDisconnect() override;

	// DreamGamepadCameraApp
	virtual RESULT OnCameraAtRest() override;
	virtual RESULT OnCameraInMotion() override;

	// Sharing Camera Placement Data
	RESULT HandleSettings(point ptPosition, quaternion qOrientation);

	RESULT SetIsSendingCameraPlacement(bool fSendingCameraPlacement);
	RESULT SetIsReceivingCameraPlacement(bool fReceivingCameraPlacement);
	bool IsSendingCameraPlacement();
	bool IsReceivingCameraPlacement();

	RESULT HideCameraSource();
	RESULT ShareCameraSource();

	RESULT BroadcastVCamMessage();
	RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

	// Sharing Camera Texture over Socket
	RESULT StartSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare);
	RESULT StopSharing();
	RESULT StartReceiving(PeerConnection *pPeerConnection, std::shared_ptr<EnvironmentShare> pEnvironmentShare);
	RESULT StopReceiving();

	// TODO: this pattern is copied from DreamShareView, move to interface
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	RESULT SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight);
	RESULT UpdateFromPendingVideoFrame();

	texture* GetCameraQuadTexture();

	struct PendingFrame {
		bool fPending = false;
		int pxWidth = 0;
		int pxHeight = 0;
		uint8_t *pDataBuffer = nullptr;
		size_t pDataBuffer_n = 0;
	} m_pendingFrame;

	RESULT SetSourceType(DreamVCamApp::SourceType sourceType);
	RESULT Mute(bool fMute);

private:
	// streaming members
	bool m_fStreaming = false;
	bool m_fReceivingSteam = false;
	bool m_fShouldBeginStream = false;
	bool m_fReadyForFrame = false;	

private:
	std::wstring k_wstrMute = L"camera/messages-muted.png";
	std::wstring k_wstrInUse = L"camera/messages-in-use.png";
	std::wstring k_wstrClosed = L"camera/messages-closed.png";

protected:
	static DreamVCamApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<NamedPipeServer> m_pNamedPipeServer = nullptr;
	texture* m_pSourceTexture = nullptr;
	texture* m_pStreamingTexture = nullptr;

	std::mutex m_BufferMutex[2];
	unsigned char* m_pLoadBuffer[2] = { 0 };
	size_t m_pLoadBuffer_n = 0;
	int m_loadBufferIndex = 0;

	std::shared_ptr<DreamGamepadCameraApp> m_pDreamGamepadCamera = nullptr;
	CameraNode* m_pCamera = nullptr;

	composite *m_pCameraComposite = nullptr;
	std::shared_ptr<model> m_pCameraModel = nullptr;
	std::shared_ptr<quad> m_pCameraQuad = nullptr;
	texture* m_pCameraQuadTexture = nullptr;
	std::shared_ptr<quad> m_pCameraQuadBackground = nullptr;
	texture *m_pCameraQuadBackgroundTexture = nullptr;
	texture *m_pShareTexture = nullptr;
	texture *m_pMuteTexture = nullptr;
	texture *m_pInUseTexture = nullptr;
	texture *m_pClosedTexture = nullptr;

	// This node is used for the render texture
	OGLProgram *m_pOGLRenderNode = nullptr;

	// This node is used to run the render
	OGLProgram *m_pOGLEndNode = nullptr;

	std::shared_ptr<EnvironmentShare> m_pCurrentCameraShare = nullptr;

	bool m_fIsRunning = false;
	bool m_fPendDisconnectPipes = false;
	bool m_fIsMuted = false;
	DreamVCamApp::SourceType m_sourceType = SourceType::INVALID;

	long m_assetID = -1;
	std::string m_strPath;
	std::string m_strScope;
	std::string m_strContentType;
	std::string m_strTitle = "Dream Virtual Camera";
	DreamUserControlAreaApp* m_pParentApp = nullptr;

	// flags for sending/receiving data
	bool m_fSendingCameraPlacement = false;
	bool m_fReceivingCameraPlacement = false;

	bool m_fPendCameraPlacement = false;
	bool m_fAutoOpened = false;

	bool m_fHasReceivedSettings = false;

	std::chrono::system_clock::time_point m_msTimeClosed;
};

#endif // ! DREAM_VCAM_SYSTEM_H_

#ifndef DREAM_GARAGE_H_
#define DREAM_GARAGE_H_

#include "RESULT/EHM.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS.h"
#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"
#include "Primitives/version.h"

class DreamUIBar;
class DreamContentView;
class DreamBrowser;
class DreamControlView;
class DreamDesktopApp;
class DreamUserControlArea;
class DreamShareView;
class DreamDesktopApp;
class DreamGamepadCameraApp;
class DreamEnvironmentApp;
class UIStageProgram;
class UpdateMouthMessage;
class SkyboxScatterProgram;
class OGLProgramScreenFade;

#define MAX_PEERS 8

class DreamGarage : public DreamOS,
				    public Subscriber<SenseKeyboardEvent>,
					public Subscriber<SenseTypingEvent>,
					public DOSObserver
{
public:

	DreamGarage() {
		// empty
	}

	~DreamGarage() {
		// empty
	}

	RESULT SendHeadPosition();
	RESULT SendHandPosition();
	RESULT SendMouthSize();

	virtual bool UseInstallPath() override {
#if defined(PRODUCTION_BUILD) || defined(DEV_PRODUCTION_BUILD)
		return true;
#else
		return false;
#endif
	}

	virtual std::wstring GetDreamFolderPath() {
#ifdef PRODUCTION_BUILD
		return std::wstring(L"\\Dream\\");
#else DEV_PRODUCTION_BUILD
		return std::wstring(L"\\DreamDev\\");
#endif
	}

	// TODO: this is just a debug test temp
	//RESULT SendSwitchHeadMessage();

	virtual RESULT ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT DidFinishLoading() override;
	virtual RESULT SetupPipeline(Pipeline* pRenderPipeline) override;
	virtual RESULT Update(void) override;

	RESULT SetupMirrorPipeline(Pipeline *pRenderPipeline);

	virtual version GetDreamVersion() override;

	// Cloud Controller
	//RESULT InitializeCloudControllerCallbacks();

	RESULT SetRoundtablePosition(int index);
	RESULT SetRoundtablePosition(DreamPeerApp *pDreamPeer, int seatingPosition);

	// PeerConnectionObserver
	//virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	//virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDataMessage) override;
	//virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;
	//virtual RESULT OnAudioData(PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	//
	virtual RESULT OnNewSocketConnection(int seatPosition) override;

	// Cloud
	virtual RESULT OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) override;
	virtual RESULT OnNewDreamPeer(DreamPeerApp *pDreamPeer) override;
	virtual RESULT OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	//virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;

	// Environment
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnReceiveAsset(long userID) override;
	virtual RESULT OnStopSending() override;
	virtual RESULT OnStopReceiving() override;

	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override;

	// Desktop Sharing
	virtual RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) override;
	virtual RESULT OnShareAsset() override;
	virtual RESULT OnCloseAsset() override;

	// User Observer
	virtual RESULT OnDreamVersion(version dreamVersion) override;

	virtual RESULT OnLogin() override;
	virtual RESULT OnLogout() override;

	virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override;
	virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override;
	virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId);


	// DOS Observer
	virtual RESULT HandleDOSMessage(std::string& strMessage) override;

	// DreamGarage Messages
	RESULT HandleHeadUpdateMessage(PeerConnection* pPeerConnection, UpdateHeadMessage *pUpdateHeadMessage);
	RESULT HandleHandUpdateMessage(PeerConnection* pPeerConnection, UpdateHandMessage *pUpdateHandMessage);
	RESULT HandleMouthUpdateMessage(PeerConnection* pPeerConnection, UpdateMouthMessage *pUpdateMouthMessage);
	RESULT HandleUserAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage);

	//
	RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

	RESULT BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT BroadcastUpdateHandMessage(hand::HandState handState);
	RESULT BroadcastUpdateMouthMessage(float mouthSize);

	user* ActivateUser(long userId);

	virtual RESULT Exit(RESULT r) override;

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseTypingEvent *kbEvent) override;

private:
	//std::map<long, user*> m_peerUsers;
	// User Pool

	RESULT SetupUserModelPool();
	RESULT AllocateAndAssignUserModelFromPool(DreamPeerApp *pDreamPeer);
	user* FindUserModelInPool(DreamPeerApp *pDreamPeer);
	RESULT UnallocateUserModelFromPool(std::shared_ptr<DreamPeerApp> pDreamPeer);
	RESULT PendClearPeers();

	std::array<std::pair<DreamPeerApp*, user*>, MAX_PEERS> m_usersModelPool = { std::pair<DreamPeerApp*, user*>(nullptr, nullptr) };

private:
	RESULT AuthenticateFromStoredCredentials();

private:
	version m_versionDreamClient = version(DREAM_VERSION);

	bool m_fSeated = false;
	float m_tick = 0.0f;

	long m_pendingAssetReceiveUserID = -1;

	// TODO: should these be here
	bool m_fFirstLogin = true;
	bool m_fHasCredentials = false;
	bool m_fClearPeers = false;

	std::string m_strRefreshToken;
	std::string m_strAccessToken;
	UserController* m_pUserController;

	CameraNode* m_pAuxCamera = nullptr;

	// UI
	//ViewMatrix *m_pClippingView;
	UIStageProgram *m_pUIProgramNode = nullptr;
	std::vector<SkyboxScatterProgram*> m_skyboxProgramNodes;
	OGLProgramScreenFade *m_pScreenFadeProgramNode = nullptr;
	ProgramNode* m_pRenderEnvironmentProgramNode = nullptr;
	ProgramNode* m_pRefractionProgramNode = nullptr;
	ProgramNode* m_pReflectionProgramNode = nullptr;

	// For mirror
	ProgramNode* m_pRenderEnvironmentProgramNodeMirror = nullptr;
	ProgramNode* m_pRefractionProgramNodeMirror = nullptr;
	ProgramNode* m_pReflectionProgramNodeMirror = nullptr;

	DreamEnvironmentApp* m_pDreamEnvironmentApp = nullptr;
	DreamUserControlArea* m_pDreamUserControlArea = nullptr;
	DreamUserApp* m_pDreamUserApp = nullptr;
	DreamUIBar* m_pDreamUIBar = nullptr;
	DreamContentView* m_pDreamContentView = nullptr;
	DreamBrowser* m_pDreamBrowser = nullptr;
	DreamControlView* m_pDreamControlView = nullptr;
	DreamDesktopApp* m_pDreamDesktop = nullptr;
	DreamGamepadCameraApp* m_pDreamGamepadCameraApp = nullptr;
};

#endif	// DREAM_GARAGE_H_

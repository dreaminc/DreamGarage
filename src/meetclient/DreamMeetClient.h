#ifndef DREAM_GARAGE_H_
#define DREAM_GARAGE_H_

#include "core/ehm/RESULT.h"          // for RESULT

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "os/DreamOS.h"

#include "core/types/version.h"
#include "core/primitives/point.h"       // for point
#include "core/primitives/quaternion.h"  // for quaternion
#include "core/primitives/vector.h"      // for vector

#include "sandbox/Sandbox.h"     // for Sandbox, Sandbox::PipelineType

#include "array"                    // for array
#include "memory"                   // for shared_ptr
#include "utility"                  // for pair
#include "xstring"                  // for string, wstring
#include <stddef.h>                 // for size_t


class AudioDataMessage;
class CameraNode;
class DreamMessage;
class DreamPeerApp;
class EnvironmentAsset;
class EnvironmentShare;
class OGLProgram;
class PeerConnection;
class Pipeline;
class ProgramNode;
class UpdateHandMessage;
class UpdateHeadMessage;
class UserController;
class quad;
class texture;
class user;
struct HandState;
struct SenseKeyboardEvent;
struct SenseTypingEvent;

class DreamUIBarApp;
class DreamContentViewApp;
class DreamBrowser;
class UIControlView;
class DreamUserControlAreaApp;
class DreamGamepadCameraApp;
class DreamEnvironmentApp;
class UIStageProgram;
class UpdateMouthMessage;
class OGLProgramScreenFade;

class SkyboxScatterProgram;
class FogProgram;


#define MAX_PEERS 8

class DreamMeetClient : public DreamOS,
				    public Subscriber<SenseKeyboardEvent>,
					public Subscriber<SenseTypingEvent>,
					public DOSObserver
{
public:

	DreamMeetClient() {
		// empty
	}

	~DreamMeetClient() {
		// empty
	}

	RESULT SendHeadPosition();
	RESULT SendHandPosition();
	RESULT SendMouthSize();

	virtual bool UseInstallPath() override {
#if defined(PRODUCTION_BUILD) || defined(STAGING_BUILD)
		return true;
#else
		return false;
#endif
	}

	virtual std::wstring GetDreamFolderPath() {
		return std::wstring(L"\\Dream\\");

		// TODO: Understand the implications of this, we no longer have a DreamDev build now
//#ifdef PRODUCTION_BUILD
//		return std::wstring(L"\\Dream\\");
//#else STAGING_BUILD
//		return std::wstring(L"\\DreamDev\\");
//#endif
	}

	// TODO: this is just a debug test temp
	//RESULT SendSwitchHeadMessage();

	virtual RESULT ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT DidFinishLoading() override;
	virtual RESULT SetupPipeline(Pipeline* pRenderPipeline) override;
	virtual RESULT Update(void) override;

	// TODO: Pull pipeline type out of sandbox
	RESULT SetupMirrorPipeline(Pipeline *pRenderPipeline);
	virtual RESULT MakePipeline(CameraNode* pCamera, OGLProgram* &pRenderNode, OGLProgram* &pEndNode, Sandbox::PipelineType pipelineType) override;
	virtual version GetDreamVersion() override;

	virtual CameraNode *GetAuxCameraNode() override;

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
	virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;

	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;

	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override;

	virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;

	// Virtual Camera
	virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnSendCameraPlacement() override;
	virtual RESULT OnStopSendingCameraPlacement() override;
	virtual RESULT OnReceiveCameraPlacement(long userID) override;
	virtual RESULT OnStopReceivingCameraPlacement() override;

	virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override;

	virtual RESULT SaveCameraSettings(point ptPosition, quaternion qOrientation) override;

	// User Observer
	virtual RESULT OnDreamVersion(version dreamVersion) override;
	virtual RESULT OnAPIConnectionCheck(bool fIsConnected) override;

	virtual RESULT OnLogin() override;
	virtual RESULT OnLogout() override;
	virtual RESULT OnPendLogout() override;
	virtual RESULT OnSwitchTeams() override;
	RESULT PendSwitchTeams();

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
	RESULT SendUpdateHandMessage(long userID, HandState handState);

	RESULT BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT BroadcastUpdateHandMessage(HandState handState);
	RESULT BroadcastUpdateMouthMessage(float mouthSize);

	user* ActivateUser(long userId);

	virtual RESULT Exit(RESULT r) override;

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseTypingEvent *kbEvent) override;

	// Camera Sharing
	virtual texture* GetSharedCameraTexture() override;

	virtual RESULT GetDefaultVCamPlacement(point& ptPosition, quaternion& qOrientation) override;

	virtual bool IsCameraInUse() override;

private:
	//std::map<long, user*> m_peerUsers;
	// User Pool

	RESULT SetupUserModelPool();
	RESULT AllocateAndAssignUserModelFromPool(DreamPeerApp *pDreamPeer);
	user* FindUserModelInPool(DreamPeerApp *pDreamPeer);
	RESULT UnallocateUserModelFromPool(std::shared_ptr<DreamPeerApp> pDreamPeer);

	std::array<std::pair<DreamPeerApp*, user*>, MAX_PEERS> m_usersModelPool = { std::pair<DreamPeerApp*, user*>(nullptr, nullptr) };

private:
	RESULT AuthenticateFromStoredCredentials();

private:
	version m_versionDreamClient = version(DREAM_VERSION);

	bool m_fSeated = false;
	float m_tick = 0.0f;

	long m_pendingAssetReceiveUserID = -1;
	std::vector<std::shared_ptr<EnvironmentShare>> m_pPendingEnvironmentShares;

	bool m_fPendLogout = false;
	bool m_fPendSwitchTeams = false;
	bool m_fPendExit = false;

	// TODO: should these be here
	bool m_fFirstLogin = true;
	bool m_fHasCredentials = false;
	bool m_fInitHands = false;

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

	std::vector<FogProgram*> m_fogProgramNodes;
	std::vector<ProgramNode*> m_waterProgramNodes;

	// For mirror
	ProgramNode* m_pRenderEnvironmentProgramNodeMirror = nullptr;
	ProgramNode* m_pRefractionProgramNodeMirror = nullptr;
	ProgramNode* m_pReflectionProgramNodeMirror = nullptr;

	quad* m_pWaterQuad = nullptr;
	DreamEnvironmentApp* m_pDreamEnvironmentApp = nullptr;
	DreamUserControlAreaApp* m_pDreamUserControlArea = nullptr;
	DreamUIBarApp* m_pDreamUIBar = nullptr;
	DreamContentViewApp* m_pDreamContentView = nullptr;
	DreamBrowser* m_pDreamBrowser = nullptr;
	UIControlView* m_pDreamControlView = nullptr;
	DreamGamepadCameraApp* m_pDreamGamepadCameraApp = nullptr;

};

#endif	// DREAM_GARAGE_H_

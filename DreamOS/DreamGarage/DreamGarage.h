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

class DreamUIBar;
class DreamContentView;
class DreamBrowser;
class DreamControlView;
class DreamDesktopApp;
class DreamUserControlArea;
class DreamShareView;
class UIStageProgram;
class UpdateMouthMessage;

#define MAX_PEERS 8

class DreamGarage : public DreamOS,
				    public Subscriber<SenseKeyboardEvent>,
					public Subscriber<SenseTypingEvent>
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

	// TODO: this is just a debug test temp
	//RESULT SendSwitchHeadMessage();

	virtual RESULT ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT DidFinishLoading() override;
	virtual RESULT SetupPipeline(Pipeline* pRenderPipeline) override;
	virtual RESULT Update(void) override;

	// Cloud Controller
	//RESULT InitializeCloudControllerCallbacks();

	RESULT GetRoundtablePosition(int index, point &ptPosition, float &rotationAngle);
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

	// Desktop Sharing
	virtual RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) override;
	virtual RESULT OnShareAsset() override;
	virtual RESULT OnCloseAsset() override;

	// DreamGarage Messages
	RESULT HandleHeadUpdateMessage(PeerConnection* pPeerConnection, UpdateHeadMessage *pUpdateHeadMessage);
	RESULT HandleHandUpdateMessage(PeerConnection* pPeerConnection, UpdateHandMessage *pUpdateHandMessage);
	RESULT HandleMouthUpdateMessage(PeerConnection* pPeerConnection, UpdateMouthMessage *pUpdateMouthMessage);
	RESULT HandleAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage);

	//
	RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

	RESULT BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT BroadcastUpdateHandMessage(hand::HandState handState);
	RESULT BroadcastUpdateMouthMessage(float mouthSize);

	user* ActivateUser(long userId);

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

	std::array<std::pair<DreamPeerApp*, user*>, MAX_PEERS> m_usersModelPool = { std::pair<DreamPeerApp*, user*>(nullptr, nullptr) };


private:
	bool m_fSeated = false;
	float m_tick = 0.0f;
	float m_seatPositioningRadius = 4.0f;
	std::vector<int> m_seatLookup = { 4, 1, 3, 2, 5, 0 };
	float m_initialAngle = 90.0f;
	float m_keepOutAngle = 5.0f;

	bool m_fShouldUpdateAppComposites = false;

	long m_pendingAssetReceiveUserID = -1;

	// UI
	//ViewMatrix *m_pClippingView;
	UIStageProgram *m_pUIProgramNode;

	std::shared_ptr<DreamUIBar> m_pDreamUIBar = nullptr;
	std::shared_ptr<DreamContentView> m_pDreamContentView = nullptr;
	std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;
	std::shared_ptr<DreamControlView> m_pDreamControlView = nullptr;
	std::shared_ptr<DreamDesktopApp> m_pDreamDesktop = nullptr;
	std::shared_ptr<DreamUserControlArea> m_pDreamUserControlArea = nullptr;
};

#endif	// DREAM_GARAGE_H_

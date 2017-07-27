#ifndef DREAM_GARAGE_H_
#define DREAM_GARAGE_H_

#include "RESULT/EHM.h"
#include "DreamConsole/DreamConsole.h"

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

class DreamGarage : public DreamOS, 
				    public Subscriber<SenseKeyboardEvent>, 
					public Subscriber<SenseTypingEvent>, 
					public Subscriber<CmdPromptEvent>,	// TODO: Remove this
					public CloudController::PeerConnectionObserver,
					public CloudController::EnvironmentObserver
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

	// TODO: this is just a debug test temp
	RESULT SendSwitchHeadMessage();

	virtual RESULT ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT SetupPipeline(Pipeline* pRenderPipeline) override;
	virtual RESULT Update(void) override;

	// Cloud Controller
	//RESULT InitializeCloudControllerCallbacks();

	RESULT GetRoundtablePosition(int index, point &ptPosition, float &rotationAngle);
	RESULT SetRoundtablePosition(int index);

	// PeerConnectionObserver
	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDataMessage) override;
	virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;

	// EnvironmentObserver
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;

	user* ActivateUser(long userId);

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseTypingEvent *kbEvent) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:
	std::map<long, user*> m_peerUsers;
	std::vector<user*> m_usersPool;

	bool m_fSeated = false;
	float m_tick = 0.0f;
	float m_seatPositioningRadius = 3.5f;
	std::vector<int> m_seatLookup = { 4, 1, 3, 2, 5, 0 };
	float m_initialAngle = 90.0f;
	float m_keepOutAngle = 5.0f;
	
	// UI
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;
	std::shared_ptr<DreamContentView> m_pDreamContentView;
	std::shared_ptr<DreamBrowser> m_pDreamBrowser;
	std::shared_ptr<DreamControlView> m_pDreamControlView;
};

#endif	// DREAM_GARAGE_H_

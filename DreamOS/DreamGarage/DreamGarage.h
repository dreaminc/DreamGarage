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

class DreamGarage : public DreamOS, public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseTypingEvent>, public Subscriber<CmdPromptEvent> {
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
	virtual RESULT Update(void) override;

	// Cloud Controller
	RESULT InitializeCloudControllerCallbacks();

	RESULT SetRoundtablePosition(float angle);
	RESULT HandlePeersUpdate(long index);

	RESULT HandleDataMessage(long senderUserID, Message *pDataMessage);
	RESULT HandleUpdateHeadMessage(long senderUserID, UpdateHeadMessage *pUpdateHeadMessage);
	RESULT HandleUpdateHandMessage(long senderUserID, UpdateHandMessage *pUpdateHandMessage);
	RESULT HandleAudioData(long senderUserID, AudioDataMessage *pAudioDataMessage);
	RESULT HandleOnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);

	user* ActivateUser(long userId);

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseTypingEvent *kbEvent) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:

	std::map<long, user*> m_peerUsers;
	std::vector<user*> m_usersPool;

	bool	m_isSeated = false;
	float tick = 0.0f;
	
	// UI
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;

	std::shared_ptr<DreamContentView> m_pDreamContentView;
};

#endif	// DREAM_GARAGE_H_

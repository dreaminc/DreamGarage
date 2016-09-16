#ifndef DREAM_GARAGE_H_
#define DREAM_GARAGE_H_

#include "RESULT/EHM.h"
#include "DreamConsole/DreamConsole.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS/DreamOS.h"
#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

class DreamGarage : public DreamOS, public Subscriber<SenseKeyboardEvent>, public Subscriber<CmdPromptEvent> {
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

	RESULT LoadScene();
	RESULT Update(void);

	// Cloud Controller
	RESULT InitializeCloudControllerCallbacks();

	RESULT HandleDataMessage(long senderUserID, Message *pDataMessage);
	RESULT HandleUpdateHeadMessage(long senderUserID, UpdateHeadMessage *pUpdateHeadMessage);
	RESULT HandleUpdateHandMessage(long senderUserID, UpdateHandMessage *pUpdateHandMessage);

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *kbEvent) override;

private:

	user *m_pPeerUser;
	//composite *m_pPeerUser;
	//volume *m_pPeerUser;

	sphere *m_pSphere;
	FlatContext *m_pContext;
	quad *m_pQuad;
	std::shared_ptr<DebugData> m_spherePosition;
};


#endif	// DREAM_GARAGE_H_
#ifndef DREAM_TEST_APP_H_
#define DREAM_TEST_APP_H_

#include "RESULT/EHM.h"
#include "DreamConsole/DreamConsole.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS/DreamOS.h"
#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

class DreamTestApp : public DreamOS, public Subscriber<SenseKeyboardEvent>, public Subscriber<CmdPromptEvent> {
public:
	DreamTestApp() {
		// empty
	}

	~DreamTestApp() {
		// empty
	}

	RESULT LoadScene();
	RESULT Update(void);

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(CollisionObjectEvent *oEvent) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:
	user *m_pPeerUser;
};


#endif	// DREAM_TEST_APP_H_
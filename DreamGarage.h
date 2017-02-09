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
#include "Sense/SenseController.h"

#include "DreamUIBar.h"

#include "Cloud/WebBrowser/browser.h"

// temporary browser management for testring
class Browsers {
public:
	void Init(composite* composite);
	void Update();
	std::string CreateNewBrowser(unsigned int width, unsigned int height, const std::string& url);
	WebBrowserController*	GetBrowser(const std::string& id);
	void SetKeyFocus(const std::string& id);

	void OnKey(unsigned int scanCode, char16_t chr);

private:
	composite*	m_composite;

	std::unique_ptr<WebBrowserService> m_BrowserService = nullptr;

	struct Browser {
		WebBrowserController*		controller;
		std::shared_ptr<texture>	texture;
		std::shared_ptr<quad>		quad;
	};

	// std::string indicates the id of the browser
	std::map<std::string, Browser> m_Browsers;

	WebBrowserController* m_browserInKeyFocus = nullptr;
};

class DreamGarage : public DreamOS, public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseTypingEvent>, public Subscriber<SenseControllerEvent>, public Subscriber<CmdPromptEvent> {

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

	RESULT HandlePeersUpdate(long index);
	RESULT HandleDataMessage(long senderUserID, Message *pDataMessage);
	RESULT HandleUpdateHeadMessage(long senderUserID, UpdateHeadMessage *pUpdateHeadMessage);
	RESULT HandleUpdateHandMessage(long senderUserID, UpdateHandMessage *pUpdateHandMessage);
	RESULT HandleAudioData(long senderUserID, AudioDataMessage *pAudioDataMessage);

	user*	ActivateUser(long userId);

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseTypingEvent *kbEvent) override;

	// SenseControllerEventSubscriber
	virtual RESULT Notify(SenseControllerEvent *event) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:

	//user *m_pPeerUser;

	std::map<long, user*> m_peerUsers;
	std::vector<user*> m_usersPool;

	//composite *m_pPeerUser;
	//volume *m_pPeerUser;

	sphere *m_pSphere;
	FlatContext *m_pContext;
	quad *m_pQuad;
	std::shared_ptr<DebugData> m_spherePosition;

	bool	m_isSeated = false;
	float tick = 0.0f;
	

	DreamUIBar *m_pDreamUIBar;
	//
	//temporarily hardcoded
	std::map<std::string, std::vector<std::string>> m_menu;
	std::stack<std::string> m_menuPath;
	std::shared_ptr<texture> m_pTestIcon;

	Browsers m_browsers;
};

#endif	// DREAM_GARAGE_H_

#ifndef DREAM_SETTINGS_APP_H_
#define DREAM_SETTINGS_APP_H_

#define SCALE_TICK 0.003f;

#include "DreamApp.h"
#include "DreamOS.h"
#include "Primitives/Subscriber.h"

class DreamUserHandle;
class DreamControlView;
class DreamBrowser;
struct UIEvent;

class DreamSettingsApp : public DreamApp<DreamSettingsApp>, public Subscriber<UIEvent>, public Subscriber<SenseControllerEvent>
{
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamSettingsApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamSettingsApp();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr);
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr);
	virtual RESULT Update(void *pContext = nullptr);
	virtual RESULT Shutdown(void *pContext = nullptr);

protected:
	static DreamSettingsApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	RESULT InitializeSettingsForm(std::string strURL);
	RESULT Show();
	RESULT Hide();

public:
	virtual RESULT Notify(UIEvent *pUIEvent) override;
	virtual RESULT Notify(SenseControllerEvent *pEvent) override;

private:
	WebBrowserPoint GetRelativePointofContact(point ptContact);

private:
	DreamUserApp* m_pUserApp = nullptr;

	std::shared_ptr<DreamControlView> m_pFormView = nullptr;
	std::shared_ptr<DreamBrowser> m_pForm = nullptr;

	std::shared_ptr<UIKeyboard> m_pKeyboard = nullptr;

	std::string m_strURL;

	float m_scaleTick = SCALE_TICK;

	//TODO: should be temporary
	bool m_fRespondToController = false;
	bool m_fLeftTriggerDown = false;
	bool m_fRightTriggerDown = false;
	bool m_fInitBrowser = false;

	//TODO: temp
	std::shared_ptr<texture> m_pTestTexture = nullptr;
};

#endif // ! DREAM_SETTINGS_APP_H_

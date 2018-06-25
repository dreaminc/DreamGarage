#ifndef DREAM_SETTINGS_APP_H_
#define DREAM_SETTINGS_APP_H_

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
	friend class DreamUserControlArea;
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

	// settings TODO: userApp
	float m_depth = 0.0f;
	float m_height = 0.0f;
	float m_scale = 1.0f;

	//TODO: should be temporary
	bool m_fRespondToController = false;
};

#endif // ! DREAM_SETTINGS_APP_H_

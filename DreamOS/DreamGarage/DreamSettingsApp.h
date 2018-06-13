#ifndef DREAM_SETTINGS_APP_H_
#define DREAM_SETTINGS_APP_H_

#include "DreamApp.h"
#include "DreamOS.h"

class DreamUserHandle;

class DreamSettingsApp : public DreamApp<DreamSettingsApp>
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
	RESULT InitializeWithParent(DreamUserControlArea *pParent);
	RESULT InitializeSettingsForm(std::string strURL);
	RESULT Show();

private:
	std::shared_ptr<DreamUserHandle> m_pUserHandle = nullptr;

	std::shared_ptr<DreamBrowser> m_pSettingsForm = nullptr;

};

#endif // ! DREAM_SETTINGS_APP_H_

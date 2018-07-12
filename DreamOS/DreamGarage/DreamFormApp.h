#ifndef DREAM_FORM_APP_H_
#define DREAM_FORM_APP_H_

#include "DreamApp.h"
#include "DreamOS.h"

class DreamFormApp : public DreamApp<DreamFormApp>
{
	friend class DreamAppManager;

public:
	DreamFormApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamFormApp();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr);
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr);
	virtual RESULT Update(void *pContext = nullptr);
	virtual RESULT Shutdown(void *pContext = nullptr);

protected:
	static DreamSettingsApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);
};

#endif // ! DREAM_FORM_APP_H_

#ifndef DREAM_LOGIN_APP_H_
#define DREAM_LOGIN_APP_H_

#include "DreamGarage/DreamFormApp.h"

class DreamUserHandle;
class DreamControlView;

class DreamLoginApp : public DreamFormApp
{
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamLoginApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamLoginApp();

protected:
	static DreamLoginApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);
};

#endif // ! DREAM_LOGIN_APP_H_
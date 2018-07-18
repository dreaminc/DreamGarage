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

	// main purpose of DreamLoginApp is to handle login-associated form events
public:
	RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) override;
	RESULT HandleDreamFormSetEnvironmentId(int environmentId) override;

protected:
	static DreamLoginApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);
};

#endif // ! DREAM_LOGIN_APP_H_
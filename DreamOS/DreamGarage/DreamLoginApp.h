#ifndef DREAM_LOGIN_APP_H_
#define DREAM_LOGIN_APP_H_

#include "DreamGarage/DreamFormApp.h"
#include "Sandbox/CredentialManager.h"



class DreamUserHandle;
class DreamControlView;

typedef enum CredentialType {
	CREDENTIAL_REFRESH_TOKEN,
	CREDENTIAL_ACCESS_TOKEN,
	CREDENTIAL_LAST_LOGIN,
	CREDENTIAL_LAST_ENVIRONMENT
} CREDENTIAL_TYPE;

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

public:
	RESULT GetCredential(CredentialType type, std::string& strCredentialValue);
	RESULT SetCredential(CredentialType type, std::string& strCredentialValue);

private:
	CredentialManager::type GetCredentialManagerType(CredentialType type);
	std::wstring GetCredentialManagerString(CredentialType type);

public:
	bool IsFirstLaunch();
	bool HasStoredCredentials(std::string& strRefreshToken, std::string& strAccessToken);

	RESULT SetLaunchDate(); // always set to now

public:
	std::wstring wstrAppName = L"Dream";
	std::wstring wstrDevelop = L"Develop";
	std::wstring wstrClient = L".Client.";

	std::wstring wstrRefreshToken = L"RefreshToken";
	std::wstring wstrAccessToken = L"AccessToken";
	std::wstring wstrLastLogin = L"LastLogin";
	std::wstring wstrEnvironmentId = L"EnvironmentID";

protected:
	static DreamLoginApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);
};

#endif // ! DREAM_LOGIN_APP_H_
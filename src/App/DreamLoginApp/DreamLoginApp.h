#ifndef DREAM_LOGIN_APP_H_
#define DREAM_LOGIN_APP_H_

#include "DreamGarage/DreamFormApp.h"
#include "Sandbox/CredentialManager.h"

class UIControlView;

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
	RESULT HandleDreamFormSuccess() override;
	RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& strAccessToken) override;
	RESULT HandleDreamFormSetEnvironmentId(int environmentId) override;

public:
	RESULT GetCredential(CredentialType type, std::string& strCredentialValue);
	RESULT SetCredential(CredentialType type, std::string& strCredentialValue);
	RESULT ClearCredential(CredentialType type);

private:
	CredentialManager::type GetCredentialManagerType(CredentialType type);
	std::wstring GetCredentialManagerString(CredentialType type);

public:
	bool IsFirstLaunch();
	bool HasStoredCredentials(std::string& strRefreshToken, std::string& strAccessToken);

	RESULT SetLaunchDate(); // always set to now
	std::string& GetAccessToken();
	RESULT SetAccessToken(std::string strAccessToken);
	RESULT SaveTokens();
	RESULT ClearTokens();

	virtual std::string GetSuccessString() override;

public:
	std::string m_strSuccess = "DreamLoginApp.OnSuccess";

	std::wstring m_wstrAppName = L"Dream";
	std::wstring m_wstrDevelop = L"Develop";
	std::wstring m_wstrClient = L".Client.";

	std::wstring m_wstrRefreshToken = L"RefreshToken";
	std::wstring m_wstrAccessToken = L"AccessToken";
	std::wstring m_wstrLastLogin = L"LastLogin";
	std::wstring m_wstrEnvironmentId = L"EnvironmentID";

private:
	std::string m_strRefreshToken;
	std::string m_strAccessToken;
	std::string m_strLastEnvironmentId;

protected:
	static DreamLoginApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);
};

#endif // ! DREAM_LOGIN_APP_H_
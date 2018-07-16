#ifndef WIN_64_CREDENTIAL_MANAGER_H_
#define WIN_64_CREDENTIAL_MANAGER_H_

#include "RESULT/EHM.h"
#include "Sandbox/CredentialManager.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64CredentialManager.h
// Dream OS Win64 sandbox credential manager

#include <windows.h>

class Win64CredentialManager : public CredentialManager {
public:
	Win64CredentialManager();

protected:
	virtual RESULT SaveCredential(std::wstring wstrKey, std::string strField) override;
	virtual RESULT GetCredential(std::wstring wstrKey, std::string &strOut) override;
	virtual RESULT RemoveCredential(std::wstring wstrKey);

private:

};

#endif // ! WIN_64_CREDENTIAL_MANAGER_H_

#ifndef WIN_64_CREDENTIAL_MANAGER_H_
#define WIN_64_CREDENTIAL_MANAGER_H_

#include "core/ehm/EHM.h"

// Dream Win64 Sandbox
// dos/src/sandbox/win64/Win64CredentialManager.h

// Dream OS Win64 sandbox credential manager

#include <windows.h>

#include "sandbox/CredentialManager.h"

class Win64CredentialManager : public CredentialManager {
public:
	Win64CredentialManager();

protected:
	virtual RESULT SetKeyValue(std::wstring wstrKey, std::string strField, CredentialManager::type credType, bool fOverwrite) override;
	virtual RESULT GetKeyValue(std::wstring wstrKey, std::string &strOut, CredentialManager::type credType) override;
	virtual RESULT RemoveKeyValue(std::wstring wstrKey, CredentialManager::type credType) override;

private:

};

#endif // ! WIN_64_CREDENTIAL_MANAGER_H_

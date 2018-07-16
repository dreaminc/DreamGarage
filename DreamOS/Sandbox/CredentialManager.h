#ifndef CREDENTIAL_MANAGER_H_
#define CREDENTIAL_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/CredentialManager.h
// Dream OS Sandbox credential manager

class CredentialManager {

public:
	// virtuals
	virtual RESULT SaveCredential(std::wstring wstrKey, std::string strField) = 0;
	virtual RESULT GetCredential(std::wstring wstrKey, std::string &strOut) = 0;
	virtual RESULT RemoveCredential(std::wstring wstrKey) = 0;

};

#endif // ! CREDENTIAL_MANAGER_H_

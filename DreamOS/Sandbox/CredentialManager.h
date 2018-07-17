#ifndef CREDENTIAL_MANAGER_H_
#define CREDENTIAL_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/CredentialManager.h
// Dream OS Sandbox credential manager

typedef enum Credential_Type {
	CREDENTIAL_INVALID,
	CREDENTIAL_GENERIC,				//CRED_TYPE_GENERIC 1
	CREDENTIAL_DOMAIN_PASSWORD,		//CRED_TYPE_DOMAIN_PASSWORD 2
	CREDENTIAL_DOMAIN_CERTIFICATE	//CRED_TYPE_DOMAIN_CERTIFICATE 3
} CREDENTIAL_TYPE;

class CredentialManager {

public:
	CredentialManager() {};	// empty
	~CredentialManager() {};

public:
	// virtuals
	virtual RESULT SaveCredential(std::wstring wstrKey, std::string strField, CREDENTIAL_TYPE credType, bool fOverwrite) = 0;
	virtual RESULT GetCredential(std::wstring wstrKey, std::string &strOut, CREDENTIAL_TYPE credType) = 0;
	virtual RESULT RemoveCredential(std::wstring wstrKey, CREDENTIAL_TYPE credType) = 0;

};

#endif // ! CREDENTIAL_MANAGER_H_

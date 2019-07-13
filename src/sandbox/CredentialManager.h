#ifndef CREDENTIAL_MANAGER_H_
#define CREDENTIAL_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/CredentialManager.h
// Dream OS Sandbox credential manager

class CredentialManager {

public:

	enum type {
		CREDENTIAL_RESERVED,
		CREDENTIAL_GENERIC,				//CRED_TYPE_GENERIC 1
		CREDENTIAL_DOMAIN_PASSWORD,		//CRED_TYPE_DOMAIN_PASSWORD 2
		CREDENTIAL_DOMAIN_CERTIFICATE,	//CRED_TYPE_DOMAIN_CERTIFICATE 3
		CREDENTIAL_INVALID
	};

	CredentialManager() {};	// empty
	~CredentialManager() {};

public:
	// virtuals
	virtual RESULT SetKeyValue(std::wstring wstrKey, std::string strField, type credType, bool fOverwrite) = 0;
	virtual RESULT GetKeyValue(std::wstring wstrKey, std::string &strOut, type credType) = 0;
	virtual RESULT RemoveKeyValue(std::wstring wstrKey, type credType) = 0;

};

#endif // ! CREDENTIAL_MANAGER_H_

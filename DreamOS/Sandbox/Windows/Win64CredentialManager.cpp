#include "Win64CredentialManager.h"

#include <wincred.h>

Win64CredentialManager::Win64CredentialManager() :
	CredentialManager()	// Call super
{
	// empty
}


RESULT Win64CredentialManager::GetCredential(std::wstring wstrKey, std::string &strOut) {
	RESULT r = R_PASS;

	PCREDENTIALW pcred;
	CBM(CredReadW(wstrKey.c_str(), CRED_TYPE_GENERIC, 0, &pcred), "Error finding credential");	// flags must be 0
	//DWORD dwError = GetLastError();	// Not found with targetname, no such logon session, invalid flags - must be 0

	strOut = (char*)pcred->CredentialBlob;
		
Error:
	CredFree(pcred); // must free memory allocated by CredRead()
	return r;
}

RESULT Win64CredentialManager::SaveCredential(std::wstring wstrKey, std::string strField) {
	RESULT r = R_PASS;

	DWORD cbCreds = (unsigned long)strField.length();	// This is fine since we're storing wchar_t entities

	CREDENTIALW cred = { 0 };
	cred.Type = CRED_TYPE_GENERIC;
	cred.TargetName = (LPWSTR)wstrKey.c_str();
	cred.CredentialBlobSize = cbCreds;
	cred.CredentialBlob = (LPBYTE)strField.c_str();
	cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
	//cred.UserName = (LPWSTR)L"user";	// this is stored as plain text, but it can be blank

	CBM(CredWriteW(&cred, 0), "Failed to save credential");
	
Error:
	return r;
}

RESULT Win64CredentialManager::RemoveCredential(std::wstring wstrKey) {
	RESULT r = R_PASS;

	CBM(CredDeleteW(wstrKey.c_str(), CRED_TYPE_GENERIC, 0), "Error removing credential");

Error:
	return r;
}


#include "Win64CredentialManager.h"
#include "Core/Utilities.h"
#include <wincred.h>

Win64CredentialManager::Win64CredentialManager()
{
	// empty
}


RESULT Win64CredentialManager::GetKeyValue(std::wstring wstrKey, std::string &strOut, CredentialManager::type credType) {
	RESULT r = R_PASS;

	PCREDENTIALW pcred = { 0 };
	CBM(CredReadW(wstrKey.c_str(), credType, 0, &pcred), "Error getting credential: 0x%x", GetLastError());
	// Not found with TargetName, no such logon session, invalid flags - must be 0

	strOut.assign((char*)pcred->CredentialBlob, pcred->CredentialBlobSize);
		
Error:
	CredFree(pcred); // must free memory allocated by CredRead()
	return r;
}

RESULT Win64CredentialManager::SetKeyValue(std::wstring wstrKey, std::string strField, CredentialManager::type credType, bool fOverwrite) {
	RESULT r = R_PASS;

	PCREDENTIALW pcred = { 0 };
	bool fAlreadyExists = CredReadW(wstrKey.c_str(), credType, 0, &pcred);
	
	if (fOverwrite || !fAlreadyExists) {	// only write cred if it doesn't already exist, or we want to overwrite it
		DWORD cbCreds = (unsigned long)strField.length();	// This is fine since we're storing wchar_t entities

		CREDENTIALW cred = { 0 };
		cred.Type = credType;
		cred.TargetName = (LPWSTR)wstrKey.c_str();
		cred.CredentialBlobSize = cbCreds;
		cred.CredentialBlob = (LPBYTE)strField.c_str();
		cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
		//cred.UserName = (LPWSTR)L"user";	// this is stored as plain text, but it can be blank

		CBM(CredWriteW(&cred, 0), "Failed to save credential: 0x%x", GetLastError());
	}
	
Error:
	return r;
}

RESULT Win64CredentialManager::RemoveKeyValue(std::wstring wstrKey, CredentialManager::type credType) {
	RESULT r = R_PASS;

	CBM(CredDeleteW(wstrKey.c_str(), credType, 0), "Error removing credential: 0x%x", GetLastError());

Error:
	return r;
}


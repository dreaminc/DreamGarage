#include "DreamLoginApp.h"
#include "DreamControlView/UIControlView.h"
#include <chrono>

DreamLoginApp::DreamLoginApp(DreamOS *pDreamOS, void *pContext) :
	DreamFormApp(pDreamOS, pContext)
{
	// empty
}

DreamLoginApp::~DreamLoginApp() {
	RESULT r = R_PASS;

	CR(DreamFormApp::Shutdown());
	CR(Shutdown());

Error:
	return;
}

DreamLoginApp* DreamLoginApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamLoginApp *pDreamApp = new DreamLoginApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamLoginApp::HandleDreamFormSuccess() {
	RESULT r = R_PASS;

	CR(DreamFormApp::HandleDreamFormSuccess());
	CR(GetDOS()->SendDOSMessage(m_strSuccess));

Error:
	return r;
}

RESULT DreamLoginApp::HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& strAccessToken) {

	RESULT r = R_PASS;

	m_strRefreshToken = strRefreshToken;
	m_strAccessToken = strAccessToken;

	auto pUserController = dynamic_cast<UserController*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
	CN(pUserController);
	CR(SetCredential(CREDENTIAL_REFRESH_TOKEN, strRefreshToken));

	CR(pUserController->SetAccessToken(strAccessToken));

Error:
	return r;
}

RESULT DreamLoginApp::HandleDreamFormSetEnvironmentId(int environmentId) {
	RESULT r = R_PASS;

	m_strLastEnvironmentId = std::to_string(environmentId);

	// deprecated
	/*
	auto pUserController = dynamic_cast<UserController*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
	CN(pUserController);
	pUserController->SetUserDefaultEnvironmentID(environmentId);
	pUserController->UpdateLoginState();

Error:
	//*/
	return r;
}

CredentialManager::type DreamLoginApp::GetCredentialManagerType(CredentialType type) {

	CredentialManager::type credentialType = CredentialManager::CREDENTIAL_INVALID;

	//TODO: none of the other CredentialManager types are used for now
	switch (type) {
	case CREDENTIAL_REFRESH_TOKEN:
	case CREDENTIAL_ACCESS_TOKEN:
	case CREDENTIAL_LAST_LOGIN:
	case CREDENTIAL_LAST_ENVIRONMENT:
		credentialType = CredentialManager::CREDENTIAL_GENERIC;
		break;
	}

	return credentialType;
}

std::wstring DreamLoginApp::GetCredentialManagerString(CredentialType type) {

	// the keys for the credential manager have the form:
	// "Dream.Client.<field>" in a production build
	// "DreamDevelop.Client.<field>" in a development build
	std::wstring wstrCredential = m_wstrAppName;
	
#ifndef PRODUCTION_BUILD
	wstrCredential += m_wstrDevelop;
#endif

	wstrCredential += m_wstrClient;

	switch (type) {
	case CREDENTIAL_REFRESH_TOKEN:
		wstrCredential += m_wstrRefreshToken;
		break;
	case CREDENTIAL_ACCESS_TOKEN: 
		wstrCredential += m_wstrAccessToken; 
		break;	
	case CREDENTIAL_LAST_LOGIN: 
		wstrCredential += m_wstrLastLogin; 
		break;
	case CREDENTIAL_LAST_ENVIRONMENT: 
		wstrCredential += m_wstrEnvironmentId; 
		break;
	}

	return wstrCredential;
}

RESULT DreamLoginApp::GetCredential(CredentialType type, std::string& strCredentialValue) {
	RESULT r = R_PASS;
	
	DOSLOG(INFO, "Retrieving Creds %d", type);
	CR(GetDOS()->GetCredential(GetCredentialManagerString(type), strCredentialValue, GetCredentialManagerType(type)));

Error:
	return r;
}

RESULT DreamLoginApp::SetCredential(CredentialType type, std::string& strCredentialValue) {
	RESULT r = R_PASS;

	// currently, always overwrite
	CR(GetDOS()->SaveCredential(GetCredentialManagerString(type), strCredentialValue, GetCredentialManagerType(type), true));

Error:
	return r;
}

RESULT DreamLoginApp::ClearCredential(CredentialType type) {
	RESULT r = R_PASS;

	DOSLOG(INFO, "Clearing cred %d", type);
	CR(GetDOS()->RemoveCredential(GetCredentialManagerString(type), GetCredentialManagerType(type)));

Error:
	return r;
}

bool DreamLoginApp::IsFirstLaunch() {

	RESULT r = R_PASS;
	std::string strLastLogin;
	DOSLOG(INFO, "Check if first time launching");
	//TODO: the last login value is not used for anything yet
	CR(GetCredential(CREDENTIAL_LAST_LOGIN, strLastLogin));

	return false;
Error:
	return true;
}

bool DreamLoginApp::HasStoredCredentials(std::string& strRefreshToken, std::string& strAccessToken) {
	RESULT r = R_PASS;

	DOSLOG(INFO, "Have stored credentials");
	CR(GetCredential(CREDENTIAL_REFRESH_TOKEN, strRefreshToken));
	//CR(GetCredential(CREDENTIAL_ACCESS_TOKEN, strAccessToken));

	return true;
Error:
	return false;
}

RESULT DreamLoginApp::SetLaunchDate() {
	RESULT r = R_PASS;

	// by default, now() tracks Unix time, which is an approximation of UTC
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();

	// conversions to get a string from now()
	//std::time_t tCurrentTime = std::chrono::system_clock::to_time_t(currentTime);
	//std::string strCurrentTime = std::ctime(&tCurrentTime);

	std::string strCurrentTime = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count());

	DOSLOG(INFO, "Setting launch date");
	CR(SetCredential(CREDENTIAL_LAST_LOGIN, strCurrentTime));

Error:
	return r;
}

std::string& DreamLoginApp::GetAccessToken() {
	return m_strAccessToken;
}

RESULT DreamLoginApp::SetAccessToken(std::string strAccessToken) {
	RESULT r = R_PASS;

	// update with new access token
	m_strAccessToken = strAccessToken;

//Error:
	return r;
}

RESULT DreamLoginApp::SaveTokens() {
	RESULT r = R_PASS;

	// probably don't need to save access token, because the app should always try to get a new one
	// with the saved refresh token
	CR(SetCredential(CREDENTIAL_REFRESH_TOKEN, m_strRefreshToken));

Error:
	return r;
}

RESULT DreamLoginApp::ClearTokens() {
	RESULT r = R_PASS;

	m_strRefreshToken = "";
	m_strAccessToken = "";

	CR(ClearCredential(CREDENTIAL_REFRESH_TOKEN));
//	CR(ClearCredential(CREDENTIAL_ACCESS_TOKEN));

Error:
	return r;
}

std::string DreamLoginApp::GetSuccessString() {
	return m_strSuccess;
}

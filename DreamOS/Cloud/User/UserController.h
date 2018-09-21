#ifndef USER_CONTROLLER_H_
#define USER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/User/UserController.h
// The User Controller 

#include "Cloud/Controller.h"

#include <string>

#include "Cloud/User/User.h"
#include "Cloud/User/TwilioNTSInformation.h"

#include "Cloud/ControllerProxy.h"

#include "json.hpp"

class UserControllerObserver;

struct LoginState {
	unsigned fFirstLaunch : 1;
	unsigned fHasCredentials : 1;
	unsigned fHasAccessToken : 1;
	unsigned fHasEnvironmentId : 1;
	unsigned fHasUserProfile : 1;
	unsigned fHasTwilioInformation : 1;
	unsigned fPendingConnect : 1;
};

class UserControllerProxy : public ControllerProxy {
public:
	virtual std::string GetUserToken() = 0;
	virtual std::string GetPeerScreenName(long peerUserID) = 0;

	virtual RESULT RequestGetSettings(std::wstring wstrHardwareID, std::string strHMDType) = 0;
	virtual RESULT RequestSetSettings(std::wstring wstrHardwareID, std::string strHMDType, float yOffset, float zOffset, float scale) = 0;
};

// TODO: This is actually a UserController - so change the name of object and file
class UserController : public Controller, public UserControllerProxy {
public:
	enum class UserMethod {
		// old
		LOGIN,
		OTK_LOGIN,
		LOAD_PROFILE,
		LOAD_TWILIO_NTS_INFO,

		// new
		GET_DREAM_VERSION,
		GET_FORM,
		GET_ACCESS_TOKEN,
		SETTINGS,
		TEAMS,
		INVALID
	};

public:
	UserController(Controller* pParentController);
	~UserController();

	RESULT Initialize();

	// Read username and password from file and login, get a token
	RESULT LoginFromFilename(const std::wstring& file);
	RESULT LoginFromCommandline();
	
	// obsolete. to be removed
	//RESULT Login_Json(const std::wstring& strFilename);
	
	// Loads the user profile using the token
	RESULT LoadProfile();
	RESULT GetPeerProfile(long peerUserID);
	RESULT LoadTwilioNTSInformation();

	bool IsLoggedIn();
	RESULT SetIsLoggedIn(bool fLoggedIn);

	// UserControllerProxy
	UserControllerProxy* GetUserControllerProxy();
	virtual std::string GetUserToken() override;

	virtual std::string GetPeerScreenName(long peerUserID) override;
	int GetPeerAvatarModelID(long peerUserID);
	std::string GetPeerProfilePhotoURL(long peerUserID);

	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) { return R_NOT_IMPLEMENTED; }

private:
	std::string GetMethodURI(UserMethod userMethod);

// User Settings
public:
	RESULT HandleEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudMessage);

	RESULT OnGetSettings(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnSetSettings(std::shared_ptr<CloudMessage> pCloudMessage);

	virtual RESULT RequestGetSettings(std::wstring wstrHardwareID, std::string strHMDType) override;
	virtual RESULT RequestSetSettings(std::wstring wstrHardwareID, std::string strHMDType, float yOffset, float zOffset, float scale) override;


// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
//private:
public:
	RESULT Login(std::string& strUsername, std::string& strPassword);
	RESULT Logout();
	RESULT LoginWithOTK(std::string& strOTK, long& environmentID);

	RESULT SwitchTeam();
	RESULT RequestSwitchTeam(std::string strTeamID);

	long GetUserDefaultEnvironmentID();
	RESULT SetUserDefaultEnvironmentID(long environmentId);
	RESULT SetAccessToken(std::string strAccessToken);
	User GetUser();
	TwilioNTSInformation GetTwilioNTSInformation();

	long GetUserID() { return m_user.GetUserID(); }

// new login flow api calls
public:
//	RESULT GetForm(std::string& strFormKey, std::string& strURL);
	RESULT GetFormURL(std::string& strFormKey);
	void OnFormURL(std::string&& strResponse);

	RESULT GetAccessToken(std::string& strRefreshToken);
	void OnAccessToken(std::string&& strResponse);

	RESULT GetSettings(std::string& strAccessToken);
	void OnGetApiSettings(std::string&& strResponse);

	RESULT SetSettings(std::string& strAccessToken, float height, float depth, float scale);
	void OnSetApiSettings(std::string&& strResponse);

	RESULT GetTeam(std::string& strAccessToken, std::string strTeamID = "");
	void OnGetTeam(std::string&& strResponse);

	RESULT RequestUserProfile(std::string& strAccessToken);
	void OnUserProfile(std::string&& strResponse);

	RESULT RequestTwilioNTSInformation(std::string& strAccessToken);
	void OnTwilioNTSInformation(std::string&& strResponse);

	RESULT RequestDreamVersion();
	void OnDreamVersion(std::string&& strResponse);

// basic http error handling
private:
	RESULT GetResponseData(nlohmann::json& jsonData, nlohmann::json jsonResponse, int& statusCode);

public:
	RESULT UpdateLoginState();
	RESULT ClearLoginState();
	std::string GetSavedAccessToken();

private:
	LoginState m_loginState = { 0 };

public:
	class UserControllerObserver {
	public:
		// socket methods
		virtual RESULT OnGetSettings(float height, float depth, float scale) = 0;
		virtual RESULT OnSetSettings() = 0;
		virtual RESULT OnLogin() = 0;
		virtual RESULT OnLogout() = 0;
		virtual RESULT OnSwitchTeams() = 0;

		// api methods
		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) = 0;
		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) = 0;
		virtual RESULT OnDreamVersion(version dreamVersion) = 0;
	};

	RESULT RegisterUserControllerObserver(UserControllerObserver* pUserControllerObserver);

private:
	bool m_fLoggedIn = false;

	bool m_fSwitchingTeams = false;
	std::string m_strPendingTeamID = "";

	std::string	m_strToken;
	std::string m_strPeerScreenName;
	long m_avatarModelId = -1;
	std::string m_strProfilePhotoURL;

	User m_user = User();
	long m_defaultEnvironmentId = -1; // used in the case m_user is not initialized
	std::string m_strAccessToken;

	TwilioNTSInformation m_twilioNTSInformation = TwilioNTSInformation();

	UserControllerObserver *m_pUserControllerObserver;
};

#endif	// ! USER_CONTROLLER_H_
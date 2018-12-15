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
class point;
class quaternion;

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

	virtual RESULT RequestFormURL(std::string& strFormKey) = 0;

	// settings for camera controls
	virtual RESULT RequestGetSettings(std::string& strAccessToken) = 0;
	virtual RESULT RequestSetSettings(std::string& strAccessToken, point ptPosition, quaternion qOrientation) = 0;

	// login flow api requests
	virtual RESULT RequestAccessToken(std::string& strRefreshToken) = 0;
	virtual RESULT RequestTeam(std::string& strAccessToken, std::string strTeamID = "") = 0;
	virtual RESULT RequestUserProfile(std::string& strAccessToken) = 0;
	virtual RESULT RequestTwilioNTSInformation(std::string& strAccessToken) = 0;
	virtual RESULT RequestDreamVersion() = 0;

	virtual RESULT CheckAPIConnection() = 0;
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
		CHECK_API_CONNECTION,
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

// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
//private:
public:
	RESULT Login(std::string& strUsername, std::string& strPassword);

	RESULT Logout();
	RESULT PendLogout();

	RESULT LoginWithOTK(std::string& strOTK, long& environmentID);

	RESULT SwitchTeam();
	RESULT RequestSwitchTeam(std::string strTeamID);

	long GetUserDefaultEnvironmentID();
	RESULT SetUserDefaultEnvironmentID(long environmentId);
	RESULT SetAccessToken(std::string strAccessToken);
	User GetUser();
	TwilioNTSInformation GetTwilioNTSInformation();

	long GetUserID() { return m_user.GetUserID(); }
	long GetUserAvatarModelID() { return m_user.GetAvatarID(); }

// new login flow api calls
public:

	virtual RESULT RequestFormURL(std::string& strFormKey) override;
	RESULT OnFormURL(std::string&& strResponse);

	virtual RESULT RequestAccessToken(std::string& strRefreshToken) override;
	RESULT OnAccessToken(std::string&& strResponse);

	virtual RESULT RequestGetSettings(std::string& strAccessToken) override;
	RESULT OnGetSettings(std::string&& strResponse);

	virtual RESULT RequestSetSettings(std::string& strAccessToken, point ptPosition, quaternion qOrientation) override;
	RESULT OnSetSettings(std::string&& strResponse);

	virtual RESULT RequestTeam(std::string& strAccessToken, std::string strTeamID = "") override;
	RESULT OnGetTeam(std::string&& strResponse);

	virtual RESULT RequestUserProfile(std::string& strAccessToken) override;
	RESULT OnUserProfile(std::string&& strResponse);

	virtual RESULT RequestTwilioNTSInformation(std::string& strAccessToken) override;
	RESULT OnTwilioNTSInformation(std::string&& strResponse);

	virtual RESULT RequestDreamVersion() override;
	RESULT OnDreamVersion(std::string&& strResponse);

	virtual RESULT CheckAPIConnection() override;
	RESULT OnAPIConnectionCheck(std::string&& strResponse);
	RESULT OnAPIConnectionCheckTimeout();

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
		virtual RESULT OnLogin() = 0;
		virtual RESULT OnLogout() = 0;
		virtual RESULT OnPendLogout() = 0;
		virtual RESULT OnSwitchTeams() = 0;

		// api methods
		virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation) = 0;
		virtual RESULT OnSetSettings() = 0;
		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) = 0;
		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) = 0;
		virtual RESULT OnDreamVersion(version dreamVersion) = 0;
		virtual RESULT OnAPIConnectionCheck(bool fIsConnected) = 0;
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
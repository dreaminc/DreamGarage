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

class UserControllerObserver;

class UserControllerProxy : public ControllerProxy {
public:
	virtual std::string GetUserToken() = 0;
	virtual std::string GetPeerScreenName(long peerUserID) = 0;

	virtual RESULT RequestGetSettings(std::wstring wstrHardwareID, std::string strHMDType) = 0;
	virtual RESULT RequestSetSettings(std::wstring wstrHardwareID, std::string strHMDType, float yOffset, float zOffset, float scale) = 0;
	virtual RESULT RequestSettingsForm(std::string key) = 0;
};

// TODO: This is actually a UserController - so change the name of object and file
class UserController : public Controller, public UserControllerProxy {
public:
	enum class UserMethod {
		LOGIN,
		OTK_LOGIN,
		LOAD_PROFILE,
		LOAD_TWILIO_NTS_INFO,
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

	// UserControllerProxy
	UserControllerProxy* GetUserControllerProxy();
	virtual std::string GetUserToken() override;
	virtual std::string GetPeerScreenName(long peerUserID) override;
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) { return R_NOT_IMPLEMENTED; }

private:
	std::string GetMethodURI(UserMethod userMethod);

// User Settings
public:
	RESULT HandleEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudMessage);

	RESULT OnGetSettings(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnSetSettings(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnSettingsForm(std::shared_ptr<CloudMessage> pCloudMessage);

	virtual RESULT RequestGetSettings(std::wstring wstrHardwareID, std::string strHMDType) override;
	virtual RESULT RequestSetSettings(std::wstring wstrHardwareID, std::string strHMDType, float yOffset, float zOffset, float scale) override;
	virtual RESULT RequestSettingsForm(std::string key) override;


// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
//private:
public:
	RESULT Login(std::string& strUsername, std::string& strPassword);
	RESULT LoginWithOTK(std::string& strOTK, long& environmentID);

	long GetUserDefaultEnvironmentID();
	User GetUser();
	TwilioNTSInformation GetTwilioNTSInformation();

	long GetUserID() { return m_user.GetUserID(); }

public:
	class UserControllerObserver {
	public:
		virtual RESULT OnGetSettings() = 0;
		virtual RESULT OnSetSettings() = 0;
		virtual RESULT OnSettings(std::string strURL) = 0;
	};

	RESULT RegisterUserControllerObserver(UserControllerObserver* pUserControllerObserver);

private:
	bool m_fLoggedIn = false;
	std::string	m_strToken;
	std::string m_strPeerScreenName;
	User m_user;
	TwilioNTSInformation m_twilioNTSInformation;

	UserControllerObserver *m_pUserControllerObserver;
};

#endif	// ! USER_CONTROLLER_H_
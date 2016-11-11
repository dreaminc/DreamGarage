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

// TODO: This is actually a UserController - so change the name of object and file
class UserController : public Controller {
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

	RESULT Initialize() {
		return R_NOT_IMPLEMENTED;
	}

	// Read username and password from file and login, get a token
	RESULT LoginFromFilename(const std::wstring& file);
	RESULT LoginFromCommandline();

	// obsolete. to be removed
	//RESULT Login_Json(const std::wstring& strFilename);
	
	// Loads the user profile using the token
	RESULT LoadProfile();
	RESULT LoadTwilioNTSInformation();

private:
	std::string GetMethodURI(UserMethod userMethod);

// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
//private:
public:
	RESULT Login(std::string& strUsername, std::string& strPassword);
	RESULT LoginWithOTK(std::string& strOTK);

	long GetUserDefaultEnvironmentID();
	User GetUser();
	TwilioNTSInformation GetTwilioNTSInformation();

	long GetUserID() { return m_user.GetUserID(); }

private:
	bool m_fLoggedIn = false;
	std::string	m_strToken;

	User m_user;
	TwilioNTSInformation m_twilioNTSInformation;
};

#endif	// ! USER_CONTROLLER_H_
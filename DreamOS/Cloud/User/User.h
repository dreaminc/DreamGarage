#ifndef USER_H_
#define USER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include <string>

typedef struct UserProfile {
	std::string	m_email;
	std::string	m_firstName;
	std::string	m_lastName;
	std::string	m_screenName;
	long m_id;
	long m_defaultEnvId;
};

// TODO: This is actually a UserController - so change the name of object and file
class User {
public:
	User();
	~User();

	// Read username and password from file and login, get a token
	RESULT LoginFromFilename(const std::wstring& file);
	RESULT LoginFromCommandline();

	// obsolete. to be removed
	//RESULT Login_Json(const std::wstring& strFilename);
	
	// Loads the user profile using the token
	RESULT LoadProfile();

// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
//private:
public:
	RESULT Login(std::string& strUsername, std::string& strPassword);

private:
	bool m_fLoggedIn = false;
	std::string	m_strToken;

	UserProfile	m_userProfile;
};

#endif	// ! USER_H_
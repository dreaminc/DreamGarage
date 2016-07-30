#pragma once

#include <string>

typedef struct UserProfile
{
	std::string	m_email;
	std::string	m_firstName;
	std::string	m_lastName;
	std::string	m_screenName;
	long m_id;
	long m_defaultEnvId;
};

class User
{
public:
	// read username and password from file and login, get a token
	bool Login(const std::wstring& file);

	// loads the user profile using the token
	bool LoadProfile();

private:
	bool m_isLoggedIn = false;
	std::string	m_token;

	UserProfile	m_profile;
};

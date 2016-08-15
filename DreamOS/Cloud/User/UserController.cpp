#include "UserController.h"

#include "Cloud/HTTP/HTTPController.h"
#include "json.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

#include <future>

UserController::UserController() :
	m_fLoggedIn(false)
{
	// empty
}


UserController::~UserController() {
	// 
}

RESULT UserController::Login(std::string& strUsername, std::string& strPassword) {
	RESULT r = R_PASS;

	std::string strHTTPRequest{ "username=" + strUsername + "&password=" + strPassword };
	HTTPResponse httpResponse;
	nlohmann::json jsonResponse;
	std::string strResponse;

	HTTPController *pHTTPController = HTTPController::instance();

	// TODO: Not hard coded!
	CRM(pHTTPController->POST("http://localhost:8000/token/", HTTPController::ContentHttp(), strHTTPRequest, httpResponse), "User login failed to post request");
	
	strResponse = std::string(httpResponse.PullResponse());
	strResponse = strResponse.substr(0, strResponse.find('\r'));
	jsonResponse = nlohmann::json::parse(strResponse);

	DEBUG_LINEOUT(jsonResponse.dump().c_str());

	CBM(("/token"_json_pointer.to_string() != ""), "Token missing from JSON");
	//CBM((jsonResponse["/token"_json_pointer].is_null()), "Token is missing from JSON");

	m_strToken = jsonResponse["/token"_json_pointer].get<std::string>();

	DEBUG_LINEOUT("User Login got token: %s", m_strToken.c_str());
	m_fLoggedIn = true;

Error:
	return r;
}

RESULT UserController::LoginFromCommandline() {
	RESULT r = R_PASS;

	// TODO:

Error:
	return r;
}

// TODO: Split this up into login / get user/pass from file function
RESULT UserController::LoginFromFilename(const std::wstring& file) {
	RESULT r = R_PASS;

	std::ifstream userLoginFile(file, std::ios::binary);

	std::string line;
	std::map<std::string, std::string> keyValue;

	CB((userLoginFile.is_open()));
	
	while (std::getline(userLoginFile, line)) {
		line.erase(std::remove_if(line.begin(), line.end(), [](char c) {return (c == '\r' || c == '\n'); }), line.end());
		size_t lineCount = line.find(':');
		CBM((lineCount != line.length()), "Cannot read line %d", line.c_str());
		keyValue[line.substr(0, lineCount)] = line.substr(lineCount + 1);
	}

	userLoginFile.close();

	CR(Login(keyValue["email"], keyValue["password"]));

Error:
	return r;
}

/*
RESULT User::Login_Json(const std::wstring& file) {
	RESULT r = R_PASS;

	std::ifstream userLoginFile(file, std::ios::binary);
	std::string line;
	std::map<std::string, std::string> keyValue;

	CB((userLoginFile.is_open()));
	{
		while (std::getline(userLoginFile, line)) {
			line.erase(std::remove_if(line.begin(), line.end(), [](char c) {return (c == '\r' || c == '\n'); }), line.end());
			size_t lineCount = line.find(':');
			CBM((lineCount != line.length()), "Cannot read line %d", line.c_str());
			keyValue[line.substr(0, lineCount)] = line.substr(lineCount + 1);
		}

		userLoginFile.close();

		nlohmann::json jsonRequest;
		jsonRequest["email"] = keyValue["email"];
		jsonRequest["password"] = keyValue["password"];

		DEBUG_LINEOUT(jsonRequest.get<std::string>().c_str());

		HTTPResponse httpResponse;

		CBM((HTTPController::GetHttp()->POST("http://10.0.75.1:8000/session/", HTTPController::ContentAcceptJson(), jsonRequest.dump(), httpResponse)),
			"User Login JSON failed to post request");

		std::string strHTTPResponse(httpResponse.PullResponse());
		strHTTPResponse = strHTTPResponse.substr(0, strHTTPResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHTTPResponse);

		CBM((jsonResponse["/data/token"_json_pointer].is_null()), "Token is missing from JSON");

		m_strToken = jsonResponse["/data/token"_json_pointer].get<std::string>();

		DEBUG_LINEOUT("User Login got token: %s", m_strToken.c_str());
		m_fLoggedIn = true;
	}

Error:
	return r;
}
*/

RESULT UserController::LoadProfile() {
	RESULT r = R_PASS;

	std::cout << "load profile..." << std::endl;

	CBM(m_fLoggedIn, "user not logged in yet");

	{
		HTTPResponse httpResponse;
		std::string strAuthorizationToken = "Authorization: Token " + m_strToken;
		HTTPController *pHTTPController = HTTPController::instance();

		auto headers = HTTPController::ContentAcceptJson();
		headers.push_back(strAuthorizationToken);

		CB((pHTTPController->GET("http://localhost:8000/user/", headers, httpResponse)), "User LoadProfile failed to post request");

		DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());

		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);

		m_user = User(
			jsonResponse["/data/id"_json_pointer].get<long>(),
			jsonResponse["/data/default_environment"_json_pointer].get<long>(),
			jsonResponse["/data/email"_json_pointer].get<std::string>(),
			jsonResponse["/data/public_name"_json_pointer].get<std::string>(),
			jsonResponse["/data/first_name"_json_pointer].get<std::string>(),
			jsonResponse["/data/last_name"_json_pointer].get<std::string>(),
			version(1.0f)	// version
		);

		m_user.SetToken(m_strToken);

		DEBUG_LINEOUT("User Profile Loaded");
		m_user.PrintUser();
	}

Error:
	return r;
}

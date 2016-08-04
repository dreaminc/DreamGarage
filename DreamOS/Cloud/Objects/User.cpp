#include "User.h"

#include "Cloud/Protocols/Http.h"
#include "json.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

#include <future>

bool User::Login(const std::wstring& file)
{
	std::ifstream userLoginFile(file, std::ios::binary);

	if (!userLoginFile.is_open())
	{
		return false;
	}

	std::string line;
	std::map<std::string, std::string> keyValue;

	while (std::getline(userLoginFile, line)) {

		line.erase(std::remove_if(line.begin(), line.end(), [](char c) {return (c == '\r' || c == '\n'); }), line.end());

		auto i = line.find(':');
		if (i == line.length())
		{
			std::cout << "Cannot read line " << line;
			return false;
		}

		keyValue[line.substr(0, i)] = line.substr(i + 1);
	}

	userLoginFile.close();
	
	std::string httpRequest{ "username=" + keyValue["email"] + "&password=" + keyValue["password"] };
	
	HttpResponse resp;

	bool res = Http::GetHttp()->POST("http://localhost:8000/token/",
		Http::ContentHttp(),
		httpRequest,
		resp);

	if (!res)
	{
		std::cout << "failed to post request" << std::endl;
		return false;
	}

	std::string s(resp.PullResponse());

	s = s.substr(0, s.find('\r'));

	nlohmann::json jsonResponse = nlohmann::json::parse(s);

	std::cout << jsonResponse << std::endl;

	if ("/token"_json_pointer.to_string() == "" || jsonResponse["/token"_json_pointer].is_null())
	{
		// no token in the json object
		std::cout << "token missing from json" << std::endl;
		return false;
	}

	m_token = jsonResponse["/token"_json_pointer].get<std::string>();

	std::cout << "got a token " << m_token << std::endl;

	m_isLoggedIn = true;

	return true;
}

bool User::Login_Json(const std::wstring& file)
{
	std::ifstream userLoginFile(file, std::ios::binary);

	if (!userLoginFile.is_open())
	{
		return false;
	}

	std::string line;
	std::map<std::string, std::string> keyValue;

	while (std::getline(userLoginFile, line)) {
		
		line.erase(std::remove_if(line.begin(), line.end(), [](char c) {return (c == '\r' || c == '\n'); }), line.end());

		auto i = line.find(':');
		if (i == line.length())
		{
			std::cout << "Cannot read line " << line;
			return false;
		}

		keyValue[line.substr(0, i)] = line.substr(i + 1);
	}

	userLoginFile.close();

	nlohmann::json jsonRequest;
	jsonRequest["email"] = keyValue["email"];
	jsonRequest["password"] = keyValue["password"];

	std::cout << jsonRequest << std::endl;

	HttpResponse resp;

	bool res = Http::GetHttp()->POST("http://10.0.75.1:8000/session/",
		Http::ContentAcceptJson(),
		jsonRequest.dump(),
		resp);

	if (!res)
	{
		std::cout << "failed to post request" << std::endl;
		return false;
	}
	
	std::string s(resp.PullResponse());

	s = s.substr(0, s.find('\r'));

	nlohmann::json jsonResponse = nlohmann::json::parse(s);

	if (jsonResponse["/data/token"_json_pointer].is_null())
	{
		// no token in the json object
		std::cout << "token missing from json" << std::endl;
		return false;
	}

	m_token = jsonResponse["/data/token"_json_pointer].get<std::string>();

	std::cout << "got a token " << m_token << std::endl;
	
	m_isLoggedIn = true;

	return true;
}

bool User::LoadProfile()
{
	std::cout << "load profile..." << std::endl;

	if (!m_isLoggedIn)
	{
		std::cout << "user not logged in yet" << std::endl;
		return false;
	}

	HttpResponse resp;

	std::string auth = "Authorization: Token " + m_token;
	
	auto headers = Http::ContentAcceptJson();
	headers.push_back(auth);

	bool res = Http::GetHttp()->GET("http://localhost:8000/user/",
		headers,
		resp);

	if (!res)
	{
		std::cout << "failed to post request" << std::endl;
		return false;
	}

	std::cout << "get returned " << resp.PullResponse() << std::endl;

	std::string s(resp.PullResponse());

	s = s.substr(0, s.find('\r'));

	nlohmann::json jsonResponse = nlohmann::json::parse(s);

	m_profile = {
		jsonResponse["/data/email"_json_pointer].get<std::string>(),
		jsonResponse["/data/first_name"_json_pointer].get<std::string>(),
		jsonResponse["/data/last_name"_json_pointer].get<std::string>(),
		jsonResponse["/data/public_name"_json_pointer].get<std::string>(),
		jsonResponse["/data/id"_json_pointer].get<long>(),
		jsonResponse["/data/default_environment"_json_pointer].get<long>(),
	};

	return true;
}

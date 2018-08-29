#include "UserController.h"
#include "DreamLogger/DreamLogger.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Sandbox/CommandLineManager.h"
#include "json.hpp"
#include "Primitives/version.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

#include <future>

#include "Cloud/CloudController.h"
#include "Cloud/CloudMessage.h"
#include "Core/Utilities.h"

UserController::UserController(Controller* pParentController) :
	Controller(pParentController),
	m_fLoggedIn(false)
{
	// empty
}


UserController::~UserController() {
	// 
}

RESULT UserController::Initialize() {
	RESULT r = R_PASS;

	// Register Methods
	//user
	CR(RegisterMethod("get_settings", std::bind(&UserController::OnGetSettings, this, std::placeholders::_1)));
	CR(RegisterMethod("set_settings", std::bind(&UserController::OnSetSettings, this, std::placeholders::_1)));

Error:
	return r;
}

// TODO: Move to Controller - register methods etc
std::string UserController::GetMethodURI(UserMethod userMethod) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strURI = "";

#ifdef USE_LOCALHOST
	std::string strAPIURL = "http://localhost:8001";
#else
	std::string strAPIURL = pCommandLineManager->GetParameterValue("api.ip");
#endif


	switch (userMethod) {
		case UserMethod::GET_DREAM_VERSION: {
			strURI = strAPIURL + "/client/settings";
		} break;

		case UserMethod::LOGIN: {
			strURI = strAPIURL + "/token/";
		} break;

		case UserMethod::OTK_LOGIN: {
			strURI = strAPIURL + "/one-time-key/";
			//strURI = strWWWURL + "/one-time-key/";
		} break;

		case UserMethod::LOAD_PROFILE: {	
			strURI = strAPIURL + "/user/";
		} break;

		case UserMethod::LOAD_TWILIO_NTS_INFO: {
			strURI = strAPIURL + "/webrtc/token/";
		} break;

		case UserMethod::GET_FORM: {
			strURI = strAPIURL + "/forms/";
		} break;

		case UserMethod::GET_ACCESS_TOKEN: {
			strURI = strAPIURL + "/access_token/";
		} break;
		
		//TODO: get and set settings need to be confirmed with doug
		case UserMethod::SETTINGS: {
			strURI = strAPIURL + "/users/settings";
		} break;

		case UserMethod::TEAMS: {
			strURI = strAPIURL + "/teams/";
		}
	}

	return strURI;
}

// //https://api.develop.dreamos.com/one-time-key/404d642e34e74d68a401f126f5b327c2
RESULT UserController::LoginWithOTK(std::string& strOTK, long& environmentID) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Login using OTK");
	
	HTTPResponse httpResponse;
	std::string strURI = GetMethodURI(UserMethod::OTK_LOGIN) + strOTK;
	HTTPController *pHTTPController = HTTPController::instance();

	auto headers = HTTPController::ContentAcceptJson();

	CBM((pHTTPController->GET(strURI, headers, httpResponse)), "User LoadTwilioNTSInformation failed to post request");
	DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());
	
	{
		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);

		// Check for error/failure
		if (jsonResponse["/meta/status_code"_json_pointer].get<int>() != 200) {
			std::string strErrorDetail = jsonResponse["/meta/status_code"_json_pointer].get<std::string>();
			DEBUG_LINEOUT("OTK failed with error: %s", strErrorDetail.c_str());
		}

		// Get the token
		//CBM((jsonResponse["/data/token"_json_pointer].is_null()), "Token is missing from JSON");
		m_strToken = jsonResponse["/data/token"_json_pointer].get<std::string>();
		environmentID = jsonResponse["/data/environment"_json_pointer].get<long>();

		DEBUG_LINEOUT("User Login got token: %s", m_strToken.c_str());
		SetIsLoggedIn(true);

		DOSLOG(INFO, "[UserController] User logged in with OTK");
	}

Error:
	return r;
}

RESULT UserController::Login(std::string& strUsername, std::string& strPassword) {
	RESULT r = R_PASS;

	std::string strHTTPRequest{ "username=" + strUsername + "&password=" + strPassword };
	HTTPResponse httpResponse;
	nlohmann::json jsonResponse;
	std::string strResponse;
	std::string strURI = GetMethodURI(UserMethod::LOGIN);

	HTTPController *pHTTPController = HTTPController::instance();
	
	// TODO: Not hard coded!
	CRM(pHTTPController->POST(strURI, HTTPController::ContentHttp(), strHTTPRequest, httpResponse), "User login failed to post request");
	
	strResponse = std::string(httpResponse.PullResponse());
	strResponse = strResponse.substr(0, strResponse.find('\r'));
	jsonResponse = nlohmann::json::parse(strResponse);

	DEBUG_LINEOUT(jsonResponse.dump().c_str());

	//CBM((jsonResponse["/token"_json_pointer].is_null()), "Token is missing from JSON");

	m_strToken = jsonResponse["/token"_json_pointer].get<std::string>();

	DEBUG_LINEOUT("User Login got token: %s", m_strToken.c_str());
	SetIsLoggedIn(true);

	DOSLOG(INFO, "[UserController] User %v logged in", strUsername);

Error:
	return r;
}

RESULT UserController::Logout() {
	RESULT r = R_PASS;
	
	CBRM(IsLoggedIn(), R_SKIPPED, "User not logged in");

	auto pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNRM(pEnvironmentController, R_SKIPPED, "Environment controller does not exist.");
	CBRM(pEnvironmentController->IsEnvironmentSocketConnected(), R_SKIPPED, "Environment socket is not connected.");
	CR(pEnvironmentController->DisconnectFromEnvironmentSocket());
	CR(SetIsLoggedIn(false));

Error:
	return r;
}

long UserController::GetUserDefaultEnvironmentID() {
	return m_defaultEnvironmentId;
//	return m_user.GetDefaultEnvironmentID();
}

RESULT UserController::SetUserDefaultEnvironmentID(long environmentID) {
	RESULT r = R_PASS;

	// user may not be initialized
//	return m_user.SetDefaultEnvironmentID(environmentID);
	
	m_loginState.fHasEnvironmentId = true;
	
	m_defaultEnvironmentId = environmentID;

	CR(UpdateLoginState());

Error:
	return r;
}

RESULT UserController::SetAccessToken(std::string strAccessToken) {
	RESULT r = R_PASS;

	m_loginState.fHasAccessToken = true;
	m_strAccessToken = strAccessToken;
	CR(UpdateLoginState());

Error:
	return r;
}

User UserController::GetUser() {
	return m_user;
}

TwilioNTSInformation UserController::GetTwilioNTSInformation() {
	return m_twilioNTSInformation;
}

RESULT UserController::RequestDreamVersion() {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;
	nlohmann::json jsonResponse;
	std::string strResponse;
	std::string strVersion;
	std::string strURI = GetMethodURI(UserMethod::GET_DREAM_VERSION);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();

	CB(pHTTPController->AGET(strURI, headers, std::bind(&UserController::OnDreamVersion, this, std::placeholders::_1)));	

Error:
	return r;
}

void UserController::OnDreamVersion(std::string&& strResponse) {
	RESULT r = R_PASS;

	std::string strDreamVersion;
	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	nlohmann::json jsonForm;
	int statusCode;

	//TODO: these function are void instead of RESULT
	CR(GetResponseData(jsonData, jsonResponse, statusCode));
	CB(statusCode == 200);

	CBM(!jsonData["/client_settings/minimum_version"_json_pointer].is_null(), "minimum version was null");

	if (jsonData["/client_settings/minimum_version"_json_pointer].is_string()) {
		strDreamVersion = jsonData["/client_settings/minimum_version"_json_pointer].get<std::string>();
	}

	if (m_pUserControllerObserver != nullptr) {
		version dreamVersion = version(strDreamVersion);
		CR(m_pUserControllerObserver->OnDreamVersion(dreamVersion));
	}

Error:
	return;
}

RESULT UserController::LoginFromCommandline() {
	RESULT r = R_PASS;

	// TODO:

//Error:
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
		CBM((lineCount != line.length()), "Cannot read line %s", line.c_str());
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
		std::string strURI = GetMethodURI(UserMethod::LOAD_PROFILE);
		HTTPController *pHTTPController = HTTPController::instance();

		auto headers = HTTPController::ContentAcceptJson();
		headers.push_back(strAuthorizationToken);

		CBM((pHTTPController->GET(strURI, headers, httpResponse)), "User LoadProfile failed to post request");

		DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());

		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);

		m_user = User(
			jsonResponse["/data/id"_json_pointer].get<long>(),
			//jsonResponse["/data/default_environment"_json_pointer].get<long>(),
			m_defaultEnvironmentId,
			jsonResponse["/data/avatar_model_id"_json_pointer].get<int>(),
			jsonResponse["/data/email"_json_pointer].get<std::string>(),
			jsonResponse["/data/public_name"_json_pointer].get<std::string>(),
			jsonResponse["/data/first_name"_json_pointer].get<std::string>(),
			jsonResponse["/data/last_name"_json_pointer].get<std::string>(),
			version(1.0f)	// version
		);

		m_user.SetToken(m_strToken);

		DEBUG_LINEOUT("User Profile Loaded");
		m_user.PrintUser();

		//HUD_OUT((std::string("User ") + m_user.GetEmail() + " is connected.").c_str());
		//OVERLAY_DEBUG_SET("User", std::string("User (") + std::to_string(m_user.GetUserID()) + ") " + m_user.GetEmail());
		//OVERLAY_DEBUG_SET("Env", "Env " + std::to_string(m_user.GetDefaultEnvironmentID()));

		DOSLOG(INFO, "User %v is connected", m_user.GetUserID());
	}

Error:
	return r;
}

RESULT UserController::GetPeerProfile(long peerUserID) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Loading peer profile");
	{
		HTTPResponse httpResponse;

		std::string strAuthorizationToken = "Authorization: Bearer " + GetSavedAccessToken();

		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		std::string strAPIURL = pCommandLineManager->GetParameterValue("api.ip");
		std::string strPeerID = std::to_string(peerUserID);
		std::string	strURI = strAPIURL + "/user/" + strPeerID + "/";

		HTTPController *pHTTPController = HTTPController::instance();

		auto headers = HTTPController::ContentAcceptJson();
		headers.push_back(strAuthorizationToken);

		CBM((pHTTPController->GET(strURI, headers, httpResponse)), "User LoadProfile failed to post request");
		
		DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());
		
		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);

		if (peerUserID == jsonResponse["/data/id"_json_pointer].get<long>()) {
			m_strPeerScreenName = jsonResponse["/data/public_name_short"_json_pointer].get<std::string>();
			m_avatarModelId = jsonResponse["/data/avatar_model_id"_json_pointer].get<int>();
		}
		
		DEBUG_LINEOUT("User Profile Loaded");
	}

Error:
	return r;
}

RESULT UserController::GetFormURL(std::string& strFormKey) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::GET_FORM) + strFormKey;

	DOSLOG(INFO, "Requesting form: %s", strURI);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();

	CB(pHTTPController->AGET(strURI, headers, std::bind(&UserController::OnFormURL, this, std::placeholders::_1)));

	/*
	DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());
	{
		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);
	}
	//*/

Error:
	return r;
}

void UserController::OnFormURL(std::string&& strResponse) {
	RESULT r = R_PASS;

	//std::string strHttpResponse = strHttpResponse.substr(0, strResponse.find('\r'));
	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	nlohmann::json jsonForm;
	int statusCode;

	//TODO: these function are void instead of RESULT
	CR(GetResponseData(jsonData, jsonResponse, statusCode));
	CB(statusCode == 200);

	CBM(!jsonData["/form"_json_pointer].is_null(), "form object is malformed");

	jsonForm = jsonData["/form"_json_pointer];

	CBM(jsonForm["/key"_json_pointer].is_string(), "form key is malformed");
	CBM(jsonForm["/title"_json_pointer].is_string(), "form title is malformed");
	CBM(jsonForm["/url"_json_pointer].is_string(), "form url is malformed");

	CNM(m_pUserControllerObserver, "user observer is nullptr");
	CR(m_pUserControllerObserver->OnFormURL(jsonForm["/key"_json_pointer].get<std::string>(),
		jsonForm["/title"_json_pointer].get<std::string>(),
		jsonForm["/url"_json_pointer].get<std::string>()));

Error:
	return;
}

RESULT UserController::GetAccessToken(std::string& strRefreshToken) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::GET_ACCESS_TOKEN);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();

	nlohmann::json jsonData = nlohmann::json::object();
	jsonData["refresh_token"] = nlohmann::json::object();
	jsonData["refresh_token"]["token"] = strRefreshToken;

	CB(pHTTPController->APOST(strURI, headers, jsonData.dump(-1), std::bind(&UserController::OnAccessToken, this, std::placeholders::_1)));

Error:
	return r;
}

void UserController::OnAccessToken(std::string&& strResponse) {
	RESULT r = R_PASS;

	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	nlohmann::json jsonAccessTokenObject;
	std::string strAccessToken;
	int statusCode;

	CR(GetResponseData(jsonData, jsonResponse, statusCode));

	// if the json data is null we know that we got an expected error in GetResponseData
	// right now, that is only a 401 bad access
	if (statusCode == 401) {
		CR(m_pUserControllerObserver->OnAccessToken(false, strAccessToken));
	}
	else if (statusCode == 200) {
		CBM(!jsonData["/access_token"_json_pointer].is_null(), "access token object is malformed");
		jsonAccessTokenObject = jsonData["/access_token"_json_pointer];
		CBM(!jsonAccessTokenObject["/token"_json_pointer].is_null(), "access token is malformed");
			
		strAccessToken = jsonAccessTokenObject["/token"_json_pointer].get<std::string>();

		CR(m_pUserControllerObserver->OnAccessToken(true, strAccessToken));

		m_strAccessToken = strAccessToken;
		m_loginState.fHasAccessToken = true;
		UpdateLoginState();
	}

Error:
	return;
}

RESULT UserController::GetSettings(std::string& strAccessToken) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::SETTINGS);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();
	headers.emplace_back(HTTPController::AuthorizationHeader(strAccessToken));

	CB(pHTTPController->AGET(strURI, headers, std::bind(&UserController::OnGetApiSettings, this, std::placeholders::_1)));

Error:
	return r;
}

void UserController::OnGetApiSettings(std::string&& strResponse) {
	RESULT r = R_PASS;

	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	int statusCode;

	float height;
	float depth;
	float scale;

	CR(GetResponseData(jsonData, jsonResponse, statusCode));

	if (statusCode == 404) {
		//TODO: it isn't right to have this code here
		std::string strSettingsFormKey = "FormKey.UsersSettings";
		GetFormURL(strSettingsFormKey);
	}
	//TODO: combine with the json rpc response
	else if (statusCode == 200) {
		nlohmann::json jsonSettings = jsonData["/user_settings"_json_pointer];
		height = jsonSettings["ui_offset_y"].get<float>();
		depth = jsonSettings["ui_offset_z"].get<float>();
		scale = jsonSettings["ui_scale"].get<float>();
	}

	CR(m_pUserControllerObserver->OnGetSettings(height, depth, scale));

Error:
	return;
}

RESULT UserController::SetSettings(std::string& strAccessToken, float height, float depth, float scale) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::SETTINGS);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();
	headers.emplace_back(HTTPController::AuthorizationHeader(strAccessToken));

	nlohmann::json jsonSettings = nlohmann::json::object();
	jsonSettings["user_settings"] = nlohmann::json::object();
	jsonSettings["user_settings"]["ui_offset_y"] = height;
	jsonSettings["user_settings"]["ui_offset_z"] = depth;
	jsonSettings["user_settings"]["ui_scale"] = scale;

	CB(pHTTPController->APOST(strURI, headers, jsonSettings.dump(-1), std::bind(&UserController::OnSetApiSettings, this, std::placeholders::_1)));

Error:
	return r;
}

void UserController::OnSetApiSettings(std::string&& strResponse) {
	RESULT r = R_PASS;

	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	int statusCode;

	CR(GetResponseData(jsonData, jsonResponse, statusCode));
	CB(statusCode == 200);

	CR(m_pUserControllerObserver->OnSetSettings());

Error:
	return;
}

RESULT UserController::GetTeam(std::string& strAccessToken) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::TEAMS);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();
	headers.emplace_back(HTTPController::AuthorizationHeader(strAccessToken));

	CB(pHTTPController->AGET(strURI, headers, std::bind(&UserController::OnGetTeam, this, std::placeholders::_1)));

Error:
	return r;
}

void UserController::OnGetTeam(std::string&& strResponse) {
	RESULT r = R_PASS;

	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	int statusCode;

	CR(GetResponseData(jsonData, jsonResponse, statusCode));
	CB(statusCode == 200 || statusCode == 404);

	if (statusCode == 404) {
		m_pUserControllerObserver->OnGetTeam(false, -1, -1);
	}
	else {
		nlohmann::json jsonTeam = jsonData["/team"_json_pointer];
		//Currently, only the default environment is needed here	
		int environmentId = jsonTeam["/default_environment/id"_json_pointer].get<int>();
		int environmentModelId = jsonTeam["/default_environment/model_id"_json_pointer].get<int>();

#ifdef _DEBUG
		// Allow force of environment ID in DEBUG
		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		std::string strEnvironmentID = pCommandLineManager->GetParameterValue("environment");
		if ((strEnvironmentID.compare("default") == 0) == false) {
			environmentId = stoi(strEnvironmentID);
		}
#endif

		SetUserDefaultEnvironmentID(environmentId);

		if (m_pUserControllerObserver != nullptr) {
			m_pUserControllerObserver->OnGetTeam(true, environmentId, environmentModelId);
		}
	}
	
Error:
	return;
}

RESULT UserController::RequestUserProfile(std::string& strAccessToken) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::LOAD_PROFILE);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();
	headers.emplace_back(HTTPController::AuthorizationHeader(strAccessToken));

	CB(pHTTPController->AGET(strURI, headers, std::bind(&UserController::OnUserProfile, this, std::placeholders::_1)));

Error:
	return r;
}

void UserController::OnUserProfile(std::string&& strResponse) {
	RESULT r = R_PASS;

	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	int statusCode;

	//CR(GetResponseData(jsonData, jsonResponse, statusCode));
	GetResponseData(jsonData, jsonResponse, statusCode);
	//CB(statusCode == 200);

	m_user = User(
		jsonData["/id"_json_pointer].get<long>(),
		m_defaultEnvironmentId,
		jsonData["/avatar_model_id"_json_pointer].get<int>(),
		jsonData["/email"_json_pointer].get<std::string>(),
		jsonData["/public_name"_json_pointer].get<std::string>(),
		jsonData["/first_name"_json_pointer].get<std::string>(),
		jsonData["/last_name"_json_pointer].get<std::string>(),
		version(1.0f)	// version
	);

	m_loginState.fHasUserProfile = true;
	CR(UpdateLoginState());

Error:
	return;
}

RESULT UserController::RequestTwilioNTSInformation(std::string& strAccessToken) {
	RESULT r = R_PASS;

	HTTPResponse httpResponse;

	std::string strURI = GetMethodURI(UserMethod::LOAD_TWILIO_NTS_INFO);

	HTTPController *pHTTPController = HTTPController::instance();
	auto headers = HTTPController::ContentAcceptJson();
	headers.emplace_back(HTTPController::AuthorizationHeader(strAccessToken));

	CB(pHTTPController->AGET(strURI, headers, std::bind(&UserController::OnTwilioNTSInformation, this, std::placeholders::_1)));

Error:
	return r;
}

void UserController::OnTwilioNTSInformation(std::string&& strResponse) {
	RESULT r = R_PASS;

	nlohmann::json jsonResponse = nlohmann::json::parse(strResponse);
	nlohmann::json jsonData;
	int statusCode;

	CR(GetResponseData(jsonData, jsonResponse, statusCode));
	CB(statusCode == 200);

	m_twilioNTSInformation = TwilioNTSInformation(
		jsonData["/date_created"_json_pointer].get<std::string>(),
		jsonData["/date_updated"_json_pointer].get<std::string>(),
		jsonData["/ttl"_json_pointer].get<int>(),
		jsonData["/username"_json_pointer].get<std::string>(),
		jsonData["/password"_json_pointer].get<std::string>()
	);

	// Ice Server URIs
	for (auto &jsonICEServer : jsonResponse["/data/ice_servers"_json_pointer]) {
		std::string strSDPCandidate = jsonICEServer["url"].get<std::string>();
		m_twilioNTSInformation.AddICEServerURI(strSDPCandidate);
	}

	DEBUG_LINEOUT("Twilio NTS Information Loaded");
	m_twilioNTSInformation.Print();

	m_loginState.fHasTwilioInformation = true;
	UpdateLoginState();

Error:
	return;
}

RESULT UserController::GetResponseData(nlohmann::json& jsonData, nlohmann::json jsonResponse, int& statusCode) {
	RESULT r = R_PASS;

	//{"errors":[],"meta":{"status_code":200},"data":{
	//"form":{"key":"FormKey.UsersSettings","title":"Settings","url":"https://www.develop.dreamos.com/forms/users/settings"}}}
	nlohmann::json jsonMeta;
	
	CBM(!jsonResponse.is_null(), "HTTP json response is null");

	CBM(!jsonResponse["/meta"_json_pointer].is_null(), "HTTP json response is malformed");
	jsonMeta = jsonResponse["/meta"_json_pointer];

	bool fNumber = jsonMeta["/status_code"_json_pointer].is_number_unsigned();

	CBM(jsonMeta["/status_code"_json_pointer].is_number_unsigned(), "HTTP json status code is malformed");
	statusCode = jsonMeta["/status_code"_json_pointer].get<int>();

	//TODO: more advanced handling of different codes
	//TODO: extract message out of errors list
	//CBM(statusCode == 200 || statusCode == 401, "website returned unhandled status code: %d", statusCode);
	
	jsonData = jsonResponse["/data"_json_pointer];

	//	jsonResponse["/errors"_json_pointer][0];

Error:
	return r;
}

RESULT UserController::UpdateLoginState() {
	RESULT r = R_PASS;

	if (m_loginState.fHasAccessToken &&
		m_loginState.fHasUserProfile &&
		m_loginState.fHasEnvironmentId &&
		m_loginState.fHasTwilioInformation) {

		if (!m_loginState.fPendingConnect) {
			m_loginState.fPendingConnect = true;

			m_user.SetDefaultEnvironmentID(m_defaultEnvironmentId);
			m_user.SetToken(m_strAccessToken);

			if (!IsLoggedIn()) {

				auto pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CN(pEnvironmentController);
				CR(pEnvironmentController->ConnectToEnvironmentSocket(m_user, m_defaultEnvironmentId));
				SetIsLoggedIn(true);
			}
		}
	}

Error:
	m_loginState.fPendingConnect = false;
	return r;
}

std::string UserController::GetSavedAccessToken() {
	return m_strAccessToken;
}

bool UserController::IsLoggedIn() {
	return m_fLoggedIn;
}

RESULT UserController::SetIsLoggedIn(bool fLoggedIn) {
	RESULT r = R_PASS;

	m_fLoggedIn = fLoggedIn;

	if (m_fLoggedIn) {
		CR(m_pUserControllerObserver->OnLogin());
	}
	else {
		CR(m_pUserControllerObserver->OnLogout());

		// Reset 
		m_loginState.fHasAccessToken = false;
		m_loginState.fHasUserProfile = false;
		m_loginState.fHasEnvironmentId = false;
		m_loginState.fHasTwilioInformation = false;
	}

Error:
	return r;
}

UserControllerProxy* UserController::GetUserControllerProxy() {
	return (UserControllerProxy*)(this);
}

std::string UserController::GetUserToken() {
	return m_strToken;
}

std::string UserController::GetPeerScreenName(long peerUserID) {
	//GetPeerProfile(peerUserID);
	return m_strPeerScreenName;
}

int UserController::GetPeerAvatarModelID(long peerUserID) {
	return m_avatarModelId;
}

CLOUD_CONTROLLER_TYPE UserController::GetControllerType() {
	return CLOUD_CONTROLLER_TYPE::USER;
}

// TODO: This may want to move to an API controller object instead
RESULT UserController::LoadTwilioNTSInformation() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Load Twilio NTS Information...");
	CBM(m_fLoggedIn, "User not logged in yet");

	{
		HTTPResponse httpResponse;
		std::string strAuthorizationToken = "Authorization: Bearer " + m_strToken;
		std::string strURI = GetMethodURI(UserMethod::LOAD_TWILIO_NTS_INFO);
		HTTPController *pHTTPController = HTTPController::instance();

		auto headers = HTTPController::ContentAcceptJson();
		headers.push_back(strAuthorizationToken);

		CBM((pHTTPController->GET(strURI, headers, httpResponse)), "User LoadTwilioNTSInformation failed to post request");

		DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());

		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);

		m_twilioNTSInformation = TwilioNTSInformation(
			jsonResponse["/data/date_created"_json_pointer].get<std::string>(),
			jsonResponse["/data/date_updated"_json_pointer].get<std::string>(),
			jsonResponse["/data/ttl"_json_pointer].get<int>(),
			jsonResponse["/data/username"_json_pointer].get<std::string>(),
			jsonResponse["/data/password"_json_pointer].get<std::string>()
		);

		// Ice Server URIs
		for (auto &jsonICEServer : jsonResponse["/data/ice_servers"_json_pointer]) {
			std::string strSDPCandidate = jsonICEServer["url"].get<std::string>();
			m_twilioNTSInformation.AddICEServerURI(strSDPCandidate);
		}

		DEBUG_LINEOUT("Twilio NTS Information Loaded");
		m_twilioNTSInformation.Print();
	}

Error:
	return r;
}

RESULT UserController::HandleEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	CR(HandleOnMethodCallback(pCloudMessage));
Error:
	return r;
}

RESULT UserController::OnGetSettings(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonUserSettings = jsonPayload["/user_settings"_json_pointer];

	if (!jsonUserSettings.is_null()) {
		if (m_pUserControllerObserver != nullptr) {
			// Moving to Send/Receive paradigm
			float height = 0.0f;
			float depth = 0.0f;
			float scale = 1.0f;

			if (jsonUserSettings["/ui_offset_y"_json_pointer].is_number_float()) {
				height = jsonUserSettings["/ui_offset_y"_json_pointer].get<float>();
			}
			if (jsonUserSettings["/ui_offset_z"_json_pointer].is_number_float()) {
				depth = jsonUserSettings["/ui_offset_z"_json_pointer].get<float>();
			}
			if (jsonUserSettings["/ui_scale"_json_pointer].is_number_float()) {
				scale = jsonUserSettings["/ui_scale"_json_pointer].get<float>();
			}

			CR(m_pUserControllerObserver->OnGetSettings(height, depth, scale));
		}
	}
	else {
		//TODO: is this the right place to request the settings form?
		auto pCloudController = GetCloudController();
		CN(pCloudController);
		auto pEnvironmentControllerProxy = dynamic_cast<EnvironmentControllerProxy*>(pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CN(pEnvironmentControllerProxy);
		pEnvironmentControllerProxy->RequestForm("FormKey.UsersSettings");
	}

Error:
	return r;
}

RESULT UserController::OnSetSettings(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();

	if (jsonPayload.size() != 0) {
		if (m_pUserControllerObserver != nullptr) {
			// Moving to Send/Receive paradigm
			CR(m_pUserControllerObserver->OnSetSettings());
		}
	}

Error:
	return r;
}

RESULT UserController::RequestGetSettings(std::wstring wstrHardwareID, std::string strHMDType) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	CloudController *pParentCloudController = GetCloudController();
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["user_settings"] = nlohmann::json::object();
	jsonPayload["user_settings"]["user"] = (int)(m_user.GetUserID());
//	jsonPayload["user_settings"]["instance_id"] = util::WideStringToString(wstrHardwareID);
//	jsonPayload["user_settings"]["hmd_type"] = strHMDType;

	pCloudRequest = CloudMessage::CreateRequest(pParentCloudController, jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("user.get_settings"));

	auto pEnvironmentController = dynamic_cast<EnvironmentController*>(pParentCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentController);
	CR(pEnvironmentController->SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::USER_GET_SETTINGS));

Error:
	return r;
}

RESULT UserController::RequestSetSettings(std::wstring wstrHardwareID, std::string strHMDType, float yOffset, float zOffset, float scale) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	CloudController *pParentCloudController = GetCloudController();
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["user_settings"] = nlohmann::json::object();
	jsonPayload["user_settings"]["user"] = (int)(m_user.GetUserID());
//	jsonPayload["user_settings"]["instance_id"] = util::WideStringToString(wstrHardwareID);
//	jsonPayload["user_settings"]["hmd_type"] = strHMDType;
	jsonPayload["user_settings"]["ui_offset_y"] = yOffset;
	jsonPayload["user_settings"]["ui_offset_z"] = zOffset;
	jsonPayload["user_settings"]["ui_scale"] = scale;

	pCloudRequest = CloudMessage::CreateRequest(pParentCloudController, jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("user.set_settings"));

	auto pEnvironmentController = dynamic_cast<EnvironmentController*>(pParentCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentController);
	CR(pEnvironmentController->SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::USER_SET_SETTINGS));

Error:
	return r;
}

RESULT UserController::RegisterUserControllerObserver(UserControllerObserver* pUserControllerObserver) {
	RESULT r = R_PASS;

	CNM((pUserControllerObserver), "Observer cannot be nullptr");
	CBM((m_pUserControllerObserver == nullptr), "Can't overwrite environment observer");
	m_pUserControllerObserver = pUserControllerObserver;

Error:
	return r;
}

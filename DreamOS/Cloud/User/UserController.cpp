#include "UserController.h"
#include "DreamLogger/DreamLogger.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Sandbox/CommandLineManager.h"
#include "json.hpp"

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

	//form
	CR(RegisterMethod("get_form", std::bind(&UserController::OnSettingsForm, this, std::placeholders::_1)));

Error:
	return r;
}

// TODO: Move to Controller - register methods etc
std::string UserController::GetMethodURI(UserMethod userMethod) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strURI = "";
	std::string strAPIURL = pCommandLineManager->GetParameterValue("api.ip");
	std::string strWWWURL = pCommandLineManager->GetParameterValue("www.ip");

	switch (userMethod) {
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
			strURI = strAPIURL + "/twilio/nts/token/";
		} break;
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
		m_fLoggedIn = true;

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
	m_fLoggedIn = true;

	DOSLOG(INFO, "[UserController] User %v logged in", strUsername);

Error:
	return r;
}

long UserController::GetUserDefaultEnvironmentID() {
	return m_user.GetDefaultEnvironmentID();
}

User UserController::GetUser() {
	return m_user;
}

TwilioNTSInformation UserController::GetTwilioNTSInformation() {
	return m_twilioNTSInformation;
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
			-1,
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

		std::string strAuthorizationToken = "Authorization: Token " + GetUserToken();

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
		}
		
		DEBUG_LINEOUT("User Profile Loaded");
	}

Error:
	return r;
}

bool UserController::IsLoggedIn() {
	return m_fLoggedIn;
}

UserControllerProxy* UserController::GetUserControllerProxy() {
	return (UserControllerProxy*)(this);
}

std::string UserController::GetUserToken() {
	return m_strToken;
}

std::string UserController::GetPeerScreenName(long peerUserID) {
	GetPeerProfile(peerUserID);
	return m_strPeerScreenName;
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
		std::string strAuthorizationToken = "Authorization: Token " + m_strToken;
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
		RequestSettingsForm("FormKey.Settings");
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

RESULT UserController::OnSettingsForm(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonForm = jsonPayload["/form"_json_pointer];

	// TODO: potentially build this out into a class similar to EnvironmentAsset
	if (!jsonForm.is_null()) {
		if (m_pUserControllerObserver != nullptr) {
			if (jsonForm["/key"_json_pointer].is_string()) {
				/*TODO*/
			}
			if (jsonForm["/title"_json_pointer].is_string()) {
				/*TODO*/
			}
			if (jsonForm["/url"_json_pointer].is_string()) {
				CR(m_pUserControllerObserver->OnSettings(jsonForm["/url"_json_pointer].get<std::string>()));
			}
			// Moving to Send/Receive paradigm
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
	jsonPayload["user_settings"]["instance_id"] = util::WideStringToString(wstrHardwareID);
	jsonPayload["user_settings"]["hmd_type"] = strHMDType;

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
	jsonPayload["user_settings"]["instance_id"] = util::WideStringToString(wstrHardwareID);
	jsonPayload["user_settings"]["hmd_type"] = strHMDType;
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

//TODO: this needs to be moved to EnvironmentController (or possibly Controller)
// so that forms are requested in the same way across all sockets
RESULT UserController::RequestSettingsForm(std::string key) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	CloudController *pParentCloudController = GetCloudController();
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["form"] = nlohmann::json::object();
	jsonPayload["form"]["key"] = key;

	pCloudRequest = CloudMessage::CreateRequest(pParentCloudController, jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("form.get_form"));

	auto pEnvironmentController = dynamic_cast<EnvironmentController*>(pParentCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentController);
	CR(pEnvironmentController->SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::FORM_SETTINGS));

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

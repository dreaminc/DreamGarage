#include "CloudMessage.h"
#include "CloudController.h"

#include "Core/Utilities.h"

CloudMessage::CloudMessage() :
	m_version(0.0f)
{
	// empty
}

CloudMessage::~CloudMessage() {
	// empty
}

std::shared_ptr<CloudMessage> CloudMessage::Create(CloudController *pParentCloudController) {
	RESULT r = R_PASS;

	std::shared_ptr<CloudMessage> pCloudMessage = nullptr;

	CN(pParentCloudController);

	pCloudMessage = std::make_shared<CloudMessage>();
	CN(pCloudMessage);

	pCloudMessage->m_version = pParentCloudController->GetUser().GetVersion();
	pCloudMessage->m_strToken = pParentCloudController->GetUser().GetToken();

//Success:
	return pCloudMessage;

Error:
	return nullptr;
}

// TODO: Handle errors bettar
std::shared_ptr<CloudMessage> CloudMessage::Create(CloudController *pParentCloudController, std::string strJSONData) {
	RESULT r = R_PASS;

	std::shared_ptr<CloudMessage> pCloudMessage = nullptr;

	CN(pParentCloudController);

	pCloudMessage = CloudMessage::Create(pParentCloudController);
	CN(pCloudMessage); 

	if(strJSONData.empty() == false) {

		nlohmann::json jsonData = nlohmann::json::parse(strJSONData);

		std::string strGUID = jsonData["/id"_json_pointer].get<std::string>();
		std::string strType = jsonData["/type"_json_pointer].get<std::string>();
		std::string strControllerMethod = jsonData["/method"_json_pointer].get<std::string>();

		CR(pCloudMessage->SetGUID(strGUID));
		CR(pCloudMessage->SetType(strType));
		CR(pCloudMessage->SetControllerMethod(strControllerMethod));
		CR(pCloudMessage->SetPayload(jsonData["/payload"_json_pointer]));
	}
	
	//Success:
	return pCloudMessage;

Error:
	return nullptr;
}

std::shared_ptr<CloudMessage> CloudMessage::CreateResponse(CloudController *pParentCloudController, nlohmann::json jsonData) {
	RESULT r = R_PASS;

	std::shared_ptr<CloudMessage> pCloudMessage = nullptr;

	CN(pParentCloudController);

	pCloudMessage = CloudMessage::Create(pParentCloudController);
	CN(pCloudMessage);

	pCloudMessage->m_type = CloudMessage::type::RESPONSE;

	pCloudMessage->SetPayload(jsonData);

//Success:
	return pCloudMessage;

Error:
	return nullptr;
}

std::shared_ptr<CloudMessage> CloudMessage::CreateRequest(CloudController *pParentCloudController, nlohmann::json jsonPayload) {
	RESULT r = R_PASS;

	std::shared_ptr<CloudMessage> pCloudMessage = nullptr;

	CN(pParentCloudController);

	pCloudMessage = CloudMessage::Create(pParentCloudController);
	CN(pCloudMessage);

	pCloudMessage->m_type = CloudMessage::type::REQUEST;

	CR(pCloudMessage->SetPayload(jsonPayload));

//Success:
	return pCloudMessage;

Error:
	return nullptr;
}

RESULT CloudMessage::SetControllerMethod(std::string strControllerMethod) {
	RESULT r = R_PASS;

	std::vector<std::string> strTokens = util::TokenizeString(strControllerMethod, '.');
	CB((strTokens.size() == 2));

	CR(SetController(strTokens[0]));
	CR(SetMethod(strTokens[1]));

Error:
	return r;
}


RESULT CloudMessage::SetMethod(std::string strMethod) {
	m_strMethod = strMethod;
	return R_PASS;
}

RESULT CloudMessage::SetController(std::string strController) {
	m_strController = strController;
	return R_PASS;
}

RESULT CloudMessage::SetPayload(nlohmann::json jsonPayload) {
	m_jsonPayload = jsonPayload;
	return R_PASS;
}

nlohmann::json CloudMessage::GetJSONPayload() {
	return m_jsonPayload;
}

std::string CloudMessage::GetJSONDataString(int indent) {
	nlohmann::json jsonData;

	jsonData["id"] = m_GUID.GetGUIDString();
	//jsonData["token"] = m_strToken;
	
	switch (m_type) {
		case type::REQUEST:		jsonData["type"] = "request"; break;
		case type::RESPONSE:	jsonData["type"] = "response"; break;
		default:				jsonData["type"] = "invalid"; break;
	}

	jsonData["method"] = m_strController + '.' + m_strMethod;
	jsonData["version"] = m_version.GetString(false);
	jsonData["payload"] = m_jsonPayload;

	if (m_type == type::RESPONSE) {
		jsonData["errors"] = nlohmann::json::object();
		jsonData["meta"] = nlohmann::json::object();
	}

	return jsonData.dump(indent);
}

std::string CloudMessage::GetController() {
	return m_strController;
}

std::string CloudMessage::GetMethod() {
	return m_strMethod;
}

CloudMessage::type CloudMessage::GetType() {
	return m_type;
}

RESULT CloudMessage::SetGUID(std::string strGUID) {
	return m_GUID.SetGUIDFromString(strGUID);
}

RESULT CloudMessage::SetType(std::string strType) {
	RESULT r = R_PASS;

	std::transform(strType.begin(), strType.end(), strType.begin(), ::tolower);
	//util::tolowerstring(strType);

	if (strType == "request") 
		m_type = type::REQUEST;
	else if (strType == "response") 
		m_type = type::RESPONSE;
	else 
		m_type = type::INVALID;

//Error:
	return r;
}

RESULT CloudMessage::Print(int indent) {
	DEBUG_LINEOUT(GetJSONDataString(indent).c_str());
	return R_PASS;
}

RESULT CloudMessage::PrintPayload(int indent) {
	DEBUG_LINEOUT(m_jsonPayload.dump(indent).c_str());
	return R_PASS;
}
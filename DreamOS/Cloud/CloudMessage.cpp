#include "CloudMessage.h"
#include "CloudController.h"

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

std::shared_ptr<CloudMessage> CloudMessage::CreateResponse(CloudController *pParentCloudController, nlohmann::json jsonData) {
	RESULT r = R_PASS;

	std::shared_ptr<CloudMessage> pCloudMessage = nullptr;

	CN(pParentCloudController);

	pCloudMessage = CloudMessage::Create(pParentCloudController);
	CN(pCloudMessage);

	pCloudMessage->m_type = CloudMessage::type::RESPONSE;

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

RESULT CloudMessage::SetMethod(std::string strMethod) {
	m_strMethod = strMethod;
	return R_PASS;
}

RESULT CloudMessage::SetPayload(nlohmann::json jsonPayload) {
	m_jsonPayload = jsonPayload;
	return R_PASS;
}

std::string CloudMessage::GetJSONDataString() {
	nlohmann::json jsonData;

	jsonData["id"] = m_GUID.GetGUIDString();
	jsonData["token"] = m_strToken;
	
	switch (m_type) {
		case type::REQUEST:		jsonData["type"] = "request"; break;
		case type::RESPONSE:	jsonData["type"] = "response"; break;
		default:				jsonData["type"] = "invalid"; break;
	}

	jsonData["method"] = m_strMethod;
	jsonData["version"] = m_version.GetString(false);
	jsonData["payload"] = m_jsonPayload;

	return jsonData.dump();
}
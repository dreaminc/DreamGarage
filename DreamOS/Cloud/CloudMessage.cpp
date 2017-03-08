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

	

//Success:
	return pCloudMessage;

Error:
	return nullptr;
}

RESULT CloudMessage::SetMethod(std::string strMethod) {
	m_strMethod = strMethod;
	return R_PASS;
}
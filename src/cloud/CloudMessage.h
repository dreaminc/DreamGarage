#ifndef CLOUD_MESSAGE_H_
#define CLOUD_MESSAGE_H_

#include "core/ehm/EHM.h"

// DREAM Cloud
// dos/src/cloud/CloudResponse.h

// The Cloud Response Object

#include <string>

#include "core/types/GUID.h"
#include "core/types/version.h"

#include "third_party/json/include/json.hpp"

class CloudController;

class CloudMessage {
public:
	enum class type {
		REQUEST,
		RESPONSE,
		INVALID
	};


public:
	CloudMessage();
	~CloudMessage();

public:
	static std::shared_ptr<CloudMessage> Create(CloudController *pParentCloudController);
	static std::shared_ptr<CloudMessage> Create(CloudController *pParentCloudController, std::string strJSONData);

	static std::shared_ptr<CloudMessage> CreateResponse(CloudController *pParentCloudController, nlohmann::json jsonData);

	static std::shared_ptr<CloudMessage> CreateRequest(CloudController *pParentCloudController, nlohmann::json jsonPayload);

public:
	RESULT SetControllerMethod(std::string strControllerMethod);
	RESULT SetMethod(std::string strMethod);
	RESULT SetController(std::string strController);

	RESULT SetPayload(nlohmann::json jsonPayload);
	nlohmann::json GetJSONPayload();

	RESULT SetGUID(std::string strGUID);
	RESULT SetType(std::string strType);

	std::string GetJSONDataString(int indent = -1);
	std::string GetController();
	std::string GetMethod();
	CloudMessage::type GetType();

	RESULT Print(int indent = -1);
	RESULT PrintPayload(int indent = -1);

private:
	CloudMessage::type m_type;
	GUID m_GUID;
	version m_version;
	std::string m_strToken;
	
	std::string m_strMethod;
	std::string m_strController;

	nlohmann::json m_jsonPayload;
};

#endif // ! CLOUD_MESSAGE_H_


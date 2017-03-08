#ifndef CLOUD_MESSAGE_H_
#define CLOUD_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/CloudResponse.h
// The Cloud Response Object

#include "Primitives/Types/guid.h"
#include "Primitives/version.h"
#include "json.hpp"

#include <string>

class CloudController;

class CloudMessage {
public:
	enum class type {
		REQUEST,
		RESPONSE,
		INVALID
	};

private:
	CloudMessage();

public:
	~CloudMessage();

public:
	static std::shared_ptr<CloudMessage> Create(CloudController *pParentCloudController);
	static std::shared_ptr<CloudMessage> CreateResponse(CloudController *pParentCloudController, nlohmann::json jsonData);
	static std::shared_ptr<CloudMessage> CreateRequest(CloudController *pParentCloudController, nlohmann::json jsonPayload);

public:
	RESULT SetMethod(std::string strMethod);

private:
	CloudMessage::type m_type;
	guid m_GUID;
	version m_version;
	std::string m_strToken;
	std::string m_strMethod;

	nlohmann::json jsonData;
};

#endif // ! CLOUD_MESSAGE_H_


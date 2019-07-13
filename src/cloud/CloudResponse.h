#ifndef CLOUD_RESPONSE_H_
#define CLOUD_RESPONSE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/CloudResponse.h
// The Cloud Response Object

#include "Primitives/Types/guid.h"
#include "Primitives/version.h"
#include "json.hpp"

#include <string>

class CloudResponse {
private:
	CloudResponse();

public:
	~CloudResponse();

public:
	static CloudResponse Create(nlohmann::json jsonData);

private:
	nlohmann::json jsonData;
	guid m_GUID;
	version m_version;
	std::string m_token;
	std::string m_method;
};

#endif // ! CLOUD_RESPONSE_H_


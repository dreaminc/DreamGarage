#ifndef CLOUD_REQUEST_H_
#define CLOUD_REQUEST_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/CloudRequest.h
// The Cloud Request Object

#include "Primitives/Types/guid.h"
#include "Primitives/version.h"
#include "json.hpp"

#include <string>

class CloudRequest {
private:
	CloudRequest();

public:
	~CloudRequest();
	
public:
	static CloudRequest Create(nlohmann::json jsonPayload);

private:
	nlohmann::json jsonData;
	guid m_GUID;
	version m_version;
	std::string m_token;
	std::string m_method;
};

#endif // ! CLOUD_REQUEST_H_


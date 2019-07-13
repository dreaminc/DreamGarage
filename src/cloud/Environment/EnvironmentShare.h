#ifndef ENVIRONMENT_SHARE_H_
#define ENVIRONMENT_SHARE_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/environment/EnvironmentShare.h

// TODO: Rename or make more general

#include <string>

#include "json.hpp"

#include "core/types/dirty.h"

class EnvironmentShare : public dirty {
public:
	EnvironmentShare(nlohmann::json jsonEnvironmentShare);
	~EnvironmentShare();

	long GetID();
	long GetAssetID();
	long GetSessionID();
	long GetUserID();

	std::string GetShareType();

	nlohmann::json MakeJsonPayload();

private:
	// share

	long m_id = -1;
	long m_assetID = -1;
	long m_sessionID = -1;
	long m_userID = -1;
	std::string m_strShareType;
};

#endif // ! ENVIRONMENT_SHARE_H_
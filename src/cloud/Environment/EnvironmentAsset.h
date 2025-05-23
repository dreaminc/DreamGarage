#ifndef ENVIRONMENT_ASSET_H_
#define ENVIRONMENT_ASSET_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/Environment/EnvironmnetAsset.h

// Environment Asset Model

#include <string>
#include <vector>

#include "core/types/dirty.h"

#include "third_party/json/include/json.hpp"

enum class ResourceHandlerType {
	DEFAULT,
	DREAM
};

class EnvironmentAsset : public dirty {
public:
	EnvironmentAsset(nlohmann::json jsonMenuNode);
	EnvironmentAsset(long m_assetID, std::string strPath, std::string strStorageProviderScope, std::string strTitle);

	RESULT PrintEnvironmentAsset();

	const std::string& GetPath();
	const std::string& GetScope();
	const std::string& GetTitle();
	const std::string& GetContentType();
	RESULT SetContentType(std::string strContentType);

	std::string GetURI();
	const std::string& GetURL();
	ResourceHandlerType GetResourceHandlerType();
	std::multimap<std::string, std::string> GetHeaders();
	long GetAssetID();
	long GetUserID();

private:
	long m_assetID;
	long m_userID = -1; // sharing user id used with on receive asset
	std::string m_strPath;
	std::string m_strStorageProviderScope;
	std::string m_strTitle;
	std::string m_strURL;
	ResourceHandlerType m_resourceHandlerType;
	std::string m_strContentType;

	std::multimap<std::string, std::string> m_headers;
};

#endif	// ! ENVIRONMENT_ASSET_H_

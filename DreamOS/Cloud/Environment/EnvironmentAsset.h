#ifndef ENVIRONMENT_ASSET_H_
#define ENVIRONMENT_ASSET_H_

#include "RESULT/EHM.h"
#include "Primitives/dirty.h"

// DREAM OS
// DreamOS/Dimension/Cloud/EnvironmnetAsset/EnvironmnetAsset.h
// Environment Asset Model

#include <string>
#include <vector>

#include "json.hpp"

class EnvironmentAsset : public dirty {
public:
	EnvironmentAsset(nlohmann::json jsonMenuNode);
	//EnvironmnetAsset(long m_assetID, MenuNode::MimeType mimeType, std::string strPath, std::string strStorageProviderScope, std::string strTitle);
	EnvironmentAsset(long m_assetID, std::string strPath, std::string strStorageProviderScope, std::string strTitle);

	RESULT PrintEnvironmentAsset();

	const std::string& GetPath();
	const std::string& GetStorageProviderScope();
	//const MenuNode::MimeType & GetMIMEType();
	const std::string& GetTitle();

	std::string GetURI();

private:
	long m_assetID;
	std::string m_strPath;
	std::string m_strStorageProviderScope;
	std::string m_strTitle;
	//MenuNode::MimeType m_mimeType;
};

#endif	// ! ENVIRONMENT_ASSET_H_

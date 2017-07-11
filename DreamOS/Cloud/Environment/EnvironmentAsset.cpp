#include "EnvironmentAsset.h"

#include "Cloud/Menu/MenuNode.h"

EnvironmentAsset::EnvironmentAsset(nlohmann::json jsonMenuNode) {
	if (jsonMenuNode["/id"_json_pointer].is_number_integer())
		m_assetID = jsonMenuNode["/id"_json_pointer].get<long>();
	
	if (jsonMenuNode["/title"_json_pointer].is_string())
		m_strTitle = jsonMenuNode["/title"_json_pointer].get<std::string>();

	if (jsonMenuNode["/path"_json_pointer].is_string())
		m_strPath = jsonMenuNode["/path"_json_pointer].get<std::string>();

	if (jsonMenuNode["/storage_provider_scope"_json_pointer].is_string())
		m_strStorageProviderScope = jsonMenuNode["/storage_provider_scope"_json_pointer].get<std::string>();
	else if (jsonMenuNode["/scope"_json_pointer].is_string())
		m_strStorageProviderScope = jsonMenuNode["/scope"_json_pointer].get<std::string>();

	/*
	if (jsonMenuNode["/mime_type"_json_pointer].is_string())
		m_mimeType = MenuNode::MimeTypeFromString(jsonMenuNode["/mime_type"_json_pointer].get<std::string>());
	*/
}

//EnvironmentAsset::EnvironmentAsset(long m_assetID, MenuNode::MimeType mimeType, std::string strPath, std::string strStorageProviderScope, std::string strTitle) :
EnvironmentAsset::EnvironmentAsset(long m_assetID, std::string strPath, std::string strStorageProviderScope, std::string strTitle) :
	m_strPath(strPath),
	m_strStorageProviderScope(strStorageProviderScope),
	m_strTitle(strTitle)
	//m_mimeType(mimeType)
{
	// empty
}

RESULT EnvironmentAsset::PrintEnvironmentAsset() {
	
	DEBUG_LINEOUT("Asset title: %s", m_strTitle.c_str());
	DEBUG_LINEOUT("Asset path: %s", m_strPath.c_str());
	DEBUG_LINEOUT("Asset scope: %s", m_strStorageProviderScope.c_str());
	//DEBUG_LINEOUT("Asset MIME type: %s", MenuNode::MimeTypeString(m_mimeType).c_str());
	DEBUG_LINEOUT("Asset ID: %d", m_assetID);

	return R_PASS;
}

/*
const MenuNode::MimeType& EnvironmentAsset::GetMIMEType() {
	return m_mimeType;
}
*/

const std::string& EnvironmentAsset::GetPath() {
	return m_strPath;
}

const std::string& EnvironmentAsset::GetStorageProviderScope() {
	return m_strStorageProviderScope;
}

const std::string& EnvironmentAsset::GetTitle() {
	return m_strTitle;
}

std::string EnvironmentAsset::GetURI() {
	std::string strURI = "https://api.develop.dreamos.com/environment-asset/";
	strURI += std::to_string(m_assetID);
	
	//strURI += "/file";
	strURI += "/content";

	return strURI;
}
#include "EnvironmentAsset.h"

#include "Cloud/Menu/MenuNode.h"
#include "Sandbox/CommandLineManager.h"

EnvironmentAsset::EnvironmentAsset(nlohmann::json jsonMenuNode) {
	if (jsonMenuNode["/id"_json_pointer].is_number_integer())
		m_assetID = jsonMenuNode["/id"_json_pointer].get<long>();

	if (jsonMenuNode["/user"_json_pointer].is_number_integer())
		m_userID = jsonMenuNode["/user"_json_pointer].get<long>();
	
	if (jsonMenuNode["/title"_json_pointer].is_string())
		m_strTitle = jsonMenuNode["/title"_json_pointer].get<std::string>();

	if (jsonMenuNode["/path"_json_pointer].is_string())
		m_strPath = jsonMenuNode["/path"_json_pointer].get<std::string>();

	if (jsonMenuNode.find("external_request") != jsonMenuNode.end()) {
		auto check = jsonMenuNode.at("external_request");
		if (!check.is_null()) {
			if (jsonMenuNode["/external_request/resource_handler"_json_pointer].is_string()) {

				std::string strResourceHandlerType = jsonMenuNode["/external_request/resource_handler"_json_pointer].get<std::string>();
				if (strResourceHandlerType == "ResourceHandler.Dream") {
					m_resourceHandlerType = ResourceHandlerType::DREAM;
				}

				else {
					m_resourceHandlerType = ResourceHandlerType::DEFAULT;
				}
			}

			if (jsonMenuNode["/external_request/url"_json_pointer].is_string())
				m_strURL = jsonMenuNode["/external_request/url"_json_pointer].get<std::string>();
			
			if (jsonMenuNode["/external_request/headers"_json_pointer].is_object()) {
				auto jsonHeaders = jsonMenuNode["/external_request/headers"_json_pointer].get<std::multimap<std::string, nlohmann::json>>();
				for (const auto it : jsonHeaders) {
					if (it.second.is_string()) {
						std::string strsecond = it.second.get<std::string>();
						m_headers.insert(std::pair<std::string, std::string>(it.first, strsecond));
					}

				}
			}
		}
	}

	if (jsonMenuNode["/content_control_type"_json_pointer].is_string()) {
		m_strContentType = jsonMenuNode["/content_control_type"_json_pointer].get<std::string>();
	}

	if (jsonMenuNode["/storage_provider_scope"_json_pointer].is_string())
		m_strStorageProviderScope = jsonMenuNode["/storage_provider_scope"_json_pointer].get<std::string>();
	else if (jsonMenuNode["/scope"_json_pointer].is_string())
		m_strStorageProviderScope = jsonMenuNode["/scope"_json_pointer].get<std::string>();

	/*
	if (jsonMenuNode["/mime_type"_json_pointer].is_string())
		m_mimeType = MenuNode::MimeTypeFromString(jsonMenuNode["/mime_type"_json_pointer].get<std::string>());
	*/
}

EnvironmentAsset::EnvironmentAsset(long m_assetID, std::string strPath, std::string strStorageProviderScope, std::string strTitle) :
	m_strPath(strPath),
	m_strStorageProviderScope(strStorageProviderScope),
	m_strTitle(strTitle)
{
	// empty
}

RESULT EnvironmentAsset::PrintEnvironmentAsset() {
	
	DEBUG_LINEOUT("Asset title: %s", m_strTitle.c_str());
	DEBUG_LINEOUT("Asset path: %s", m_strPath.c_str());
	DEBUG_LINEOUT("Asset scope: %s", m_strStorageProviderScope.c_str());
	DEBUG_LINEOUT("Asset ID: %d", m_assetID);

	return R_PASS;
}

const std::string& EnvironmentAsset::GetPath() {
	return m_strPath;
}

const std::string& EnvironmentAsset::GetScope() {
	return m_strStorageProviderScope;
}

const std::string& EnvironmentAsset::GetTitle() {
	return m_strTitle;
}

const std::string& EnvironmentAsset::GetURL() {
	return m_strURL;
}

ResourceHandlerType EnvironmentAsset::GetResourceHandlerType() {
	return m_resourceHandlerType;
}

const std::string& EnvironmentAsset::GetContentType() {
	return m_strContentType;
}

RESULT EnvironmentAsset::SetContentType(std::string strContentType) {
	m_strContentType = strContentType;
	return R_PASS;
}

std::multimap<std::string, std::string> EnvironmentAsset::GetHeaders() {
	return m_headers;
}

long EnvironmentAsset::GetAssetID() {
	return m_assetID;
}

long EnvironmentAsset::GetUserID() {
	return m_userID;
}

std::string EnvironmentAsset::GetURI() {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strURI = "";
	std::string strAPIURL = pCommandLineManager->GetParameterValue("api.ip");
	
	//std::string strURI = "https://api.develop.dreamos.com/environment-asset/";

	strURI = strAPIURL + "/environment-asset/";
	strURI += std::to_string(m_assetID);
	
	//strURI += "/file";
	strURI += "/content";

	return strURI;
}
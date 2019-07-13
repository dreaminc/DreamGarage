#include "EnvironmentShare.h"

EnvironmentShare::EnvironmentShare(nlohmann::json jsonEnvironmentShare) {

	if (jsonEnvironmentShare["/id"_json_pointer].is_number_integer())
		m_id = jsonEnvironmentShare["/id"_json_pointer].get<long>();
	if (jsonEnvironmentShare["/asset"_json_pointer].is_number_integer())
		m_assetID = jsonEnvironmentShare["/asset"_json_pointer].get<long>();
	if (jsonEnvironmentShare["/session"_json_pointer].is_number_integer())
		m_sessionID = jsonEnvironmentShare["/session"_json_pointer].get<long>();
	if (jsonEnvironmentShare["/user"_json_pointer].is_number_integer())
		m_userID = jsonEnvironmentShare["/user"_json_pointer].get<long>();

	if (jsonEnvironmentShare["/share_type"_json_pointer].is_string())
		m_strShareType = jsonEnvironmentShare["/share_type"_json_pointer].get<std::string>();

}

EnvironmentShare::~EnvironmentShare() {
	// empty
}

long EnvironmentShare::GetID() {
	return m_id;
}

long EnvironmentShare::GetAssetID() {
	return m_assetID;
}

long EnvironmentShare::GetSessionID() {
	return m_sessionID;
}

long EnvironmentShare::GetUserID() {
	return m_userID;
}

std::string EnvironmentShare::GetShareType() {
	return m_strShareType;
}

nlohmann::json EnvironmentShare::MakeJsonPayload() {

	nlohmann::json jsonPayload;
	jsonPayload["environment_share"] = nlohmann::json::object();
	jsonPayload["environment_share"]["id"] = m_id;
	jsonPayload["environment_share"]["asset"] = m_assetID;
	jsonPayload["environment_share"]["user"] = m_userID;
	jsonPayload["environment_share"]["session"] = m_sessionID;
	jsonPayload["environment_share"]["share_type"] = m_strShareType;

	return jsonPayload;
}

#include "EnvironmentShare.h"

EnvironmentShare::EnvironmentShare(nlohmann::json jsonEnvironmentShare) {

	if (jsonEnvironmentShare["/id"_json_pointer].is_number_integer())
		m_id = jsonEnvironmentShare["/id"_json_pointer].get<long>();
	if (jsonEnvironmentShare["/asset"_json_pointer].is_number_integer())
		m_assetID = jsonEnvironmentShare["/asset"_json_pointer].get<long>();
	if (jsonEnvironmentShare["/session"_json_pointer].is_number_integer())
		m_sessionID = jsonEnvironmentShare["/id"_json_pointer].get<long>();
	if (jsonEnvironmentShare["/user"_json_pointer].is_number_integer())
		m_userID = jsonEnvironmentShare["/id"_json_pointer].get<long>();

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

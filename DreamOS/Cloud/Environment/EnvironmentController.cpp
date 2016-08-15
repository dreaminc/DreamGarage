#include "EnvironmentController.h"
#include "Cloud/User/User.h"

#include "Cloud/Websockets/Websocket.h"
#include "json.hpp"

EnvironmentController::EnvironmentController(long environmentID) :
	m_fConnected(false),
	m_pEnvironmentWebsocket(nullptr)
{
	m_environment = Environment(environmentID);
}

EnvironmentController::~EnvironmentController() {
	if (m_pEnvironmentWebsocket != nullptr) {
		m_pEnvironmentWebsocket->Stop();
	}
}

RESULT EnvironmentController::InitializeWebsocket(std::string& strURI) {
	RESULT r = R_PASS;

	m_pEnvironmentWebsocket = std::unique_ptr<Websocket>(new Websocket(strURI));
	CN(m_pEnvironmentWebsocket);

Error:
	return r;
}

RESULT EnvironmentController::ConnectToEnvironmentSocket(User user) {
	RESULT r = R_PASS;

	// TODO: Not hard coded!
	if (m_pEnvironmentWebsocket == nullptr) {
		std::string strURI = "ws://localhost:8000/environment/" + std::to_string(user.GetDefaultEnvironmentID()) + "/";
		CR(InitializeWebsocket(strURI));
	}

	m_pEnvironmentWebsocket->SetToken(user.GetToken());

	CRM(m_pEnvironmentWebsocket->Start(), "Failed to start environment websocket");

Error:
	return r;
}


RESULT EnvironmentController::CreateEnvironmentUser(User user) {
	RESULT r = R_PASS;

	CBM((m_fConnected), "Environment socket not connected");

	// TODO: Create Environment User


Error:
	return r;
}


RESULT EnvironmentController::GetEnvironmentPeerList(User user) {
	RESULT r = R_PASS;

	CBM((m_fConnected), "Environment socket not connected");

	// TODO: Get Environment peer list


Error:
	return r;
}
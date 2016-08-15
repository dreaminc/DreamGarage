#ifndef ENVIRONMENT_CONTROLLER_H_
#define ENVIRONMENT_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/EnvironmentController.h
// The Environment Controller

#include <string>
#include <memory>

#include "Environment.h"

class User;
class Websocket;

// TODO: This is actually a UserController - so change the name of object and file
class EnvironmentController {
public:
	EnvironmentController(long environmentID);
	~EnvironmentController();

public:
	RESULT ConnectToEnvironmentSocket(User user);
	RESULT CreateEnvironmentUser(User user);	
	RESULT GetEnvironmentPeerList(User user);

private:
	RESULT InitializeWebsocket(std::string& strURI);

private:
	bool m_fConnected;

	Environment m_environment;

	std::unique_ptr<Websocket> m_pEnvironmentWebsocket;
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_
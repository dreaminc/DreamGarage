#include "MenuController.h"

#include "Cloud/CloudController.h"

#include "Sandbox/CommandLineManager.h"

#include "json.hpp"
#include "Primitives/Types/UID.h"
#include "Primitives/Types/guid.h"

MenuController::MenuController(Controller* pParentController) :
	Controller(pParentController)
{
	// empty
}


MenuController::~MenuController() {
	// 
}

// TODO: Move to Controller - register methods etc
std::string MenuController::GetMethodURI(MenuMethod menuMethod) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strURI = "";
	std::string strAPIURL = pCommandLineManager->GetParameterValue("api.ip");
	std::string strWWWURL = pCommandLineManager->GetParameterValue("www.ip");

	/*
	switch (menuMethod) {
		case MenuMethod::GET_SUB_MENU: {
			strURI = strAPIURL + "/token/";
		} break;
	}
	*/

	return strURI;
}

RESULT MenuController::GetSubMenu() {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());
	guid guidMessage;

//Error:
	return r;
}
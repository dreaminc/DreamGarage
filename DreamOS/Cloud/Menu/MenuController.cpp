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

MenuControllerProxy* MenuController::GetMenuControllerProxy() {
	return (MenuControllerProxy*)(this);
}

CLOUD_CONTROLLER_PROXY_TYPE MenuController::GetControllerType() {
	return CLOUD_CONTROLLER_PROXY_TYPE::MENU;
}

RESULT MenuController::RequestSubMenu() {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetCloudController());
	EnvironmentController *pParentEnvironmentController = dynamic_cast<EnvironmentController*>(GetParentController());
	guid guidMessage;

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CNM(pParentEnvironmentController, "Parent Environment Controller not found or null");

	// TODO: Wrap in request class
	jsonData["id"] = guidMessage.GetGUIDString();
	jsonData["method"] = "menu.get_submenu";

	jsonData["payload"] = nlohmann::json::object();
	jsonData["payload"]["menu"] = nlohmann::json::object();
	jsonData["payload"]["menu"]["node_type"] = "NodeType.Folder";
	jsonData["payload"]["menu"]["path"] = "";
	jsonData["payload"]["menu"]["scope"] = "";

	jsonData["token"] = pParentCloudController->GetUser().GetToken();
	jsonData["type"] = "request";
	jsonData["version"] = pParentCloudController->GetUser().GetVersion().GetString(false);

	strData = jsonData.dump();
	DEBUG_LINEOUT("Create Environment User JSON: %s", strData.c_str());

	CR(pParentEnvironmentController->SendEnvironmentSocket(strData, EnvironmentController::state::MENU_API_REQUEST));

Error:
	return r;
}
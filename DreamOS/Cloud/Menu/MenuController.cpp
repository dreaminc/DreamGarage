#include "MenuController.h"

#include "Cloud/CloudController.h"

#include "Sandbox/CommandLineManager.h"

#include "json.hpp"
#include "Primitives/Types/UID.h"
#include "Primitives/Types/guid.h"

#include "Cloud/CloudMessage.h"

#include "Cloud/Menu/MenuNode.h"

MenuController::MenuController(Controller* pParentController) :
	Controller(pParentController)
{
	// empty
}


MenuController::~MenuController() {
	// empty
}

CLOUD_CONTROLLER_TYPE MenuController::observer::GetControllerType() {
	return CLOUD_CONTROLLER_TYPE::MENU;
}

RESULT MenuController::Initialize() {
	RESULT r = R_PASS;

	// Register Methods
	CR(RegisterMethod("get_submenu", std::bind(&MenuController::OnGetSubMenu, this, std::placeholders::_1)));

Error:
	return r;
}

//RESULT MenuController::RegisterMenuControllerObserver(MenuController::observer* pMenuControllerObserver) {
RESULT MenuController::RegisterControllerObserver(ControllerObserver* pControllerObserver) {
	RESULT r = R_PASS;

	MenuController::observer *pMenuControllerObserver = dynamic_cast<MenuController::observer*>(pControllerObserver);
	
	CNM((pMenuControllerObserver), "Observer invalid or cannot be nullptr");
	CBM((m_pMenuControllerObserver == nullptr), "Can't overwrite menu observer");
	m_pMenuControllerObserver = pMenuControllerObserver;

Error:
	return r;
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

RESULT MenuController::HandleEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	CR(HandleOnMethodCallback(pCloudMessage));

Error:
	return r;
}

RESULT MenuController::OnGetSubMenu(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonMenu = jsonPayload["/menu"_json_pointer];

	if (m_pMenuControllerObserver != nullptr) {
		std::shared_ptr<MenuNode> pMenuNode = nullptr;
		if (jsonMenu.size() != 0) 
			pMenuNode = std::make_shared<MenuNode>(jsonMenu);
			
		CR(m_pMenuControllerObserver->OnMenuData(pMenuNode));
	}

Error:
	return r;
}

MenuControllerProxy* MenuController::GetMenuControllerProxy() {
	return (MenuControllerProxy*)(this);
}

CLOUD_CONTROLLER_TYPE MenuController::GetControllerType() {
	return CLOUD_CONTROLLER_TYPE::MENU;
}

RESULT MenuController::RequestSubMenu(std::string strScope, std::string strPath) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetCloudController());
	EnvironmentController *pParentEnvironmentController = dynamic_cast<EnvironmentController*>(GetParentController());
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CNM(pParentEnvironmentController, "Parent Environment Controller not found or null");

	jsonPayload["menu"] = nlohmann::json::object();
	jsonPayload["menu"]["node_type"] = "NodeType.Folder";
	jsonPayload["menu"]["path"] = strPath;
	jsonPayload["menu"]["scope"] = strScope;

	pCloudRequest = CloudMessage::CreateRequest(pParentCloudController, jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("menu.get_submenu"));

	CR(pParentEnvironmentController->SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::MENU_API_REQUEST));

Error:
	return r;
}
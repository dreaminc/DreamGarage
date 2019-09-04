#ifndef MENU_CONTROLLER_H_
#define MENU_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Menu/MenuController.h
// The Menu Controller 

#include "Cloud/Controller.h"
#include "Cloud/ControllerProxy.h"
#include "Cloud/ControllerObserver.h"

#include <string>

//#include "Cloud/Menu/Menu.h"

class CloudMessage;
class MenuNode;

class MenuControllerProxy : public ControllerProxy {
public:
	//virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
	virtual RESULT RequestSubMenu(std::string strScope = "", std::string strPath = "", std::string strTitle = "", std::string strNextPageToken = "") = 0;
};

// TODO: This is actually a UserController - so change the name of object and file
class MenuController : public Controller, public MenuControllerProxy {
public:
	enum class MenuMethod {		
		INVALID
	};

public:
	// TODO: Convert to a proper controller observer pattern?
	class observer : public ControllerObserver {
	public:
		virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
		
		virtual RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode) = 0;
	};

	//RESULT RegisterMenuControllerObserver(MenuController::observer* pMenuControllerObserver);

public:
	MenuController(Controller* pParentController);
	~MenuController();

	RESULT Initialize();

	RESULT HandleEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudMessage);

	RESULT OnGetSubMenu(std::shared_ptr<CloudMessage> pCloudMessage);

private:
	std::string GetMethodURI(MenuMethod menuMethod);

	// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
	//private:
public:
	MenuControllerProxy* GetMenuControllerProxy();

	// Menu Controller Proxy
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
	virtual RESULT RequestSubMenu(std::string strScope = "", std::string strPath = "", std::string strTitle = "", std::string strNextPageToken = "") override;
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) override;

private:
	//Menu m_menu;

	MenuController::observer *m_pMenuControllerObserver;
};

#endif	// ! USER_CONTROLLER_H_
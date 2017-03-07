#ifndef MENU_CONTROLLER_H_
#define MENU_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Menu/MenuController.h
// The Menu Controller 

#include "Cloud/Controller.h"
#include "Cloud/ControllerProxy.h"

#include <string>

//#include "Cloud/Menu/Menu.h"

class MenuControllerProxy : ControllerProxy {
public:
	virtual CLOUD_CONTROLLER_PROXY_TYPE GetControllerType() = 0;
	virtual RESULT RequestSubMenu() = 0;
};

// TODO: This is actually a UserController - so change the name of object and file
class MenuController : public Controller, public MenuControllerProxy {
public:
	enum class MenuMethod {		
		INVALID
	};

public:
	class MenuControllerObserver {
	public:
		virtual RESULT OnMenuData() = 0;
	};

public:
	MenuController(Controller* pParentController);
	~MenuController();

	RESULT Initialize() {
		return R_NOT_IMPLEMENTED;
	}


private:
	std::string GetMethodURI(MenuMethod menuMethod);

	// TODO: Move to private when CommandLineManager is brought in from WebRTC branch
	//private:
public:
	MenuControllerProxy* GetMenuControllerProxy();

	// Menu Controller Proxy
	virtual CLOUD_CONTROLLER_PROXY_TYPE GetControllerType() override;
	virtual RESULT RequestSubMenu() override;

private:
	//Menu m_menu;
};

#endif	// ! USER_CONTROLLER_H_
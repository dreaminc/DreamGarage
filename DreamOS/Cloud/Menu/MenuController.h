#ifndef MENU_CONTROLLER_H_
#define MENU_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Menu/MenuController.h
// The Menu Controller 

#include "Cloud/Controller.h"

#include <string>

//#include "Cloud/Menu/Menu.h"

// TODO: This is actually a UserController - so change the name of object and file
class MenuController : public Controller {
public:
	enum class MenuMethod {		
		INVALID
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
	RESULT GetSubMenu();

private:

	//Menu m_menu;
};

#endif	// ! USER_CONTROLLER_H_
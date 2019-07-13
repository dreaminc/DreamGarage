#include "UserFactory.h"
#include "UserController.h"

#include <memory>

UserController* UserFactory::MakeUserController(version ver, Controller* pParentController) {
	RESULT r = R_PASS;
	UserController *pUser = nullptr;

	pUser = new UserController(pParentController);
	CN(pUser);

	//Success:
	return pUser;

Error:
	if (pUser != nullptr) {
		delete pUser;
		pUser = nullptr;
	}

	return nullptr;
}

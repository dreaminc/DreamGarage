#include "UserFactory.h"
#include "UserController.h"

#include <memory>

UserController* UserFactory::MakeUserController(version ver) {
	RESULT r = R_PASS;
	UserController *pUser = nullptr;

	pUser = new UserController();
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

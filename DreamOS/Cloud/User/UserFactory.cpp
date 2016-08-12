#include "UserFactory.h"
#include "User.h"

#include <memory>

User* UserFactory::MakeUser(version ver) {
	RESULT r = R_PASS;
	User *pUser = nullptr;

	pUser = new User();
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

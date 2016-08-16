#ifndef USER_FACTORY_H_
#define USER_FACTORY_H_

#include "RESULT/EHM.h"
#include "UserController.h"
#include "Core/Types/Factory.h"

// DREAM OS
// DreamOS/Cloud/User/UserFactory.h
// Dream OS User factory for the creation of users across various versions

#include "Primitives/version.h"

class UserFactory : public Factory {
public:
	static UserController* MakeUserController(version ver);
};

#endif // ! USER_FACTORY_H_

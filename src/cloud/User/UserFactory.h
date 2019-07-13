#ifndef USER_FACTORY_H_
#define USER_FACTORY_H_

#include "core/ehm/EHM.h"

// Dream Cloud User
// dos/src/cloud/User/UserFactory.h

// Dream OS User factory for the creation of users across various versions

#include "UserController.h"

#include "core/types/version.h"
#include "core/types/Factory.h"

class UserFactory : public Factory {
public:
	static UserController* MakeUserController(version ver, Controller* pParentController);
};

#endif // ! USER_FACTORY_H_

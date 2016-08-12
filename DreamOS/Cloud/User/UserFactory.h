#ifndef USER_FACTORY_H_
#define USER_FACTORY_H_

#include "RESULT/EHM.h"
#include "User.h"
#include "Core/Types/Factory.h"

// DREAM OS
// DreamOS/Cloud/User/UserFactory.h
// Dream OS User factory for the creation of users across various versions

#include "Primitives/version.h"

class UserFactory : public Factory {
public:
	static User* MakeUser(version ver);
};

#endif // ! USER_FACTORY_H_

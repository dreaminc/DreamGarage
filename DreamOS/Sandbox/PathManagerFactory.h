#ifndef PATH_MANAGER_FACTORY_H_
#define PATH_MANAGER_FACTORY_H_

#include "RESULT/EHM.h"
#include "Sandbox/PathManager.h"
#include "Core/Types/Factory.h"

// DREAM OS
// DreamOS/Sandbox/PathManagerFactory.h
// Dream OS Path Manager Factory for the construction of path managers within a given
// sandbox or native implementation

typedef enum {
	PATH_MANAGER_WIN32,
	PATH_MANAGER_OSX,
	PATH_MANAGER_INVALID
} PATH_MANAGER_TYPE;

class PathManagerFactory : public Factory {
public:
	static PathManager* MakePathManager(PATH_MANAGER_TYPE type);
};

#endif // ! PATH_MANAGER_FACTORY_H_

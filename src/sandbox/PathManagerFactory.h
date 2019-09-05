#ifndef PATH_MANAGER_FACTORY_H_
#define PATH_MANAGER_FACTORY_H_

#include "core/ehm/EHM.h"


// Dream Sandbox
// dos/src/andbox/PathManagerFactory.h

// Dream OS Path Manager Factory for the construction of path managers within a given
// sandbox or native implementation

#include "Sandbox/PathManager.h"
#include "Core/Types/Factory.h"

class DreamOS;

typedef enum {
	PATH_MANAGER_WIN32,
	PATH_MANAGER_OSX,
	PATH_MANAGER_INVALID
} PATH_MANAGER_TYPE;

class PathManagerFactory : public Factory {
public:
	static PathManager* MakePathManager(PATH_MANAGER_TYPE type, DreamOS *pDOSHandle);
};

#endif // ! PATH_MANAGER_FACTORY_H_

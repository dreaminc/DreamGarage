#include "PathManagerFactory.h"

#include "Sandbox/Windows/Win64PathManager.h"

PathManager* PathManagerFactory::MakePathManager(PATH_MANAGER_TYPE type) {
	PathManager *pPathManager = NULL;

	switch (type) {
		case PATH_MANAGER_WIN32: {
			pPathManager = new Win64PathManager();
			pPathManager->InitializePaths();
		} break;

		case PATH_MANAGER_OSX:
		default: {

		} break;
	}

	return pPathManager;
}

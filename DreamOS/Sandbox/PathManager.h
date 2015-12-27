#ifndef PATH_MANAGER_H_
#define PATH_MANAGER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"

// DREAM OS
// DreamOS/Sandbox/PathManager.h
// Dream OS Sandbox path manager - this class effectively manages the given paths of the Sandbox
// and ultimately may or may not be used for the native implementation

// This should be used to scrape Dream OS path
#define DREAM_OS_PATH_ENV "DREAMOSPATH"
#define DREAM_OS_PATHS_FILE "dreampaths.txt"	// TODO: Rename?

class PathManagerFactory;

class PathManager {
	friend class PathManagerFactory;

public:
	PathManager() {
		//ACRM(InitializePaths(), "Path Manager failed to initialzie paths");
	}

	~PathManager() {
		ACRM(Dealloc(), "PathManager failed to deallocate paths");
	}

protected:
	virtual RESULT Dealloc() = 0;					// Deallocate all paths
	virtual RESULT OpenDreamPathsFile() = 0;
	
	// Set up path manager
	virtual RESULT InitializePaths() {
		DEBUG_LINEOUT("PathManager Initialize Paths");
		return R_VIRTUAL;
	}

public:
	// Print Paths
	virtual RESULT PrintPaths() = 0;

private:
	UID m_uid;
};

#endif // ! PATH_MANAGER_H_

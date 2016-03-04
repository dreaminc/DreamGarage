#include "PathManagerFactory.h"

#if defined(_WIN32)
    #if defined(_WIN64)
        #include "Sandbox/Windows/Win64PathManager.h"
    #else
        #include "Sandbox/Windows/Win64PathManager.h"
    #endif
#elif defined(__APPLE__)
    #include "Sandbox/OSX/OSXPathManager.h"
#elif defined(__linux__)
    #include "Sandbox/Linux/LinuxPathManager.h"
#endif

PathManager* PathManagerFactory::MakePathManager(PATH_MANAGER_TYPE type) {
    RESULT r = R_PASS;
    PathManager *pPathManager = NULL;

	switch (type) {
		case PATH_MANAGER_WIN32: {
            #if defined(_WIN32)
                pPathManager = new Win64PathManager();
                CRM(pPathManager->InitializePaths(), "Failed to initialize paths!");
            #else
                pPathManager = NULL;
                DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
            #endif
		} break;

        case PATH_MANAGER_OSX: {
            #if defined(__APPLE__)
                pPathManager = new OSXPathManager();
                CRM(pPathManager->InitializePaths(), "Failed to initialize paths!");
            #else
                pPathManager = NULL;
                DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
            #endif
        } break;
            
		default: {
            pPathManager = NULL;
            DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
		} break;
	}

	PathManager::SetSingletonPathManager(pPathManager);
	return pPathManager;
    
Error:
    if(pPathManager != NULL) {
        delete pPathManager;
        pPathManager = NULL;
    }
    return NULL;
}

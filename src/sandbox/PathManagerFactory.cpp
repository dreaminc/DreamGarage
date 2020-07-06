#include "PathManagerFactory.h"

#if defined(_WIN32)
    #if defined(_WIN64)
        #include "sandbox/win64/Win64PathManager.h"
    #else
        #include "sandbox/win64/Win64PathManager.h"
    #endif
#elif defined(__ANDROID__)
    #include "sandbox/android/AndroidPathManager.h"
#elif defined(__APPLE__)
    #include "sandbox/osx/OSXPathManager.h"
#elif defined(__linux__)
    #include "sandbox/linux/LinuxPathManager.h"
#endif

PathManager* PathManagerFactory::MakePathManager(PATH_MANAGER_TYPE type, DreamOS *pDOSHandle) {
    RESULT r = R_PASS;
    PathManager *pPathManager = NULL;

	CN(pDOSHandle);

	switch (type) {
		case PATH_MANAGER_WIN32: {
            #if defined(_WIN32)
                pPathManager = new Win64PathManager();
            #else
                pPathManager = NULL;
                DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
            #endif
		} break;

		case PATH_MANAGER_ANDROID: {
            #if defined(__ANDROID__)
                pPathManager = new AndroidPathManager();
            #else
			    pPathManager = NULL;
			    DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
            #endif
		} break;

        case PATH_MANAGER_OSX: {
            #if defined(__APPLE__)
                pPathManager = new OSXPathManager();
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

	CNM(pPathManager, "Failed to allocate path manager");

	CRM(pPathManager->InitializePaths(pDOSHandle), "Failed to initialize paths!");

	PathManager::SetSingletonPathManager(pPathManager);
	return pPathManager;
    
Error:
    if(pPathManager != NULL) {
        delete pPathManager;
        pPathManager = NULL;
    }
    return NULL;
}

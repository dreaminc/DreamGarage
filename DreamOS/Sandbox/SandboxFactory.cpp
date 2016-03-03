#include "SandboxFactory.h"

#if defined(_WIN32) 
    #if defined(_WIN64)
        #include "./Sandbox/Windows/Windows64App.h"
    #else
        #include "./Sandbox/Windows/Windows64App.h"
    #endif
#elif defined(__APPLE__)
    #include "./Sandbox/OSX/OSXSandboxApp.h"
#elif defined(__linux__)
    #include "./Sandbox/Linux/LinuxApp.h"
#endif

SandboxApp* SandboxFactory::MakeSandbox(SANDBOX_APP_TYPE type) {
	SandboxApp *pSandbox = NULL;
	
	switch (type) {
		case SANDBOX_APP_WIN32: {
            #if defined(_WIN32)
                pSandbox = new Windows64App(_T("DreamOSSandbox"));
            #else
                pSandbox = NULL;
                DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
            #endif
		} break;

        case SANDBOX_APP_OSX: {
            #if defined(__APPLE__)
                pSandbox = new OSXSandboxApp("DreamOSSandbox");
            #else
                pSandbox = NULL;
                DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
            #endif
        } break;
            
		default: {
            pSandbox = NULL;
            DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
		} break;
	}

	return pSandbox;
}
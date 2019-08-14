#include "SandboxFactory.h"

#if defined(_WIN32) 
	// 'Add an optimus prime comment' - Idan
	extern "C" {
		_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	}
    #if defined(_WIN64)
        #include "sandbox/win64/Win64Sandbox.h"
    #else
        #include "sandbox/win64/Win64Sandbox.h"
    #endif
#elif defined(__APPLE__)
    #include "sandbox/osx/OSXSandbox.h"
#elif defined(__linux__)
    #include "sandbox/linux/LinuxSandbox.h"
#endif

#include "os/DreamOS.h"

Sandbox* SandboxFactory::MakeSandbox(SANDBOX_APP_TYPE type, DreamOS *pDOSHandle) {
	RESULT r = R_PASS;
	Sandbox *pSandbox = NULL;
	
	switch (type) {
		case SANDBOX_APP_WIN32: {
            #if defined(_WIN32)
                pSandbox = new Win64Sandbox(L"DreamOSSandbox");
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

	// Set up the Sandbox
	CRM(pSandbox->InitializePathManager(pDOSHandle), "Failed to initialize Sandbox path manager");
	CRM(pSandbox->InitializeOGLRenderingContext(), "Failed to initialize Sandbox OpenGL rendering context");

	return pSandbox;
Error:
	if (pSandbox != NULL) {
		delete pSandbox;
		pSandbox = NULL;
	}
	return NULL;
}
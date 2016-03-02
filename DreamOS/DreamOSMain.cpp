//#include "DreamOSMain.h"
//#include "OSX/OSXApp.h"

#include "RESULT/EHM.h"

#include "Sandbox/SandboxFactory.h"

#define DREAM_OS_VERSION_STR "V0.1"

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
    SandboxApp *pSandbox = NULL;
	
	// This is the entry point for the DreamOS Engine
    DEBUG_LINEOUT("DREAM OS %s Starting ...", DREAM_OS_VERSION_STR);
    
    //CRM(InitializeOSXWindow(), "Failed to launch OSX Window");
	
	// Create the Sandbox
	//SandboxApp *pSandbox = SandboxFactory::MakeSandbox(SANDBOX_APP_WIN32);
    pSandbox = SandboxFactory::MakeSandbox(SANDBOX_APP_OSX);
    CNM(pSandbox, "Failed to create sandbox");
    CVM(pSandbox, "Sandbox is Invalid!");

    // This will start the application
    CRM(pSandbox->ShowSandbox(), "Failed to show sandbox window");
	
Error:
	//system("pause");
    DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", DREAM_OS_VERSION_STR, r);
	return (int)(r);
}
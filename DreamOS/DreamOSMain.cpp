//#include "DreamOSMain.h"
//#include "OSX/OSXApp.h"

#include <ctime>

#include "RESULT/EHM.h"

#include "Sandbox/SandboxFactory.h"

#define DREAM_OS_VERSION_STR "V0.1"

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
    SandboxApp *pSandbox = NULL;

	srand(static_cast <unsigned> (time(0)));
	
	// This is the entry point for the DreamOS Engine
    DEBUG_LINEOUT("DREAM OS %s Starting ...", DREAM_OS_VERSION_STR);
    
    //CRM(InitializeOSXWindow(), "Failed to launch OSX Window");
	
	// Create the Sandbox
    pSandbox = SandboxFactory::MakeSandbox(CORE_CONFIG_SANDBOX_PLATFORM);
    CNM(pSandbox, "Failed to create sandbox");
    CVM(pSandbox, "Sandbox is Invalid!");

    // This will start the application
    CRM(pSandbox->ShowSandbox(), "Failed to show sandbox window");
	
	DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", DREAM_OS_VERSION_STR, r);
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS %s Exiting with Error 0x%x result", DREAM_OS_VERSION_STR, r);
	system("pause");
	return (int)(r);
}
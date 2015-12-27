#include "DreamOSMain.h"
//#include "OSX/OSXApp.h"

#include "Sandbox/SandboxFactory.h"

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
	
	// This is the entry point for the DreamOS Engine
    DEBUG_LINEOUT("DREAM OS v0.1 Starting ...");
    
    //CRM(InitializeOSXWindow(), "Failed to launch OSX Window");
	
	// Create the Sandbox
	SandboxApp *pWin32Sandbox = SandboxFactory::MakeSandbox(SANDBOX_APP_WIN32);
	CNM(pWin32Sandbox, "Failed to create win32 sandbox");

	CRM(pWin32Sandbox->ShowSandbox(), "Failed to show Win32 Sandbox window");
	
Error:
	//system("pause");
	return (int)(r);
}
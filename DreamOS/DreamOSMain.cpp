#include "DreamOSMain.h"
#include "OSX/OSXApp.h"

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
	
	// This is the entry point for the DreamOS Engine
    DEBUG_LINEOUT("DREAM OS v0.1 Starting ...");
    
    CRM(InitializeOSXWindow(), "Failed to launch OSX Window");
	
Error:
	return (int)(r);
}
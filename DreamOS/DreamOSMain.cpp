#include "DreamOSMain.h"
#include "OSX/OSXApp.h"

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
	
	// This is the entry point for the DreamOS Engine
    printf("test\n");
    
    CRM(InitializeOSXWindow(), "Failed to launch OSX Window");
	
Error:
	return (int)(r);
}
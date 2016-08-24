#include "DreamOSMain.h"
//#include "OSX/OSXApp.h"

#include <ctime>
#include "RESULT/EHM.h"

// TODO: Eventually this should be a DreamGarage / Application specific main
// and should likely sit in the same dir as the DreamOS derivation
#include "../DreamGarage.h"

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
    
	DreamGarage dreamGarageApp;
	CRM(dreamGarageApp.Initialize(argc, argv), "Failed to initialize Dream Garage");
	
	// This is the entry point for the DreamOS Engine
	CRM(dreamGarageApp.Start(), "Failed to start Dream Garage");

//Success:
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
	system("pause");

	return (int)(r);
}
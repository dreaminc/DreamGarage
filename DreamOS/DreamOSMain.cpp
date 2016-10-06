#include "DreamOSMain.h"
//#include "OSX/OSXApp.h"

#include <ctime>
#include "RESULT/EHM.h"

// TODO: Eventually this should be a DreamGarage / Application specific main
// and should likely sit in the same dir as the DreamOS derivation
#include "../DreamGarage.h"
#include "../DreamTestApp.h"

// TODO: some other better way?
#define _USE_TEST_APP	

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
    
#ifdef _USER_TEST_APP
	DreamGarage dreamGarageApp;
	CRM(dreamGarageApp.Initialize(argc, argv), "Failed to initialize Dream Garage");
	CRM(dreamGarageApp.Start(), "Failed to start Dream Garage");	// This is the entry point for the DreamOS Engine
#else
	DreamTestApp dreamTestApp;
	CRM(dreamTestApp.Initialize(argc, argv), "Failed to initialize Dream Test App");
	CRM(dreamTestApp.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine
#endif

//Success:
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
	system("pause");

	return (int)(r);
}

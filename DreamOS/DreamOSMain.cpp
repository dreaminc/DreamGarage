#include "DreamOSMain.h"
//#include "OSX/OSXApp.h"

#include <ctime>
#include "RESULT/EHM.h"

// TODO: Eventually this should be a DreamGarage / Application specific main
// and should likely sit in the same dir as the DreamOS derivation
#include "DreamGarage/DreamGarage.h"
#include "../DreamTestApp/DreamTestApp.h"

#include "test/MatrixTestSuite.h"

#define ELPP_THREAD_SAFE 1
//#define ELPP_FORCE_USE_STD_THREAD 1
#define ELPP_NO_DEFAULT_LOG_FILE

#include "easylogging++.h"

#ifndef _EASY_LOGGINGPP_INITIALIZED
INITIALIZE_EASYLOGGINGPP
#define _EASY_LOGGINGPP_INITIALIZED
#endif

int main(int argc, const char *argv[]) {
	RESULT r = R_PASS;
    
#if defined(_UNIT_TESTING)
	// TODO: Replace this with a real unit testing framework in testing filter
	MatrixTestSuite matrixTestSuite;

	matrixTestSuite.Initialize();
	CRM(matrixTestSuite.RunTests(), "Failed to run matrix test suite tests");

	DEBUG_LINEOUT("Unit tests complete 0x%x result", r);
	system("pause");

	return (int)(r);
#elif defined(_USE_TEST_APP)
	DreamTestApp dreamTestApp;
	CRM(dreamTestApp.Initialize(argc, argv), "Failed to initialize Dream Test App");
	CRM(dreamTestApp.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine

//Success:
	DEBUG_LINEOUT("DREAM OS Exiting");
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
	system("pause");

	return (int)(r);

#else
	DreamGarage dreamGarageApp;
	CRM(dreamGarageApp.Initialize(argc, argv), "Failed to initialize Dream Garage");
	CRM(dreamGarageApp.Start(), "Failed to start Dream Garage");	// This is the entry point for the DreamOS Engine

//Success:
	DEBUG_LINEOUT("DREAM OS Exiting");
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
	DEBUG_SYSTEM_PAUSE();

	return (int)(r);
#endif
}

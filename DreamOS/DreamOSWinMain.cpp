#include "DreamOSMain.h"

#include <ctime>
#include "RESULT/EHM.h"

#include "DreamGarage/DreamGarage.h"
#include "../DreamTestApp/DreamTestApp.h"
#include "test/MatrixTestSuite.h"

#include <string>

// We use window subsystem in PRODUCTION build to allow Dream to run without a console window

#ifdef _WINDOWS
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	RESULT r = R_PASS;

	// get cmdln args and put them on the stack in argc,argv format
	LPWSTR *wargv;
	int argc;

	wargv = CommandLineToArgvW(GetCommandLine(), &argc);
	char** argv = new char*[argc];
	std::vector<std::string> args;

	for (int i = 0; i < argc; i++) {
		std::wstring warg(wargv[i]);
		args.push_back(std::string(warg.begin(), warg.end()));
	}

	for (int i = 0; i < argc; i++) {
		argv[i] = new char;
		argv[i] = (char*)args[i].c_str();
	}
	// now argc,argv are available and will get destroyed on exit

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
	CRM(dreamTestApp.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");
	CRM(dreamTestApp.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine
#else
	// This is the entry point for the DreamOS Engine
	DreamGarage dreamGarageApp;
	CRM(dreamGarageApp.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");
	CRM(dreamGarageApp.Start(), "Failed to start Dream Garage");	// This is the entry point for the DreamOS Engine
#endif

	//Success:
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
	DEBUG_SYSTEM_PAUSE();

	return (int)(r);
}
#endif
#include "DreamOSMain.h"

#include <ctime>
#include "RESULT/EHM.h"

#include "../DreamGarage.h"

#include <string>

// we use window subsystem in PRODUCTION build to allow Dream to run without a console window

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

	for (int i = 0; i < argc; i++)
	{
		argv[i] = new char;

		std::wstring warg(wargv[i]);
		args.push_back(std::string(warg.begin(), warg.end()));

		argv[i] = (char*)args[i].c_str();
	}
	// now argc,argv are available and will get destroyed on exit

	DreamGarage dreamGarageApp;

	CRM(dreamGarageApp.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");

	// This is the entry point for the DreamOS Engine
	CRM(dreamGarageApp.Start(), "Failed to start Dream Garage");

	//Success:
	return (int)(r);

Error:
	DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
	system("pause");

	return (int)(r);
}
#endif
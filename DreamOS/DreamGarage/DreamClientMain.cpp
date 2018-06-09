#include "DreamClientMain.h"

#include <ctime>
#include "RESULT/EHM.h"

#include "DreamGarage.h"

#include <string>

// We use window subsystem in PRODUCTION build to allow Dream to run without a console window

#ifdef _WIN32

	int WINAPI WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow)
	{
		RESULT r = R_PASS;

		// get cmdln args and put them on the stack in argc,argv format
		LPWSTR *wargv = nullptr;
		int argc = 0;

		wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
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

		// This is the entry point for the DreamOS Engine
		DreamGarage dreamGarageApp;
		CRM(dreamGarageApp.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");
		CRM(dreamGarageApp.Start(), "Failed to start Dream Garage");	// This is the entry point for the DreamOS Engine

	//Success:
		return (int)(r);

	Error:
		DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
		DEBUG_SYSTEM_PAUSE();

		return (int)(r);
	}
#else
	int main(int argc, const char *argv[]) {
		RESULT r = R_PASS;

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
	}
#endif
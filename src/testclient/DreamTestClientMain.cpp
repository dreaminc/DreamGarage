#include "DreamTestClientMain.h"

#include <ctime>
#include "RESULT/EHM.h"

#include "DreamTestClient.h"

#include <string>

#ifdef _WINDOWS
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
		RESULT r = R_PASS;

		// Get command line args and put them on the stack in argc,argv format
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

		DreamTestApp dreamTestApp;
		CRM(dreamTestApp.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");
		CRM(dreamTestApp.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine

	Success:
		DreamLogger::instance()->Flush();
		return (int)(r);

	Error:
		DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
		DreamLogger::instance()->Flush();
		DEBUG_SYSTEM_PAUSE();

		return (int)(r);
	}
#else
	int main(int argc, const char *argv[]) {
		RESULT r = R_PASS;

		DreamTestApp dreamTestApp;
		CRM(dreamTestApp.Initialize(argc, argv), "Failed to initialize Dream Test App");
		CRM(dreamTestApp.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine

	Success:
		DEBUG_LINEOUT("DREAM OS Exiting");
		DreamLogger::instance()->Flush();
		return (int)(r);

	Error:
		DEBUG_LINEOUT("DREAM Test App exiting with Error 0x%x result", r);
		DreamLogger::instance()->Flush();
		system("pause");

		return (int)(r);
	}
#endif
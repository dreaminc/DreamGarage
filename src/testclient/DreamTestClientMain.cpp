#include "DreamTestClientMain.h"

#include <ctime>

#include "DreamTestClient.h"

#include <string>

#ifdef _WINDOWS
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
		RESULT r = R_PASS;

		DreamTestClient dreamTestClient;

		// Get command line args and put them on the stack in argc,argv format
		LPWSTR *wargv = nullptr;
		int argc = 0;

		wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
		char** argv = new char*[argc];
		std::vector<std::string> args;

		// Set up a console and attach to it
		// TODO: Might want to explore this more
		if (AllocConsole()) {

			AttachConsole(GetCurrentProcessId());
			
			freopen("CONOUT$", "wt", stdout);
			freopen("CONOUT$", "wt", stderr);
			
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}
		else {
			CBM(false, "Failed to allocate console");
		}

		for (int i = 0; i < argc; i++) {
			std::wstring warg(wargv[i]);
			args.push_back(std::string(warg.begin(), warg.end()));
		}

		// now argc,argv are available and will get destroyed on exit
		for (int i = 0; i < argc; i++) {
			argv[i] = new char;
			argv[i] = (char*)args[i].c_str();
		}

		dreamTestClient = DreamTestClient();
		CRM(dreamTestClient.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");
		CRM(dreamTestClient.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine

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

		DreamTestClient DreamTestClient;
		CRM(DreamTestClient.Initialize(argc, argv), "Failed to initialize Dream Test App");
		CRM(DreamTestClient.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine

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
#include "Logger.h"

#include "launcher.h"

#include "ProcessExecutor.h"
#include "callback.h"
#include "registry.h"

#include "json.hpp"

#include <map>

#ifdef DEV_ENVIRONMENT
std::wstring	updatesUrl{ L"https://github.com/dreaminc/Dream/releases/download/DevReleases/" };
#else
std::wstring	updatesUrl{ L"https://github.com/dreaminc/Dream/releases/download/Releases/" };
#endif

#define ELPP_THREAD_SAFE 1
//#define ELPP_FORCE_USE_STD_THREAD 1
#define ELPP_NO_DEFAULT_LOG_FILE

#include "easylogging++.h"

#ifndef _EASY_LOGGINGPP_INITIALIZED
INITIALIZE_EASYLOGGINGPP
#define _EASY_LOGGINGPP_INITIALIZED
#endif

// the following methods need a bit of organizing into a class

bool CheckForUpdate(bool& updated) {
	SquirrelEvent res;

	std::wstring wstrLoadingGif;
	//loadingGif += L"--loadingGif=\"";
	//loadingGif += ProcessExecutor::GetProcessExecutor()->GetCurrentProcessDir();
	//loadingGif += L"loading.gif\"";

	if (!ProcessExecutor::instance()->Execute(L"Update.exe", L"--checkForUpdate=\"" + updatesUrl + L"\" " + wstrLoadingGif,
		ProcessExecutor::PROCESS_DIRECTORY_TYPE::PARENT,
		false,
		false,
		[&](const SquirrelEvent& event) -> bool {
			LOG(INFO) << "event " << event;

			res = event;
			return false;
		})
		)
	{
		LOG(ERROR) << "process execute failed";
		return false;
	}

	nlohmann::json releasesToApply = res.GetCallback().GetJson()["/releasesToApply"_json_pointer];

	LOG(INFO) << "check for updates returned " << releasesToApply;

	updated = releasesToApply.empty();

	if (updated)
		LOG(INFO) << "check for updates returned updated";
	else
		LOG(INFO) << "check for updates returned not updated";

	return true;
}

bool Update()
{
	SquirrelEvent res;

	std::wstring loadingGif;
	//loadingGif += L"--loadingGif=\"";
	//loadingGif += ProcessExecutor::GetProcessExecutor()->GetCurrentProcessDir();
	//loadingGif += L"loading.gif\"";


	if (!ProcessExecutor::instance()->Execute(L"Update.exe", L"--update=\"" + updatesUrl + L"\" " + loadingGif,
		ProcessExecutor::PROCESS_DIRECTORY_TYPE::PARENT,
		false,
		true,
		[&](const SquirrelEvent& event) -> bool
	{
		LOG(INFO) << "event " << event;

		res = event;
		return false;
	}
	))
	{
		LOG(ERROR) << "process execute failed";
		return false;
	}

	LOG(INFO) << "update downloaded";

	return true;
}

bool ExecuteUpdate(int argc, char *argv[])
{
	SquirrelEvent res;

	if (argc <= 1)
	{
		if (!ProcessExecutor::instance()->Execute(L"Update.exe", L"--processStart \"DreamLauncher.exe\"",
			ProcessExecutor::PROCESS_DIRECTORY_TYPE::PARENT,
			false,
			false))
		{
			LOG(ERROR) << "process execute failed";
			return false;
		}
	}
	else
	{
		std::string cmdlnArgs;

		for (int i = 2; i <= argc; i++)
		{
			cmdlnArgs += std::string(argv[i - 1]) + " ";
		}

		std::wstring wcmdlnArgs(cmdlnArgs.begin(), cmdlnArgs.end() - 1);

		std::wstring exe(L"Update.exe");
		std::wstring args(L"--processStart \"DreamLauncher.exe\" --process-start-args \"");
		args += wcmdlnArgs;
		args += L"\"";

		if (!ProcessExecutor::instance()->Execute(exe.c_str(),
			args.c_str(),
			ProcessExecutor::PROCESS_DIRECTORY_TYPE::PARENT,
			false,
			false))
		{
			LOG(ERROR) << "process execute failed";
			return false;
		}
	}

	LOG(INFO) << "executed updated version completed";

	return true;
}

/*
// From Squirrel.Windows docs: https://github.com/Squirrel/Squirrel.Windows/blob/master/docs/using/custom-squirrel-events-non-cs.md

--squirrel-install x.y.z.m - called when your app is installed. Exit as soon as you're finished setting up the app
--squirrel-firstrun - called after everything is set up. You should treat this like a normal app run (maybe show the "Welcome" screen)
--squirrel-updated x.y.z.m - called when your app is updated to the given version. Exit as soon as you're finished.
--squirrel-obsolete x.y.z.m - called when your out-of-date app is no longer the newest version. Exit as soon as you're finished.
--squirrel-uninstall x.y.z.m - called when your app is uninstalled. Exit as soon as you're finished.
*/

enum class CmdEventType
{
	// not an event
	None,

	// squirrel.windows events
	Install,
	FirstRun,
	Updated,
	Obsolete,
	Uninstall,

	// custom events
	Registry
};

std::map<std::string, CmdEventType> squirrelCmdlnEventDictionary
{
	{ "--squirrel-install",		CmdEventType::Install },
	{ "--squirrel-firstrun",	CmdEventType::FirstRun },
	{ "--squirrel-updated",		CmdEventType::Updated },
	{ "--squirrel-obsolete",	CmdEventType::Obsolete },
	{ "--squirrel-uninstall",	CmdEventType::Uninstall },
	{ "--registry",				CmdEventType::Registry },
};

CmdEventType CheckCmdlnEvent(int argc, char *argv[])
{
	if (argc < 2)
		return CmdEventType::None;

	std::string arg(argv[1]);

	if (squirrelCmdlnEventDictionary.find(arg) != squirrelCmdlnEventDictionary.end())
	{
		return squirrelCmdlnEventDictionary[arg];
	}

	return CmdEventType::None;
}

// checks for dev releases
bool CheckAccess(int argc, char *argv[])
{
	if (!CheckDev())
	{
		LOG(ERROR) << "access denied";
		MessageBox(NULL, L"Access denied. contact www.dreamos.com for dev access.\n", L"Dream Message", MB_OK);
		return false;
	}

	LOG(INFO) << "dev access succeded";
	return true;
}

bool InstallShortcuts()
{
	// installing shortcuts only in dev releases
#ifdef DEV_ENVIRONMENT
	std::wstring exe(L"Update.exe");
	std::wstring args(L"--createShortcut=\"Dream.html\" --icon=\"");
	args += ProcessExecutor::instance()->GetCurrentProcessDir();
	args += L"Dream.ico\"";

	if (!ProcessExecutor::instance()->Execute(exe.c_str(),
		args.c_str(),
		ProcessExecutor::PROCESS_DIRECTORY_TYPE::PARENT,
		false,
		false))
	{
		LOG(ERROR) << "process execute failed";
		return false;
	}
#else
#endif // DEV_ENVIRONMENT
	return true;
}

bool RemoveShortcuts()
{
#ifdef DEV_ENVIRONMENT
	std::wstring exe(L"Update.exe");
	std::wstring args(L"--removeShortcut=\"Dream.html\" --icon=\"");
	args += ProcessExecutor::instance()->GetCurrentProcessDir();
	args += L"Dream.ico\"";

	if (!ProcessExecutor::instance()->Execute(exe.c_str(),
		args.c_str(),
		ProcessExecutor::PROCESS_DIRECTORY_TYPE::PARENT,
		false,
		false))
	{
		LOG(ERROR) << "process execute failed";
		return false;
	}
#else
#endif // DEV_ENVIRONMENT
	return true;
}

bool InstallRegistry()
{
	if (!CheckRegistryVars())
	{
		if (InstallRegistryVars())
		{
			LOG(INFO) << "InstallRegistryVars ok";
		}
		else
		{
			LOG(INFO) << "InstallRegistryVars failed";
		}

		if (!CheckRegistryVars())
		{
			LOG(ERROR) << "registry mismatch - install vars";

			std::wstring exe(L"DreamLauncher.exe");
			std::wstring args(L"--registry");

			if (!ProcessExecutor::instance()->Execute(exe.c_str(),
				args.c_str(),
				ProcessExecutor::PROCESS_DIRECTORY_TYPE::CURRENT,
				true,
				true))
			{
				LOG(ERROR) << "process execute failed " << exe;
				return false;
			}

			LOG(INFO) << "process execute completed as admin " << exe;

			if (!CheckRegistryVars())
			{
				LOG(ERROR) << "registry still mismatching. exit";
				return false;
			}
		}
	}

	LOG(INFO) << "install registry ok";

	return true;
}

bool RunDream(int argc, char *argv[])
{	
	std::wstring wcmdlnArgs;

	if (argc > 1)
	{
		std::string cmdlnArgs;

		for (int i = 2; i <= argc; i++)
		{
			cmdlnArgs += std::string(argv[i - 1]) + " ";
		}

		wcmdlnArgs = std::wstring(cmdlnArgs.begin(), cmdlnArgs.end() - 1);
	}

	if (!ProcessExecutor::instance()->Execute(L"DreamOS.exe",
		wcmdlnArgs.c_str(),
		ProcessExecutor::PROCESS_DIRECTORY_TYPE::CURRENT,
		false,
		false))
	{
		LOG(ERROR) << "process execute failed";
		return false;
	}

	return true;
}

int main(int argc, char *argv[], WindowController* pSplashWindow)
{
	Logger::InitializeLogger();

	if (!ProcessExecutor::instance()->Initialize())
	{
		LOG(ERROR) << "process executor init failed";
		return -1;
	}

#ifdef DEV_ENVIRONMENT
	LOG(INFO) << "dev environment";

	if (!CheckAccess(argc, argv))
	{
		LOG(ERROR) << "process executor init failed";
		return -1;
	}
#else
	LOG(INFO) << "production environment";
#endif

	LOG(INFO) << "access ok";

	auto squirrelCmdlnEvent = CheckCmdlnEvent(argc, argv);

	if (squirrelCmdlnEvent != CmdEventType::None)
	{
		if (squirrelCmdlnEvent == CmdEventType::FirstRun)
		{
			if (!InstallRegistry())
			{
				LOG(ERROR) << "process registry failed";
				return -1;
			}

			InstallShortcuts();

#ifdef DEV_ENVIRONMENT
			// open in external browser (for now used as an indication updated completed)
			//ShellExecute(0, 0, L"https://www.develop.dreamos.com/", 0, 0, SW_SHOW);
			if (!ProcessExecutor::instance()->Execute(L"Dream.html",
				L"",
				ProcessExecutor::PROCESS_DIRECTORY_TYPE::CURRENT,
				false,
				false))
			{
				LOG(ERROR) << "error loading Dream.html";
			}
#endif
		}
		else if (squirrelCmdlnEvent == CmdEventType::Updated)
		{
			InstallShortcuts();
		}
		else if (squirrelCmdlnEvent == CmdEventType::Registry)
		{
			InstallRegistryVars();
		}
		else if (squirrelCmdlnEvent == CmdEventType::Install)
		{

		}
		else if (squirrelCmdlnEvent == CmdEventType::Uninstall)
		{
			if (!RemoveShortcuts())
			{
				LOG(ERROR) << "RemoveShortcuts failed";
			}
		}

		LOG(INFO) << "squirrel updated event OK -> exit process";

		return 0;
	}

	if (!InstallRegistry())
	{
		LOG(ERROR) << "process registry failed";
		return -1;
	}

	bool updated = false;

	if (!CheckForUpdate(updated))
	{
		LOG(ERROR) << "failed to check for update";
		return -1;
	}
	
	if (!updated)
	{
		LOG(INFO) << "updating...";

		if (!Update())
		{
			LOG(ERROR) << "failed to update";
			return -1;
		}

		LOG(INFO) << "updated completed, running...";
		
		if (!ExecuteUpdate(argc, argv))
		{
			LOG(ERROR) << "failed to execute update";
			return -1;
		}

		LOG(INFO) << "running completed -> exit";

		return 0;
	}

	LOG(INFO) << "updated - running Dream...";

	if (!RunDream(argc, argv))
	{
		LOG(ERROR) << "running Dream failed";
		return false;
	}

	LOG(INFO) << "running Dream -> wait for signal";

	HANDLE hCloseSplashScreenEvent = CreateEvent(NULL,        // no security
		TRUE,       // manual-reset event
		FALSE,      // not signaled
		(LPTSTR)L"CloseSplashScreenEvent"); // event name

	DWORD res = WaitForSingleObject(hCloseSplashScreenEvent, 50000);

	switch (res)
	{
	case WAIT_ABANDONED:
	case WAIT_FAILED:
	case WAIT_TIMEOUT:{
		LOG(INFO) << "waiting for Dream failed = " << res;
	}break;
	case WAIT_OBJECT_0: {
		LOG(INFO) << "waiting for Dream ok";
	}break;
	}

	CloseHandle(hCloseSplashScreenEvent);

	LOG(INFO) << "running Dream done -> exit";

	return 0;
}

int run(WindowController* pSplashWindow)
{
	// get cmdln args and put them on the stack in argc,argv format
	LPWSTR *wargv;
	int argc;

	wargv = CommandLineToArgvW(GetCommandLine(), &argc);

	char** argv = new char*[argc];

	std::vector<std::string> args;

	for (int i = 0; i < argc; i++)
	{
		std::wstring warg(wargv[i]);
		args.push_back(std::string(warg.begin(), warg.end()));
	}

	for (int i = 0; i < argc; i++)
	{
		argv[i] = new char;
		argv[i] = (char*)args[i].c_str();
	}
	// now argc,argv are available and will get destroyed on exit

	return main(argc, argv, pSplashWindow);
}

void launcher::EntryPoint(WindowController* pSplashWindow)
{
	int ret = run(pSplashWindow);

	if (ret != 0)
	{
		MessageBox(NULL, L"Dream failed to load :( Please try to reinstall Dream client.\n", L"Dream Message", MB_OK);
	}

	exit(ret);
}

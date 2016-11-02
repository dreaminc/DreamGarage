#include "Logger.h"

#include "launcher.h"

#include "ProcessExecutor.h"
#include "callback.h"
#include "registry.h"

#include "json.hpp"

#include <map>

const std::wstring	updatesUrl{L"https://github.com/dreaminc/Dream/releases/download/Releases/"};

// the following methods need a bit of organizing into a class

bool CheckForUpdate(bool& updated)
{
	SquirrelEvent res;

	std::wstring loadingGif;
	//loadingGif += L"--loadingGif=\"";
	//loadingGif += ProcessExecutor::GetProcessExecutor()->GetCurrentProcessDir();
	//loadingGif += L"loading.gif\"";

	if (!ProcessExecutor::GetProcessExecutor()->Execute(L"Update.exe", L"--checkForUpdate=\"" + updatesUrl + L"\" " + loadingGif,
		ProcessExecutor::ProcessDir::ParentDir,
		false,
		false,
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


	if (!ProcessExecutor::GetProcessExecutor()->Execute(L"Update.exe", L"--update=\"" + updatesUrl + L"\" " + loadingGif,
		ProcessExecutor::ProcessDir::ParentDir,
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
		if (!ProcessExecutor::GetProcessExecutor()->Execute(L"Update.exe", L"--processStart \"DreamLauncher.exe\"",
			ProcessExecutor::ProcessDir::ParentDir,
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

		if (!ProcessExecutor::GetProcessExecutor()->Execute(exe.c_str(),
			args.c_str(),
			ProcessExecutor::ProcessDir::ParentDir,
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

CmdEventType CheckSquirrelCmdlnEvent(int argc, char *argv[])
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

bool InstallShortcuts()
{
	std::wstring exe(L"Update.exe");
	std::wstring args(L"--createShortcut=\"Dream.html\" --icon=\"");
	args += ProcessExecutor::GetProcessExecutor()->GetCurrentProcessDir();
	args += L"Dream.ico\"";

	if (!ProcessExecutor::GetProcessExecutor()->Execute(exe.c_str(),
		args.c_str(),
		ProcessExecutor::ProcessDir::ParentDir,
		false,
		false))
	{
		LOG(ERROR) << "process execute failed";
		return false;
	}

	return true;
}

bool InstallRegistry()
{
	if (!CheckRegistryVars())
	{
		LOG(ERROR) << "registry mismatch - install vars";

		std::wstring exe(L"DreamLauncher.exe");
		std::wstring args(L"--registry");

		if (!ProcessExecutor::GetProcessExecutor()->Execute(exe.c_str(),
			args.c_str(),
			ProcessExecutor::ProcessDir::CurrentDir,
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

	if (!ProcessExecutor::GetProcessExecutor()->Execute(L"DreamOS.exe",
		wcmdlnArgs.c_str(),
		ProcessExecutor::ProcessDir::CurrentDir,
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

	if (!ProcessExecutor::GetProcessExecutor()->Init())
	{
		LOG(ERROR) << "process executor init failed";
		return -1;
	}

	auto squirrelCmdlnEvent = CheckSquirrelCmdlnEvent(argc, argv);

	if (squirrelCmdlnEvent != CmdEventType::None)
	{
		if (squirrelCmdlnEvent == CmdEventType::FirstRun)
		{
			InstallShortcuts();

			// open in external browser (for now used as an indication updated completed)
			//ShellExecute(0, 0, L"https://www.develop.dreamos.com/", 0, 0, SW_SHOW);
			if (!ProcessExecutor::GetProcessExecutor()->Execute(L"Dream.html",
				L"",
				ProcessExecutor::ProcessDir::CurrentDir,
				false,
				false))
			{
				LOG(ERROR) << "error loading Dream.html";
			}

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
			if (!InstallRegistry())
			{
				LOG(ERROR) << "process registry failed";
				return -1;
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
	exit(ret);
}

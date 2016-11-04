#include "ProcessExecutor.h"

#include "windows.h"

bool ProcessExecutor::Init()
{
	TCHAR path[MAX_PATH];

	int bytes = GetModuleFileName(NULL, path, MAX_PATH);

	if (bytes == 0)
	{
		LOG(ERROR) << "failed to GetModuleFileName";
		return false;
	}

	m_currentProcessDir = std::wstring(path);
	
	m_currentProcessDir = m_currentProcessDir.substr(0, m_currentProcessDir.find_last_of(L"\\/"));
	
	m_parentDir = m_currentProcessDir.substr(0, m_currentProcessDir.find_last_of(L"\\/"));
	
	m_currentProcessDir += L"\\";
	m_parentDir += L"\\";

	LOG(INFO) << "current dir = " << m_currentProcessDir;
	LOG(INFO) << "parent dir (update.exe) = " << m_parentDir;

	return true;
}

bool ProcessExecutor::ExecuteProcess(const std::wstring& processFullPath, const std::wstring& args, bool runAsAdmin, bool wait)
{
	LOG(INFO) << "Executing " << processFullPath << " " << args << " ...";

	if (!wait)
	{
		HINSTANCE hInst = ShellExecute(NULL, (!runAsAdmin) ? NULL : L"runas", processFullPath.c_str(), args.c_str(), 0, SW_SHOWNORMAL);
		return (hInst != NULL);
	}
	else
	{
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = (!runAsAdmin) ? NULL : L"runas";
		ShExecInfo.lpFile = processFullPath.c_str();
		ShExecInfo.lpParameters = args.c_str();
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOWNORMAL;
		ShExecInfo.hInstApp = NULL;
		if (ShellExecuteEx(&ShExecInfo) == TRUE)
		{	
			DWORD res = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

			switch (res)
			{
			case WAIT_ABANDONED:
			case WAIT_FAILED:
			case WAIT_TIMEOUT: {
				LOG(INFO) << "create process failed = " << res;
				return false;
			}break;
			case WAIT_OBJECT_0: {
				LOG(INFO) << "create process ok";
				return true;
			}break;
			}

			return false;
		}
		else
			return false;
	}
}

bool ProcessExecutor::Execute(const std::wstring& processPath, const std::wstring& args, ProcessDir processDir, bool runAsAdmin, bool wait, const std::function<bool(const std::string&)> callback)
{
	std::wstring fullPath;

	switch (processDir)
	{
		case ProcessDir::FullPath: break;
		case ProcessDir::CurrentDir: {
			fullPath = m_currentProcessDir + processPath;
		} break;
		case ProcessDir::ParentDir: {
			fullPath = m_parentDir + processPath;
		} break;
	}

	LOG(INFO) << "Executing " << fullPath << " " << args << " ...";

	bool	success = true;
	bool	exeSuccess = false;

	if (!callback)
	{
		exeSuccess = ExecuteProcess(fullPath, args, runAsAdmin, wait);
	}
	else
	{		
		remove("callback.0");
	
		// TODO: this should be written cleanly to run a process with stream out
		{
			std::wstring wcmd(fullPath + L" " + args + L" >> callback.0");

			LOG(INFO) << "run " << wcmd;

			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE;

			HANDLE h = CreateFile(L"callback.0",
				FILE_APPEND_DATA,
				FILE_SHARE_WRITE | FILE_SHARE_READ,
				&sa,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			BOOL ret = FALSE;
			DWORD flags = CREATE_NO_WINDOW;

			ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags |= STARTF_USESTDHANDLES;
			si.hStdInput = NULL;
			si.hStdError = h;
			si.hStdOutput = h;

			ret = CreateProcess(NULL, (wchar_t*)wcmd.c_str(), NULL, NULL, TRUE, flags, NULL, NULL, &si, &pi);

			if (ret)
			{
				DWORD res = WaitForSingleObject(pi.hProcess, INFINITE);

				exeSuccess = false;

				switch (res)
				{
				case WAIT_ABANDONED:
				case WAIT_FAILED:
				case WAIT_TIMEOUT: {
					LOG(INFO) << "create process failed = " << res;
					exeSuccess = false;
				}break;
				case WAIT_OBJECT_0: {
					LOG(INFO) << "create process ok";
					exeSuccess = true;
				}break;
				}
			}
			else
				exeSuccess = false;

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			CloseHandle(h);
		}

		/*
		if (!runAsAdmin)
			res = ShellExecute(NULL, NULL, fullPath.c_str(), (args + L" >> callback.0").c_str(), 0, SW_SHOWNORMAL);
		else
			res = ShellExecute(NULL, L"runas", fullPath.c_str(), (args + L" >> callback.0").c_str(), 0, SW_SHOWNORMAL);

		//res = _wsystem((fullPath + L" >> callback.0").c_str());
		*/

		std::ifstream cbFile("callback.0", std::ios::binary);
		std::string line;

		while (std::getline(cbFile, line))
		{
			LOG(INFO) << "callback " << line;
			if (callback(line))
			{
				success = false;
				break;
			}
		}

		cbFile.close();
		remove("callback.0");
	}

	if (exeSuccess)
		LOG(INFO) << "Executed OK " << fullPath << " " << args;
	else
		LOG(ERROR) << "Executed failed " << fullPath << " " << args;

	if (!success)
		LOG(INFO) << "Executed callback failed";

	return (success && exeSuccess);
}

bool ProcessExecutor::Execute(const std::wstring& processPath, const std::wstring& args, ProcessDir processDir, bool runAsAdmin, bool wait, const std::function<bool(const SquirrelEvent&)> squirrelCallback)
{
	SquirrelEvent event;

	return Execute(processPath,
		args,
		processDir,
		runAsAdmin,
		wait,
		[&](const std::string& callback) -> bool
	{
		auto newState = event.OnCallback(callback);
		bool res = squirrelCallback(event);
		return (newState == SquirrelEvent::State::Invalid || res);
	});
}

const std::wstring& ProcessExecutor::GetCurrentProcessDir() const
{
	return m_currentProcessDir;
}

const std::wstring& ProcessExecutor::GetParentDir() const
{
	return m_parentDir;
}
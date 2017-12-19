#include "ProcessExecutor.h"

#include "windows.h"

// Instantiate the static singleton instance
ProcessExecutor* ProcessExecutor::s_pProcessExecutor = nullptr;

bool ProcessExecutor::Initialize() {
	TCHAR path[MAX_PATH];

	int bytes = GetModuleFileName(NULL, path, MAX_PATH);

	if (bytes == 0) {
		LOG(ERROR) << "failed to GetModuleFileName";
		return false;
	}

	m_strCurrentProcessDir = std::wstring(path);

	m_strCurrentProcessDir = m_strCurrentProcessDir.substr(0, m_strCurrentProcessDir.find_last_of(L"\\/"));
	
	m_parentDir = m_strCurrentProcessDir.substr(0, m_strCurrentProcessDir.find_last_of(L"\\/"));
	
	m_strCurrentProcessDir += L"\\";
	m_parentDir += L"\\";

	LOG(INFO) << "current dir = " << m_strCurrentProcessDir;
	LOG(INFO) << "parent dir (update.exe) = " << m_parentDir;

	return true;
}

bool ProcessExecutor::ExecuteProcess(const std::wstring& wstrProcessFullPath, const std::wstring& wstrArgs, bool fRunAsAdmin, bool fWait) {
	LOG(INFO) << "Executing " << wstrProcessFullPath << " " << wstrArgs << " ...";

	if (!fWait) {
		HINSTANCE hInst = ShellExecute(NULL, (!fRunAsAdmin) ? NULL : L"runas", wstrProcessFullPath.c_str(), wstrArgs.c_str(), 0, SW_SHOWNORMAL);
		return (hInst != NULL);
	}
	else {
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = nullptr;
		ShExecInfo.lpVerb = (!fRunAsAdmin) ? NULL : L"runas";
		ShExecInfo.lpFile = wstrProcessFullPath.c_str();
		ShExecInfo.lpParameters = wstrArgs.c_str();
		ShExecInfo.lpDirectory = nullptr;
		ShExecInfo.nShow = SW_SHOWNORMAL;
		ShExecInfo.hInstApp = nullptr;

		if (ShellExecuteEx(&ShExecInfo) == true) {	

			DWORD res = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

			switch (res) {
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
		else {
			return false;
		}
	}
}

bool ProcessExecutor::Execute(const std::wstring& wstrProcessPath, const std::wstring& wstrArgs, PROCESS_DIRECTORY_TYPE processDir, bool runAsAdmin, bool wait, const std::function<bool(const std::string&)> callback) {
	std::wstring fullPath;

	switch (processDir) {
		case PROCESS_DIRECTORY_TYPE::FULL: break;

		case PROCESS_DIRECTORY_TYPE::CURRENT: {
			fullPath = m_strCurrentProcessDir + wstrProcessPath;
		} break;

		case PROCESS_DIRECTORY_TYPE::PARENT: {
			fullPath = m_parentDir + wstrProcessPath;
		} break;
	}

	LOG(INFO) << "Executing " << fullPath << " " << wstrArgs << " ...";

	bool fSuccess = true;
	bool fExecutedSuccessfully  = false;

	if (!callback) {
		fExecutedSuccessfully  = ExecuteProcess(fullPath, wstrArgs, runAsAdmin, wait);
	}
	else
	{		
		remove("callback.0");
	
		// TODO: this should be written cleanly to run a process with stream out
		{
			std::wstring wcmd(fullPath + L" " + wstrArgs + L" >> callback.0");

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

				fExecutedSuccessfully  = false;

				switch (res)
				{
				case WAIT_ABANDONED:
				case WAIT_FAILED:
				case WAIT_TIMEOUT: {
					LOG(INFO) << "create process failed = " << res;
					fExecutedSuccessfully  = false;
				}break;
				case WAIT_OBJECT_0: {
					LOG(INFO) << "create process ok";
					fExecutedSuccessfully  = true;
				}break;
				}
			}
			else
				fExecutedSuccessfully  = false;

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

		while (std::getline(cbFile, line)) {
			LOG(INFO) << "callback " << line;

			if (callback(line)) {
				fSuccess = false;
				break;
			}
		}

		cbFile.close();
		remove("callback.0");
	}

	if (fExecutedSuccessfully ) {
		LOG(INFO) << "Executed OK " << fullPath << " " << wstrArgs;
	}
	else {
		LOG(ERROR) << "Executed failed " << fullPath << " " << wstrArgs;
	}

	if (!fSuccess) {
		LOG(INFO) << "Executed callback failed";
	}

	return (fSuccess && fExecutedSuccessfully );
}

bool ProcessExecutor::Execute(const std::wstring& processPath, const std::wstring& args, PROCESS_DIRECTORY_TYPE processDir, bool runAsAdmin, bool wait, const std::function<bool(const SquirrelEvent&)> squirrelCallback) {
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

const std::wstring& ProcessExecutor::GetCurrentProcessDir() const {
	return m_strCurrentProcessDir;
}

const std::wstring& ProcessExecutor::GetParentDir() const {
	return m_parentDir;
}
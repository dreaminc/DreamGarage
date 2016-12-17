#pragma once

#include "Logger.h"
#include "callback.h"

#include <string>

class ProcessExecutor
{
public:
	enum class ProcessDir
	{
		FullPath,
		CurrentDir,
		ParentDir
	};

	static ProcessExecutor* GetProcessExecutor()
	{
		static ProcessExecutor pe;
		return &pe;
	}

	// initialize working path, returns bool when failed
	bool Init();

	bool Execute(const std::wstring& processPath, const std::wstring& args, ProcessDir processDir, bool runAsAdmin, bool wait, const std::function<bool(const std::string&)> callback = nullptr);
	bool Execute(const std::wstring& processPath, const std::wstring& args, ProcessDir processDir, bool runAsAdmin, bool wait, const std::function<bool(const SquirrelEvent&)> callback);

	const std::wstring& GetCurrentProcessDir() const;
	const std::wstring& GetParentDir() const;

private:
	bool ExecuteProcess(const std::wstring& processFullPath, const std::wstring& args, bool runAsAdmin, bool wait);

private:

	// exe tree :
	// %ProgramData%/../Dream
	//		Update.exe
	// %ProgramData%/../Dream/version
	//		DreamOS.exe
	//		DreamLauncher.exe

	std::wstring m_currentProcessDir;
	std::wstring m_parentDir;
};

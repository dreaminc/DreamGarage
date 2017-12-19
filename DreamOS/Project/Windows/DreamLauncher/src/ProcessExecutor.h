#pragma once

#include "Logger.h"
#include "callback.h"

#include <string>

class ProcessExecutor {
public:
	enum class PROCESS_DIRECTORY_TYPE {
		FULL,
		CURRENT,
		PARENT
	};

	// initialize working path, returns bool when failed
	bool Initialize();

	bool Execute(const std::wstring& wstrProccessPath, const std::wstring& wstrArgs, PROCESS_DIRECTORY_TYPE processDir, bool runAsAdmin, bool wait, const std::function<bool(const std::string&)> callback = nullptr);
	bool Execute(const std::wstring& wstrProccessPath, const std::wstring& wstrArgs, PROCESS_DIRECTORY_TYPE processDir, bool runAsAdmin, bool wait, const std::function<bool(const SquirrelEvent&)> callback);

	const std::wstring& GetCurrentProcessDir() const;
	const std::wstring& GetParentDir() const;

private:
	bool ExecuteProcess(const std::wstring& wstrProcessFullPath, const std::wstring& strArgs, bool fRunAsAdmin, bool fWait);

private:

	// exe tree :
	// %ProgramData%/../Dream
	//		Update.exe
	// %ProgramData%/../Dream/version
	//		DreamOS.exe
	//		DreamLauncher.exe

	std::wstring m_strCurrentProcessDir;
	std::wstring m_parentDir;

	// Singleton

public:
	static ProcessExecutor* instance() {
		if (s_pProcessExecutor == nullptr) {
			s_pProcessExecutor = new ProcessExecutor();
			
			//s_pProcessExecutor->Initialize();
		}

		return s_pProcessExecutor;
	}

private:
	static ProcessExecutor *s_pProcessExecutor;
};

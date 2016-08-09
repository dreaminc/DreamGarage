#ifndef COMMAND_LINE_MANAGER_H_
#define COMMAND_LINE_MANAGER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Sandbox/CommandLineManager.h
// Dream OS Command Line Manager - simple interface to encapsulate command line arguments

#include <list>
#include <map>
#include <vector>

class CommandLineManager : public valid {
private:
	CommandLineManager();
	~CommandLineManager();

public:
	RESULT InitializeFromCommandLine(int argc, const char *argv[]);
	std::string GetCommandLineArgument(int num);

	int GetNumCommandLineArguments();
	bool CommandLineArgumentFound(std::string strCommandLineArgument);

private:
	std::vector<std::string> m_strCommandLineArguments;
	UID m_uid;

	// TODO: Replace with Singleton pattern / manager
	// Singleton Usage
protected:
	static CommandLineManager *s_pInstance;

public:
	static CommandLineManager *instance() {
		if (!s_pInstance)
			s_pInstance = new CommandLineManager();
		
		return s_pInstance;
	}
};

#endif // ! COMMAND_LINE_MANAGER_H_
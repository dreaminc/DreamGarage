#ifndef COMMAND_LINE_MANAGER_H_
#define COMMAND_LINE_MANAGER_H_

#include "core/ehm/EHM.h"


// DREAM OS
// DreamOS/Sandbox/CommandLineManager.h
// Dream OS Command Line Manager - simple interface to encapsulate command line arguments

#include <list>
#include <map>
#include <vector>
#include <functional>

#include "core/types/DObject.h"

class CommandLineManager : public DObject {
private:
	CommandLineManager();
	~CommandLineManager();

public:
	RESULT InitializeFromCommandLine(int argc, const char *argv[]);
	std::string GetCommandLineArgument(int num);

	int GetNumCommandLineArguments();
	bool CommandLineArgumentFound(std::string strCommandLineArgument);

	struct RegisteredParameter {
		std::string m_strParamName;
		std::string m_strParamTag;
		std::string m_strDefaultValue;

		bool m_fValueSet;
		std::string m_strParamValue;
		bool m_fEnabled;
	};

	RESULT EnableParameter(std::string strParamName);
	RESULT DisableParameter(std::string strParamName);
	RESULT RegisterParameter(std::string strParamName, std::string strParamTag, std::string strDefaultValue);
	RESULT SetParameterValue(std::string strParamName, std::string strParamValue);
	std::string GetParameterValue(std::string strParamName);
	std::vector<std::string> GetParameterValues(std::string strParamName, char cDelim = '.');

	void ForEach(std::function<void(const std::string&)> func);

private:
	std::vector<std::string> m_strCommandLineArguments;
	
	std::map<std::string, RegisteredParameter> m_mapRegisteredParams;	// This holds the registered params

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
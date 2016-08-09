#include "CommandLineManager.h"

// Initialize and allocate the instance
CommandLineManager* CommandLineManager::s_pInstance = nullptr;

CommandLineManager::CommandLineManager() {
	Validate();
	return;
}

CommandLineManager::~CommandLineManager() {
	// empty
}


RESULT CommandLineManager::InitializeFromCommandLine(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	for (int i = 0; i < argc; i++) {
		m_strCommandLineArguments.push_back(std::string(argv[i]));
	}

Error:
	return r;
}

std::string CommandLineManager::GetCommandLineArgument(int num) {
	std::string strRet;

	if (num < m_strCommandLineArguments.size()) {
		strRet = m_strCommandLineArguments[num];
	}

	return strRet;
}

int CommandLineManager::GetNumCommandLineArguments() {
	return m_strCommandLineArguments.size();
}

bool CommandLineManager::CommandLineArgumentFound(std::string strCommandLineArgument) {
	return (std::find(m_strCommandLineArguments.begin(), m_strCommandLineArguments.end(), strCommandLineArgument) != m_strCommandLineArguments.end());
}
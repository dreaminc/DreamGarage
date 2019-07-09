#include "CommandLineManager.h"

// Initialize and allocate the instance
CommandLineManager* CommandLineManager::s_pInstance = nullptr;

CommandLineManager::CommandLineManager() {
	m_strCommandLineArguments.clear();
	m_mapRegisteredParams.clear();

	Validate();
	return;
}

CommandLineManager::~CommandLineManager() {
	m_strCommandLineArguments.clear();
	m_mapRegisteredParams.clear();
}


RESULT CommandLineManager::InitializeFromCommandLine(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	DEBUG_OUT("CMD LINE MGR: ");

	// This will retain ordering
	for (int i = 0; i < argc; i++) {
		m_strCommandLineArguments.push_back(std::string(argv[i]));
		DEBUG_OUT("%s ", argv[i]);
	}

	DEBUG_LINEOUT(" ");

	//for (auto &strArgument : m_strCommandLineArguments) {
	for (auto it = m_strCommandLineArguments.begin(); it != m_strCommandLineArguments.end(); it++) {
		// Check to see if any of these are registered 
		std::string strArgument = (*it);

		if (strArgument[0] == '-') {
			std::string strTag = strArgument.substr(1);

			for (auto &regParam : m_mapRegisteredParams) {
				if (regParam.second.m_strParamTag == strTag) {
					std::string strParamName = regParam.first;
					std::string strParamValue = (*(++it));
					CRM(SetParameterValue(strParamName, strParamValue), "Failed to set param %s", strParamName.c_str());
					break;
				}
			}
		}
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
	return (int)(m_strCommandLineArguments.size());
}

bool CommandLineManager::CommandLineArgumentFound(std::string strCommandLineArgument) {
	return (std::find(m_strCommandLineArguments.begin(), m_strCommandLineArguments.end(), strCommandLineArgument) != m_strCommandLineArguments.end());
}

RESULT CommandLineManager::RegisterParameter(std::string strParamName, std::string strParamTag, std::string strDefaultValue) {
	RESULT r = R_PASS;
	RegisteredParameter regParam;

	CBM((m_mapRegisteredParams.find(strParamName) == m_mapRegisteredParams.end()), 
		"Command Line Manager %s already registered", strParamName.c_str());

	regParam.m_strParamName = strParamName;
	regParam.m_strParamTag = strParamTag;
	regParam.m_strDefaultValue = strDefaultValue;
	regParam.m_fValueSet = false;
	regParam.m_strParamValue = "";
	regParam.m_fEnabled = true;

	m_mapRegisteredParams[strParamName] = regParam;

Error:
	return r;
}

RESULT CommandLineManager::SetParameterValue(std::string strParamName, std::string strParamValue) {
	RESULT r = R_PASS;

	CBM((m_mapRegisteredParams.find(strParamName) != m_mapRegisteredParams.end()),
		"Param %s not registered", strParamName.c_str());

	m_mapRegisteredParams[strParamName].m_fValueSet = true;
	m_mapRegisteredParams[strParamName].m_strParamValue = strParamValue;

Error:
	return r;
}

std::string CommandLineManager::GetParameterValue(std::string strParamName) {

	if (m_mapRegisteredParams.find(strParamName) == m_mapRegisteredParams.end())
		return "";

	RegisteredParameter regParam = m_mapRegisteredParams[strParamName];

	if (regParam.m_fValueSet && regParam.m_fEnabled) {
		return regParam.m_strParamValue;
	}
	else {
		return regParam.m_strDefaultValue;
	}
}

std::vector<std::string> CommandLineManager::GetParameterValues(std::string strParamName, char cDelim) {
	std::vector<std::string> retVec = std::vector<std::string>();
	
	std::string strParamVal = GetParameterValue(strParamName);
	
	size_t dPosition = 0;
	std::string strTok;

	while ((dPosition = strParamVal.find(cDelim)) != std::string::npos) {
		strTok = strParamVal.substr(0, dPosition);
		retVec.push_back(strTok);
		strParamVal.erase(0, dPosition + 1);
	}

	retVec.push_back(strParamVal);

	return retVec;
}

RESULT CommandLineManager::DisableParameter(std::string strParamName) {
	RESULT r = R_PASS;

	CBM((m_mapRegisteredParams.find(strParamName) != m_mapRegisteredParams.end()), "%s param not found", strParamName.c_str());
	m_mapRegisteredParams[strParamName].m_fEnabled = false;

Error:
	return r;
}

RESULT CommandLineManager::EnableParameter(std::string strParamName) {
	RESULT r = R_PASS;

	CBM((m_mapRegisteredParams.find(strParamName) != m_mapRegisteredParams.end()), "%s param not found", strParamName.c_str());
	m_mapRegisteredParams[strParamName].m_fEnabled = true;

Error:
	return r;
}

void CommandLineManager::ForEach(std::function<void(const std::string&)> func) {
	for (auto& arg : m_strCommandLineArguments) {
		func(arg);
	}
}
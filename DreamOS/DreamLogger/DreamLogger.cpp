#include "DreamLogger.h"

#include "Sandbox/PathManager.h"

#include <ctime>

/*
#define ELPP_THREAD_SAFE 1
//#define ELPP_FORCE_USE_STD_THREAD 1
#define ELPP_NO_DEFAULT_LOG_FILE


INITIALIZE_EASYLOGGINGPP
*/

//SHARE_EASYLOGGINGPP
//INITIALIZE_NULL_EASYLOGGINGPP

// Init the singleton
DreamLogger* DreamLogger::s_pInstance = nullptr;

#if (defined(_WIN32) || defined(_WIN64))
	//extern char* GetCommandLineA();
	//extern unsigned long GetModuleFileNameA(void*, char*, unsigned long);

	// TODO: Replace these with the proper path manager stuff
	std::string GetPathOfExecutible () {
		char szPathResult[MAX_PATH];
		return std::string(szPathResult, GetModuleFileNameA(nullptr, szPathResult, MAX_PATH));
	}

	std::string GetCommandLineString() {
		return std::string(GetCommandLineA());
	}

	uint32_t GetProcessID() {
		return GetCurrentProcessId();
	}
#endif

#if (defined(__linux) || defined(__linux__))
	#include <string>
	#include <limits.h>
	#include <unistd.h>

	std::string GetPathOfExecutible () {
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		return std::string(result, (count > 0) ? count : 0);
	}

	std::string GetCommandLineString() {
		// TODO: Not imp.
		return "";
	}

	uint32_t GetProcessID() {
		// TODO: Not imp.
		return 0;
	}

#endif


DreamLogger::DreamLogger() {
	// empty
}

DreamLogger::~DreamLogger() {
	// empty
}

RESULT DreamLogger::InitializeLogger() {
	RESULT r = R_PASS;

	// TODO: Delete old logs

	std::time_t timeNow = std::time(nullptr);
	std::tm *localTimeNow = std::localtime(&timeNow);

	char szTime[32];
	std::strftime(szTime, 32, "%Y-%m-%d_%H-%M-%S", localTimeNow);

	// TODO: Move this to PathManager please 
	auto pszAppDataPath = std::getenv(DREAM_OS_PATH_ENV);
	if (pszAppDataPath != nullptr) {
		m_strDreamLogPath = std::string{ pszAppDataPath } + "\\logs\\" + "log-" + szTime + ".log";
	}
	else {
		m_strDreamLogPath = std::string("\\logs\\") + "log-" + szTime + ".log";
	}

	// Set up the logger 
	/*
	el::Configurations loggerConfiguration;
	m_pDreamLogger = el::Loggers::getLogger("dos");
	CN(m_pDreamLogger);

	loggerConfiguration.setToDefault();
	loggerConfiguration.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %thread [DOS] %level %msg");
	loggerConfiguration.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %thread [DOS] %level %msg");

	loggerConfiguration.setGlobally(el::ConfigurationType::Filename, m_strDreamLogPath);
	loggerConfiguration.setGlobally(el::ConfigurationType::ToStandardOutput, "false");

	m_pDreamLogger->configure(loggerConfiguration);
	*/

	el::Configurations defaultConf;

	defaultConf.setToDefault();
	defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %thread [DOS] %level %msg");
	defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %thread [DOS] %level %msg");
	
	defaultConf.setGlobally(el::ConfigurationType::Filename, m_strDreamLogPath);
	defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");

	el::Loggers::reconfigureLogger("default", defaultConf);

	m_pDreamLogger = el::Loggers::getLogger("default");
	CN(m_pDreamLogger);

	Log(DreamLogger::Level::INFO, "Process Launched: %v", GetPathOfExecutible());
	Log(DreamLogger::Level::INFO, "PID: %v", GetProcessID());
	Log(DreamLogger::Level::INFO, "Process Arguments: %v", GetCommandLineString());

Error:
	return R_PASS;
}
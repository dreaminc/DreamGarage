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

	if (m_pDreamLogger != nullptr) {
		m_pDreamLogger = nullptr;
	}

	spdlog::drop_all();
}

RESULT DreamLogger::Flush() {
	if (m_pDreamLogger != nullptr)
		m_pDreamLogger->flush();
	else
		R_NOT_INITIALIZED;

	return R_PASS;
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

	// Check if logs folder exists 
	PathManager::instance()->GetDirectoryPathFromFilePath(m_strDreamLogPath);
	if (PathManager::instance()->DoesPathExist(m_strDreamLogPath) != R_DIRECTORY_FOUND) {
		// Create the directory
		PathManager::instance()->CreateDirectory(L"logs");
	}

	// Set up async mode and flush to 1 second
	spdlog::set_async_mode(LOG_QUEUE_SIZE, spdlog::async_overflow_policy::block_retry, nullptr, std::chrono::seconds(1));

	m_pDreamLogger = spdlog::basic_logger_mt("DOS", m_strDreamLogPath);
	CN(m_pDreamLogger);

	Log(DreamLogger::Level::INFO, "Process Launched: %s", GetPathOfExecutible());
	Log(DreamLogger::Level::INFO, "PID: %d", GetProcessID());
	//Log(DreamLogger::Level::INFO, "Process Arguments: %s", GetCommandLineString());

Error:
	return R_PASS;
}
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
	std::string GetPathOfExecutible() {
		char szPathResult[MAX_PATH];
		return std::string(szPathResult, GetModuleFileNameA(nullptr, szPathResult, MAX_PATH));
	}

	std::string GetFolderPathOfExecutible() {
		std::string strExecPath = GetPathOfExecutible();
		char pBuffer[MAX_PATH];
		GetModuleFileNameA(NULL, pBuffer, MAX_PATH);
		auto slashPosition = strExecPath.find_last_of("\\/");
		return strExecPath.substr(0, slashPosition);
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

RESULT DreamLogger::InitializeLoggerNoPathmanager(std::string strLogName) {
	RESULT r = R_PASS;

	// TODO: Delete old logs

	std::time_t timeNow = std::time(nullptr);
	std::tm *localTimeNow = std::localtime(&timeNow);

	char szTime[32];
	std::strftime(szTime, 32, "%Y-%m-%d_%H-%M-%S", localTimeNow);

	m_strDreamLogPath = GetFolderPathOfExecutible() + "\\" + "log-" + szTime + ".log";

	// Set up async mode and flush to 1 second
	spdlog::set_async_mode(LOG_QUEUE_SIZE, spdlog::async_overflow_policy::block_retry, nullptr, std::chrono::seconds(1));

	m_pDreamLogger = spdlog::basic_logger_mt(strLogName, m_strDreamLogPath);
	CN(m_pDreamLogger);

	Log(DreamLogger::Level::INFO, "Process Launched: %s", GetPathOfExecutible());
	Log(DreamLogger::Level::INFO, "PID: %d", GetProcessID());
	//Log(DreamLogger::Level::INFO, "Process Arguments: %s", GetCommandLineString());

Error:
	return R_PASS;
}

RESULT DreamLogger::InitializeLogger(std::string strLogName) {
	RESULT r = R_PASS;

	// TODO: Delete old logs

	std::time_t timeNow = std::time(nullptr);
	std::tm *localTimeNow = std::localtime(&timeNow);

	char szTime[32];
	std::strftime(szTime, 32, "%Y-%m-%d_%H-%M-%S", localTimeNow);

	std::wstring wstrAppDataPath;
	PathManager::instance()->GetDreamPath(wstrAppDataPath, DREAM_PATH_TYPE::DREAM_PATH_LOCAL);
	wstrAppDataPath = wstrAppDataPath + L"logs\\";

	// Check if logs folder exists 
	if (PathManager::instance()->DoesPathExist(wstrAppDataPath) != R_DIRECTORY_FOUND) {
		// Create the directory
		wchar_t* pwszDirectory = const_cast<wchar_t*>(wstrAppDataPath.c_str());
		PathManager::instance()->CreateDirectory(pwszDirectory);
	}

	if (wstrAppDataPath.c_str() != nullptr) {
		m_strDreamLogPath = std::string(wstrAppDataPath.begin(), wstrAppDataPath.end()) + "log-" + szTime + ".log";
	}
	else {
		m_strDreamLogPath = std::string(wstrAppDataPath.begin(), wstrAppDataPath.end()) + "log-" + szTime + ".log";
	}

	// Set up async mode and flush to 1 second
	spdlog::set_async_mode(LOG_QUEUE_SIZE, spdlog::async_overflow_policy::block_retry, nullptr, std::chrono::seconds(1));

	m_pDreamLogger = spdlog::basic_logger_mt(strLogName, m_strDreamLogPath);
	CN(m_pDreamLogger);

	Log(DreamLogger::Level::INFO, "Process Launched: %s", GetPathOfExecutible());
	Log(DreamLogger::Level::INFO, "PID: %d", GetProcessID());
	//Log(DreamLogger::Level::INFO, "Process Arguments: %s", GetCommandLineString());

Error:
	return R_PASS;
}

/*
// Note: constructor must be public for this to work
DreamLogger* DreamLogger::instance() {
	RESULT r = R_PASS;

	if (!s_pInstance) {
		s_pInstance = new DreamLogger();
		CN(s_pInstance);

		// This allows the singleton to run an initialization function that
		// can fail (unlike the constructor)
		CR(s_pInstance->InitializeLogger());
	}

	// Success:
	return s_pInstance;

Error:
	if (s_pInstance != nullptr) {
		delete s_pInstance;
		s_pInstance = nullptr;
	}

	return nullptr;
}
*/
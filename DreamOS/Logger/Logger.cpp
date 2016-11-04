#include "Logger.h"
#include <ctime>

INITIALIZE_EASYLOGGINGPP

#if (defined(_WIN32) || defined(_WIN64))
#include <string>
#include <windows.h>

std::string getexepath()
{
	char result[MAX_PATH];
	return std::string(result, GetModuleFileNameA(NULL, result, MAX_PATH));
}

std::string getcommandline()
{
	return std::string(GetCommandLineA());
}

uint32_t getprocessid()
{
	return GetCurrentProcessId();
}

#endif

#if (defined(__linux) || defined(__linux__))
#include <string>
#include <limits.h>
#include <unistd.h>

std::string getexepath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

std::string getcommandline()
{
	// TODO: Not imp.
	return "";
}

uint32_t getprocessid()
{
	// TODO: Not imp.
	return 0;
}

#endif


RESULT	Logger::InitializeLogger()
{
	// TODO: garbage-collect older logs

	std::time_t now = std::time(NULL);
	std::tm * ptm = std::localtime(&now);
	char buffer[32];
	std::strftime(buffer, 32, "%Y-%m-%d_%H-%M-%S", ptm);

	auto app_data = std::getenv("APPDATA");
	std::string path = std::string{ app_data } +"\\Dream\\logs\\" + "log-" + buffer + ".log";

	el::Configurations defaultConf;

	defaultConf.setToDefault();
	defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %thread [DOS] %level %msg");
	defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %thread [DOS] %level %msg");
	defaultConf.setGlobally(el::ConfigurationType::Filename, path);
	defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
	el::Loggers::reconfigureLogger("default", defaultConf);
	
	LOG(INFO) << "Process launched " << getexepath();
	LOG(INFO) << "Process id " << getprocessid();
	LOG(INFO) << "Process args " << getcommandline();

	return R_PASS;
}

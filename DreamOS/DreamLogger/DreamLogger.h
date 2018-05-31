#ifndef LOGGER_H_
#define LOGGER_H_
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamLogger/DreamLogger.h
// The Dream Logger contains all logging capabilities of Dream
// It should be possible to create custom logs per module
// Internally it is using EasyLogger++ for now

#include "Primitives/singleton.h"

#include <string>

#include "spdlog.h"

#define LOG_QUEUE_SIZE 1024

class DreamLogger {
public:
	enum class Level {
		INFO,
		WARN,
		ERR,
		CRITICAL,
		INVALID
	};

public:
	///*
	template <typename... Args>
	RESULT Log(DreamLogger::Level logLevel, const char* pszMessage, const Args&... args) {
		RESULT r = R_PASS;

		CN(m_pDreamLogger);

		switch (logLevel) {
			case DreamLogger::Level::INFO: {
				m_pDreamLogger->info(pszMessage, args...);				
			} break;

			case DreamLogger::Level::WARN: {
				m_pDreamLogger->warn(pszMessage, args...);
			} break;

			case DreamLogger::Level::ERR: {
				m_pDreamLogger->error(pszMessage, args...);
			} break;

			//case DreamLogger::Level::CRITICAL: {
			//	m_pDreamLogger->critical(pszMessage, args...);
			//} break;
		}

	Error:
		return r;
	}
	//*/

private:
	DreamLogger();
	~DreamLogger();

public:
	RESULT InitializeLogger();
	RESULT Flush();

private:
	//el::Logger *m_pDreamLogger = nullptr;
	std::shared_ptr<spdlog::logger> m_pDreamLogger = nullptr;

	std::string m_strDreamLogPath;

// Singleton
private:
	static DreamLogger *s_pInstance;

public:
	// Note: constructor must be public for this to work
	static DreamLogger *instance() {
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
};

#endif	// ! LOGGER_H_

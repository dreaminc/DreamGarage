#ifndef LOGGER_H_
#define LOGGER_H_
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamLogger/DreamLogger.h
// The Dream Logger contains all logging capabilities of Dream
// It should be possible to create custom logs per module
// Internally it is using EasyLogger++ for now

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

		if (m_pDreamLogger == nullptr) {
			r = R_FAIL;
			goto Error;
		}

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
	RESULT InitializeLoggerNoPathmanager(std::string strLogName = "DOS");
	RESULT InitializeLogger(std::string strLogName = "DOS");
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
	//static DreamLogger *instance();

	static DreamLogger *initializeSingletonInstance() {
		RESULT r = R_PASS;

		if (s_pInstance != nullptr) {
			r = R_FAIL;
			goto Error;
		}

		s_pInstance = new DreamLogger();
		if (s_pInstance == nullptr) {
			r = R_FAIL;
			goto Error;
		}

		r = s_pInstance->InitializeLogger();
		if (r != R_PASS) {
			goto Error;
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

	static DreamLogger *instance() {
		RESULT r = R_PASS;

		if (!s_pInstance) {
			s_pInstance = new DreamLogger();
			if (s_pInstance == nullptr) {
				r = R_FAIL;
				goto Error;
			}

			// This allows the singleton to run an initialization function that
			// can fail (unlike the constructor)
			r = s_pInstance->InitializeLogger();
			if (r != R_PASS) {
				goto Error;
			}
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

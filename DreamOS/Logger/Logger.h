#ifndef LOGGER_H_
#define LOGGER_H_

#include "RESULT/EHM.h"

#define ELPP_THREAD_SAFE 1
//#define ELPP_FORCE_USE_STD_THREAD 1
#define ELPP_NO_DEFAULT_LOG_FILE

#include "easylogging++.h"

// DREAM OS
// DreamOS/Logger/Logger.h


class Logger {
public:
	static RESULT	InitializeLogger();
};

#endif	// ! LOGGER_H_

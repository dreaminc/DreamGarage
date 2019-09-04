#ifndef DREAM_TEST_APP_MAIN_H_
#define DREAM_TEST_APP_MAIN_H_

#include "RESULT/EHM.h"
#include <stdlib.h>

// DREAM OS
// DreamTestApp/DreamTestAppMain.h
// Dream Test App Main

// TODO: This is a bit of a hack around the WIN32_LEAN_AND_MEAN OpenGL compilation issue
#if defined(_WIN32)
	#include <windows.h>
	#include <shellapi.h>
#endif

#endif // !#define DREAM_TEST_APP_MAIN_H_
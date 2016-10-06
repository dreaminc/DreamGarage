#ifndef DREAM_OS_MAIN_H_
#define DREAM_OS_MAIN_H_

#include "RESULT/EHM.h"
#include <stdlib.h>

// DREAM OS
// DreamOS/DREAMOSMain.h
// Dream OS Main Header File

// TODO: This is a bit of a hack around the WIN32_LEAN_AND_MEAN OpenGL compilation issue
#if defined(_WIN32)
	#include <windows.h>
#endif

#endif // !DREAM_OS_MAIN_H_
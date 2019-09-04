#ifndef DREAM_CLIENT_MAIN_H_
#define DREAM_CLIENT_MAIN_H_

#include "RESULT/EHM.h"
#include <stdlib.h>

// DREAM OS
// DreamOS/DreamGarage/DreamClientMain.h
// Dream Client Main Header File

// TODO: This is a bit of a hack around the WIN32_LEAN_AND_MEAN OpenGL compilation issue
#if defined(_WIN32)
	#include <windows.h>
	#include <shellapi.h>
#endif

#endif // !DREAM_CLIENT_MAIN_H_
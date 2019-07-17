#ifndef WIN32_HELPER_H_
#define WIN32_HELPER_H_

#include <thread>
#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win32Helper.h
// Win32 helper functions



class Win32Helper {
public:

	static RESULT ThreadBlockingMessageLoop();
	static RESULT PostQuitMessage(std::thread& thread);

private:

};

#endif // ! WIN32_HELPER_H_
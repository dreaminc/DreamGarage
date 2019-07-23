#ifndef WIN32_HELPER_H_
#define WIN32_HELPER_H_

#include "core/ehm/EHM.h"

// DREAM OS
// dos/src/sandbox/win64/Win32Helper.h

// Win32 helper functions

#include <thread>
#include <windows.h>

// TODO: Wtf does this do?
class Win32Helper {
public:
	static RESULT ThreadBlockingMessageLoop();
	static RESULT PostQuitMessage(std::thread& thread);

private:

};

#endif // ! WIN32_HELPER_H_
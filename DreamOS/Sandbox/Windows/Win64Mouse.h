#ifndef WIN64_MOUSE_H_
#define WIN64_MOUSE_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Mouse.h
// Sense Windows 64 Mouse Device

#include "Sense/SenseMouse.h"

class Win64Mouse : public SenseMouse {
public:
	Win64Mouse() :
		SenseMouse()
	{
		// empty for now
	}

	RESULT UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state) {
		return SetMouseState(eventType, newX, newY, state);
	}
};

#endif // ! WIN64_KEYBOARD_H_
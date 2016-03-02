#ifndef WIN64_MOUSE_H_
#define WIN64_MOUSE_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Mouse.h
// Sense Windows 64 Mouse Device

#include "Sense/SenseMouse.h"

class Windows64App;

class Win64Mouse : public SenseMouse {
public:
	Win64Mouse(Windows64App *pWin64AppParent);

	RESULT UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state);

	RESULT CaptureMouse();
	RESULT ReleaseMouse();
	RESULT SetMousePosition(int x, int y);
	RESULT CenterMousePosition();

	RESULT UpdateMousePosition();

private:
	Windows64App *m_pWin64AppParent;
};

#endif // ! WIN64_KEYBOARD_H_
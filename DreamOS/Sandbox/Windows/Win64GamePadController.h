#ifndef WIN64_GAMEPAD_CONTROLLER_H_
#define WIN64_GAMEPAD_CONTROLLER_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Mouse.h
// Sense Windows 64 Mouse Device

#include "Sense/SenseGamePadController.h"

class Windows64App;
class XINPUT_GAMEPAD;

class Win64GamePadController : public SenseGamePadController {
public:
	Win64GamePadController(Windows64App *pWin64AppParent);

	RESULT UpdateGamePad();

private:
	RESULT GetGamePadState();
	RESULT ProcessGamePadState(XINPUT_GAMEPAD xInputGamePad);

private:
	Windows64App* m_pWin64AppParent = nullptr;

	DWORD m_dwPreviousPacketNumber = 0;
	WORD m_buttonState = 0;
	ButtonStruct m_buttonStruct = { 0 };
};

#endif // ! WIN64_KEYBOARD_H_

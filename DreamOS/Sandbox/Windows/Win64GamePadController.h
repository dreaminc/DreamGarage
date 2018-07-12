#ifndef WIN64_GAMEPAD_CONTROLLER_H_
#define WIN64_GAMEPAD_CONTROLLER_H_

#include <windows.h>
#include <Xinput.h>
#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Mouse.h
// Sense Windows 64 Mouse Device

#include "Sense/SenseGamepadController.h"

class Windows64App;

class Win64GamepadController : public SenseGamepadController {
public:
	Win64GamepadController(Windows64App *pWin64AppParent);

	virtual RESULT UpdateGamepad() override;

private:
	RESULT GetGamepadState();
	RESULT ProcessGamepadState(XINPUT_GAMEPAD xInputGamepad);

private:
	Windows64App* m_pWin64AppParent = nullptr;

	point2D m_leftJoystick;
	point2D m_rightJoystick;

	int m_triggerLeft;
	int m_triggerRight;

	unsigned long m_dwPreviousPacketNumber = 0;
	unsigned short m_previousButtonState = 0;

	SenseGamepadState m_currentGamepadState;
};

#endif // ! WIN64_GAMEPAD_H_

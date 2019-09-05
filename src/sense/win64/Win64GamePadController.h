#ifndef WIN64_GAMEPAD_CONTROLLER_H_
#define WIN64_GAMEPAD_CONTROLLER_H_

#include "core/ehm/EHM.h"

// Dream Sense Win64
// dos/src/sense/win64/Win64Mouse.h

// Sense Windows 64 Mouse Device

#include <windows.h>
#include <Xinput.h>

#include "sense/SenseGamepadController.h"

class Win64Sandbox;

class Win64GamepadController : public SenseGamepadController {
public:
	Win64GamepadController(Win64Sandbox *pParentWin64Sandbox);

	virtual RESULT UpdateGamepad() override;

private:
	RESULT GetGamepadState();
	RESULT ProcessGamepadState(XINPUT_GAMEPAD xInputGamepad);

private:
	Win64Sandbox* m_pParentWin64Sandbox = nullptr;

	point2D m_leftJoystick;
	point2D m_rightJoystick;

	int m_triggerLeft;
	int m_triggerRight;

	unsigned long m_dwPreviousPacketNumber = 0;
	unsigned short m_previousButtonState = 0;

	SenseGamepadState m_currentGamepadState;
};

#endif // ! WIN64_GAMEPAD_H_

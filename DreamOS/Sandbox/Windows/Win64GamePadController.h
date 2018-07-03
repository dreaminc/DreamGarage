#ifndef WIN64_GAMEPAD_CONTROLLER_H_
#define WIN64_GAMEPAD_CONTROLLER_H_

#include <windows.h>
#include <Xinput.h>
#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Mouse.h
// Sense Windows 64 Mouse Device

#include "Sense/SenseGamePadController.h"

class Windows64App;

class Win64GamePadController : public SenseGamePadController {
public:
	Win64GamePadController(Windows64App *pWin64AppParent);

	virtual RESULT UpdateGamePad() override;

private:
	RESULT GetGamePadState();
	RESULT ProcessGamePadState(XINPUT_GAMEPAD xInputGamePad);

private:
	Windows64App* m_pWin64AppParent = nullptr;

	DWORD m_dwPreviousPacketNumber = 0;
	WORD m_buttonState = 0;
	GamePadButtonStruct m_buttonStruct;
};

#endif // ! WIN64_GAMEPAD_H_

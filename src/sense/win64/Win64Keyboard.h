#ifndef WIN64_KEYBOARD_H_
#define WIN64_KEYBOARD_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Keyboard.h
// Sense Windows 64 Keyboard Device

#include "Sense/SenseKeyboard.h"

class Windows64App;

class Win64Keyboard : public SenseKeyboard {
public:
	Win64Keyboard(Windows64App *pWin64AppParent);
	RESULT UpdateKeyStates();
	virtual RESULT UpdateKeyState(SenseVirtualKey key, uint8_t keyState) override;
	RESULT CheckKeyState(SenseVirtualKey key);

private:
	Windows64App *m_pWin64AppParent;
};

#endif // ! WIN64_KEYBOARD_H_
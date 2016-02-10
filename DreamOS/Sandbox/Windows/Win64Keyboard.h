#ifndef WIN64_KEYBOARD_H_
#define WIN64_KEYBOARD_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Keyboard.h
// Sense Windows 64 Keyboard Device

#include "Sense/SenseKeyboard.h"

class Win64Keyboard : public SenseKeyboard {
public:
	Win64Keyboard() :
		SenseKeyboard()
	{
		// empty for now
	}

	RESULT UpdateKeyStates() {
		RESULT r = R_PASS;

		CBM((GetKeyboardState(m_KeyStates)), "Win64Keyboard: Failed to get Keyboard state with error 0x%x", GetLastError());

	Error:
		return r;
	}

	RESULT UpdateKeyState(uint8_t key, uint8_t keyState) {
		return SetKeyState(key, keyState);
	}

	RESULT CheckKeyState(int key) {
		RESULT r = R_PASS;

		uint16_t keyState = GetAsyncKeyState(key);

		bool fDown = keyState & 0x8000;
		bool fPressed = keyState & 0x0001;

		if (fDown)
			DEBUG_LINEOUT("Key %d: %d down pressed: %d", key, fDown, fPressed);

		uint8_t newKeyState = (fDown << 1) + (fPressed << 0);
		SetKeyState(key, newKeyState);

	Error:
		return r;
	}
};

#endif // ! WIN64_KEYBOARD_H_
#ifndef WIN64_KEYBOARD_H_
#define WIN64_KEYBOARD_H_

#include "core/ehm/EHM.h"

// Dream Sense Win64
// dos/src/sense/win64/Win64Keyboard.h

// Sense Windows 64 Keyboard Device

#include <windows.h>

#include "sense/SenseKeyboard.h"

class Win64Sandbox;

class Win64Keyboard : public SenseKeyboard {
public:
	Win64Keyboard(Win64Sandbox *pParentWin64Sandbox);

	RESULT UpdateKeyStates();
	
	virtual RESULT UpdateKeyState(SenseVirtualKey key, uint8_t keyState) override;
	
	RESULT CheckKeyState(SenseVirtualKey key);

private:
	Win64Sandbox *m_pParentWin64Sandbox = nullptr;
};

#endif // ! WIN64_KEYBOARD_H_
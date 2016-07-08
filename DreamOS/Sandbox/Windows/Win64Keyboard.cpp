#include "Win64Keyboard.h"
#include "Windows64App.h"

Win64Keyboard::Win64Keyboard(Windows64App *pWin64AppParent) :
	SenseKeyboard(),
	m_pWin64AppParent(pWin64AppParent)
{
	// empty for now
}

RESULT Win64Keyboard::UpdateKeyStates() {
	RESULT r = R_PASS;

	CBM((GetKeyboardState(m_KeyStates)), "Win64Keyboard: Failed to get Keyboard state with error 0x%x", GetLastError());

Error:
	return r;
}

RESULT Win64Keyboard::UpdateKeyState(SK_SCAN_CODE key, uint8_t keyState) {
	return SetKeyState(key, keyState);
}

RESULT Win64Keyboard::CheckKeyState(SK_SCAN_CODE key) {
	RESULT r = R_PASS;

	uint16_t keyState = GetAsyncKeyState(key);

	bool fDown = (keyState & 0x8000) != 0;
	bool fPressed = (keyState & 0x0001) != 0;

	if (fDown) {
		DEBUG_LINEOUT("Key %d: %d down pressed: %d", key, fDown, fPressed);
	};

	uint8_t newKeyState = (fDown << 1) + (fPressed << 0);
	SetKeyState(key, newKeyState);

	return r;
}

#include "SenseKeyboard.h"

#include <string.h>
#include <functional>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseKeyboard.h
// Sense Keyboard Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

SenseKeyboard::SenseKeyboard() {
	memset(m_KeyStates, 0, sizeof(uint8_t) * NUM_SENSE_KEYBOARD_KEYS);

	for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; i++) {
		RegisterEvent(i);
	}
}

SenseKeyboard::~SenseKeyboard() {
	// empty stub
}

RESULT SenseKeyboard::SetKeyState(SK_SCAN_CODE KeyCode, uint8_t KeyState) {
	RESULT r = R_PASS;

	if (KeyState != m_KeyStates[KeyCode]) {
		m_KeyStates[KeyCode] = KeyState;

		//DEBUG_LINEOUT("Key %d state: %x", KeyCode, KeyState);

		// Notify Observers
		SenseKeyboardEvent kbEvent(KeyCode, KeyState, this);
		CR(NotifySubscribers(KeyCode, &kbEvent));

		SenseKeyboardEvent kbEventAll(SK_ALL, 0, this);
		CR(NotifySubscribers(SK_ALL, &kbEventAll));
	}

Error:
	return r;
}

uint8_t SenseKeyboard::GetKeyState(SK_SCAN_CODE KeyCode) {
	switch (KeyCode)
	{
		case SK_SHIFT:
		case SK_CONTROL: {
#if defined(_WIN32) || defined(_WIN64)
			return ::GetKeyState(KeyCode);// ((::GetKeyState(KeyCode) & 0x80) > 0) ? 1 : 0;
#else
			// Not Implemented.
			#pragma message ("GetKeyState not implemented for Shift/Control keys")
			return 0;
#endif
		} break;

		default: {
			return m_KeyStates[(int)KeyCode];
		} break;
	};
}

RESULT SenseKeyboard::SetKeyStates(uint8_t KeyStates[NUM_SENSE_KEYBOARD_KEYS]) {
	memcpy(m_KeyStates, KeyStates, sizeof(uint8_t) * NUM_SENSE_KEYBOARD_KEYS);
	return R_PASS;
}

void SenseKeyboard::ForEachKeyPressed(std::function<void(SK_SCAN_CODE)> func) {
	for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; ++i) {
		// For now a key press is when the state not equal zero.
		// TODO: Needs to be changed to a type rather than an int.
		if (m_KeyStates[i] != 0) {
			func(static_cast<SK_SCAN_CODE>(i));
		}
	}
}

#ifndef SENSE_KEYBOARD_H_
#define SENSE_KEYBOARD_H_

#include <string.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseKeyboard.h
// Sense Keyboard Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

#define NUM_SENSE_KEYBOARD_KEYS 256

// TODO: Complete the scan codes, ensure they make sense 
typedef enum SenseKeyboardScanCodes {
	SK_SPACE = 0x20,
	SK_PRIOR = 0x21,
	SK_NEXT = 0x22,
	SK_END = 0x23,
	SK_HOME = 0x24,
	SK_LEFT = 0x25,
	SK_UP = 0x26,
	SK_RIGHT = 0x27,
	SK_DOWN = 0x28,
	SK_SELECT = 0x29,
	SK_PRINT = 0x2A,
	SK_EXECUTE = 0x2B,
	SK_SNAPSHOT = 0x2C,
	SK_INSERT = 0x2D,
	SK_DELETE = 0x2E,
	SK_HELP = 0x2F,
	SK_INVALID
} SK_SCAN_CODE;

typedef struct SenseKeyboardEvent : SenseDevice::SenseDeviceEvent {
	SK_SCAN_CODE KeyCode;
	uint8_t KeyState;

	SenseKeyboardEvent(SK_SCAN_CODE key, uint8_t state) :
		SenseDeviceEvent()
	{
		SenseEventSize = sizeof(SenseKeyboardEvent);
		KeyCode = key;
		KeyState = state;
	}
} SENSE_KEYBOARD_EVENT;

class SenseKeyboard : public SenseDevice, public Publisher<int, SenseKeyboardEvent> {
public:
	SenseKeyboard() {
		memset(m_KeyStates, 0, sizeof(uint8_t) * NUM_SENSE_KEYBOARD_KEYS);

		for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; i++) {
			RegisterEvent(i);
		}
	}

	~SenseKeyboard() {
		// empty stub
	}

	RESULT SetKeyState(SK_SCAN_CODE KeyCode, uint8_t KeyState) {
		RESULT r = R_PASS;

		if (KeyState != m_KeyStates[KeyCode]) {
			m_KeyStates[KeyCode] = KeyState;

			DEBUG_LINEOUT("Key %d state: %x", KeyCode, KeyState);

			// Notify Observers
			SenseKeyboardEvent kbEvent(KeyCode, KeyState);
			CR(NotifySubscribers(KeyCode, &kbEvent));
		}

	Error:
		return r;
	}

	uint8_t GetKeyState(SK_SCAN_CODE KeyCode) {
		return m_KeyStates[(int)KeyCode];
	}

	RESULT SetKeyStates(uint8_t KeyStates[NUM_SENSE_KEYBOARD_KEYS]) {
		memcpy(m_KeyStates, KeyStates, sizeof(uint8_t) * NUM_SENSE_KEYBOARD_KEYS);
		return R_PASS;
	}
	
	// The SenseKeyboard interface
public:
	virtual RESULT UpdateKeyStates() = 0;
	virtual RESULT CheckKeyState(SK_SCAN_CODE key) = 0;

protected:
	uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];
};

#endif // ! SENSE_KEYBOARD_H_
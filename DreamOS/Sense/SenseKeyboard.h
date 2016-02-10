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

typedef struct SenseKeyboardEvent : SenseDevice::SenseDeviceEvent {
	uint8_t KeyCode;
	uint8_t KeyState;

	SenseKeyboardEvent(uint8_t key, uint8_t state) :
		SenseDeviceEvent()
	{
		SenseEventSize = sizeof(SenseKeyboardEvent);
		KeyCode = key;
		KeyState = state;
	}
} SENSE_KEYBOARD_EVENT;

class SenseKeyboard : public SenseDevice, public Publisher<int> {
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

	RESULT SetKeyState(uint8_t KeyCode, uint8_t KeyState) {
		if (KeyState != m_KeyStates[KeyCode]) {
			m_KeyStates[KeyCode] = KeyState;

			DEBUG_LINEOUT("Key %d state: %x", KeyCode, KeyState);

			// TODO: Notify Observers
		}

		return R_PASS;
	}

	uint8_t GetKeyState(uint8_t KeyCode) {
		return m_KeyStates[KeyCode];
	}

	RESULT SetKeyStates(uint8_t KeyStates[NUM_SENSE_KEYBOARD_KEYS]) {
		memcpy(m_KeyStates, KeyStates, sizeof(uint8_t) * NUM_SENSE_KEYBOARD_KEYS);
		return R_PASS;
	}
	
	// The SenseKeyboard interface
public:
	virtual RESULT UpdateKeyStates() = 0;
	virtual RESULT CheckKeyState(int key) = 0;

protected:
	uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];
};

#endif // ! SENSE_KEYBOARD_H_
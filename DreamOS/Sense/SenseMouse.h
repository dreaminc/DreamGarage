#ifndef SENSE_KEYBOARD_H_
#define SENSE_KEYBOARD_H_

#include <string.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseMouse.h
// Sense Mouse Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

typedef struct SenseMouseEvent : SenseDevice::SenseDeviceEvent {

	SenseMouseEvent() : 
	SenseDeviceEvent() 
	{
		SenseEventSize = sizeof(SenseMouseEvent);
	}
} SENSE_MOUSE_EVENT;

class SenseMouse : public SenseDevice, public Publisher<int> {
public:
	SenseMouse() {

		/*
		for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; i++) {
			RegisterEvent(i);
		}
		*/
	}

	~SenseMouse() {
		// empty stub
	}

	/*
	RESULT SetKeyState(uint8_t KeyCode, uint8_t KeyState) {
		RESULT r = R_PASS;

		if (KeyState != m_KeyStates[KeyCode]) {
			m_KeyStates[KeyCode] = KeyState;

			DEBUG_LINEOUT("Key %d state: %x", KeyCode, KeyState);

			// Notify Observers
			SenseKeyboardEvent kbEvent(KeyCode, KeyState);
			CR(NotifySubscribers(KeyCode, &kbEvent));
		}

	Error:
		return R_PASS;
	}
	*/

	// The SenseMouse interface
public:
	//virtual RESULT UpdateKeyStates() = 0;
	//virtual RESULT CheckKeyState(int key) = 0;

protected:
	//uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];
};

#endif // ! SENSE_KEYBOARD_H_
#ifndef SENSE_KEYBOARD_H_
#define SENSE_KEYBOARD_H_

#include <string.h>
#include <functional>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseKeyboard.h
// Sense Keyboard Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

#define NUM_SENSE_KEYBOARD_KEYS 256

// TODO: Complete the scan codes, ensure they make sense 
typedef enum SenseKeyboardScanCodes {
	SK_ALL = 0x0,
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

class SenseKeyboard;

typedef struct SenseKeyboardEvent : SenseDevice::SenseDeviceEvent {
	SK_SCAN_CODE KeyCode;
	uint8_t KeyState;
	SenseKeyboard* m_pSenseKeyboard = nullptr;

	SenseKeyboardEvent(SK_SCAN_CODE key, uint8_t state, SenseKeyboard* pSenseKeyboard = nullptr) :
		SenseDeviceEvent(),
		m_pSenseKeyboard(pSenseKeyboard)
	{
		SenseEventSize = sizeof(SenseKeyboardEvent);
		KeyCode = key;
		KeyState = state;
	}
} SENSE_KEYBOARD_EVENT;

class SenseKeyboard : public SenseDevice, public Publisher<int, SenseKeyboardEvent> {
public:
	SenseKeyboard();

	~SenseKeyboard();

	virtual RESULT UpdateKeyState(SK_SCAN_CODE key, uint8_t keyState) = 0;

	RESULT SetKeyState(SK_SCAN_CODE KeyCode, uint8_t KeyState);

	uint8_t GetKeyState(SK_SCAN_CODE KeyCode);

	RESULT SetKeyStates(uint8_t KeyStates[NUM_SENSE_KEYBOARD_KEYS]);

	void ForEachKeyPressed(std::function<void(SK_SCAN_CODE)> func);
	
	// The SenseKeyboard interface
public:
	virtual RESULT UpdateKeyStates() = 0;
	virtual RESULT CheckKeyState(SK_SCAN_CODE key) = 0;

protected:
	uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];
};

#endif // ! SENSE_KEYBOARD_H_
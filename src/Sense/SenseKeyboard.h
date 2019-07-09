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

// helper functions for converting the received UTF-16 character (on windows platform) to a non-unicode string
std::string utf16_to_utf8(std::u16string utf16_string);

#define NUM_SENSE_KEYBOARD_KEYS 256

// SenseVirtualKey is identical to window's VK, but is used as a platform-free vk codes
typedef enum SenseKeyboardVirtualKeys : unsigned int {
	SVK_ALL = 0x0,
	SVK_BACK = 0x08,
	SVK_TAB = 0x09,
	SVK_SHIFT = 0x10,
	SVK_CONTROL = 0x11,
	SVK_SPACE = 0x20,
	SVK_PRIOR = 0x21,
	SVK_NEXT = 0x22,
	SVK_END = 0x23,
	SVK_HOME = 0x24,
	SVK_LEFT = 0x25,
	SVK_UP = 0x26,
	SVK_RIGHT = 0x27,
	SVK_DOWN = 0x28,
	SVK_SELECT = 0x29,
	SVK_PRINT = 0x2A,
	SVK_ESCAPE = 0x1B,
	SVK_EXECUTE = 0x2B,
	SVK_SNAPSHOT = 0x2C,
	SVK_RETURN = 0x0D,
	SVK_INSERT = 0x2D,
	SVK_DELETE = 0x2E, 
	SVK_HELP = 0x2F,
	SVK_INVALID
} SenseVirtualKey;

class SenseKeyboard;

typedef struct SenseKeyboardEvent : SenseDevice::SenseDeviceEvent {
	SenseVirtualKey KeyCode;
	uint8_t KeyState;
	SenseKeyboard* m_pSenseKeyboard = nullptr;

	SenseKeyboardEvent(SenseVirtualKey key, uint8_t state, SenseKeyboard* pSenseKeyboard = nullptr) :
		SenseDeviceEvent(),
		m_pSenseKeyboard(pSenseKeyboard)
	{
		SenseEventSize = sizeof(SenseKeyboardEvent);
		KeyCode = key;
		KeyState = state;
	}

} SENSE_KEYBOARD_EVENT;

typedef enum SenseTypingEventType {
	CHARACTER_TYPING
} SENSE_TYPING_EVENT_TYPE;

typedef struct SenseTypingEvent : SenseDevice::SenseDeviceEvent {
	SenseVirtualKey KeyCode;
	uint8_t KeyState;
	char16_t u16character;
	SenseKeyboard* m_pSenseKeyboard = nullptr;

	SenseTypingEvent(SenseVirtualKey key, uint8_t state, char16_t u16char, SenseKeyboard* pSenseKeyboard = nullptr) :
		SenseDeviceEvent(),
		m_pSenseKeyboard(pSenseKeyboard)
	{
		SenseEventSize = sizeof(SenseTypingEvent);
		KeyCode = key;
		KeyState = state;
		u16character = u16char;
	}

} SENSE_TYPING_EVENT;

class SenseKeyboard : public SenseDevice, public Publisher<SenseVirtualKey, SenseKeyboardEvent>, public Publisher<SenseTypingEventType, SenseTypingEvent> {
public:
	SenseKeyboard();

	~SenseKeyboard();

	// raw keyboard event
	virtual RESULT UpdateKeyState(SenseVirtualKey key, uint8_t keyState) = 0;
	// text typing event
	// this event is separated than the raw event as it in Windows OS
	// and represents a character being typed in a UTF-16 format
	virtual RESULT NotifyTextTyping(SenseVirtualKey key, char16_t u16char, uint8_t keyState);

	RESULT SetKeyState(SenseVirtualKey KeyCode, uint8_t KeyState);

	uint8_t GetKeyState(SenseVirtualKey KeyCode);

	RESULT SetKeyStates(uint8_t KeyStates[NUM_SENSE_KEYBOARD_KEYS]);

	void ForEachKeyPressed(std::function<void(SenseVirtualKey)> func);
	
	// The SenseKeyboard interface
public:
	virtual RESULT UpdateKeyStates() = 0;
	virtual RESULT CheckKeyState(SenseVirtualKey key) = 0;

protected:
	uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];
};

#endif // ! SENSE_KEYBOARD_H_
#include "SenseKeyboard.h"

#include <string.h>
#include <functional>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseKeyboard.h
// Sense Keyboard Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

#include <locale>
#include <codecvt>

#if _MSC_VER == 1900
std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
	return convert.to_bytes(p, p + utf16_string.size());
}
#else
std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(utf16_string);
}
#endif


SenseKeyboard::SenseKeyboard() {
	memset(m_KeyStates, 0, sizeof(uint8_t) * NUM_SENSE_KEYBOARD_KEYS);

	for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; i++) {
		Publisher<SenseVirtualKey, SenseKeyboardEvent>::RegisterEvent(static_cast<SenseVirtualKey>(i));
	}

	Publisher<SenseTypingEventType, SenseTypingEvent>::RegisterEvent(CHARACTER_TYPING);
}

SenseKeyboard::~SenseKeyboard() {
	// empty stub
}

RESULT SenseKeyboard::SetKeyState(SenseVirtualKey KeyCode, uint8_t KeyState) {
	RESULT r = R_PASS;

	if (KeyState != m_KeyStates[KeyCode]) {
		m_KeyStates[KeyCode] = KeyState;

		// Notify Observers
		SenseKeyboardEvent kbEvent(KeyCode, KeyState, this);
		Publisher<SenseVirtualKey, SenseKeyboardEvent>::NotifySubscribers(KeyCode, &kbEvent);

		SenseKeyboardEvent kbEventAll(KeyCode, KeyState, this);
		Publisher<SenseVirtualKey, SenseKeyboardEvent>::NotifySubscribers(SVK_ALL, &kbEventAll);
	}

//Error:
	return r;
}

RESULT SenseKeyboard::NotifyTextTyping(SenseVirtualKey key, char16_t u16char, uint8_t keyState) {
	RESULT r = R_PASS;

	SenseTypingEvent kbEvent(key, keyState, u16char, this);
	Publisher<SenseTypingEventType, SenseTypingEvent>::NotifySubscribers(CHARACTER_TYPING, &kbEvent);

//Error:
	return r;
}

uint8_t SenseKeyboard::GetKeyState(SenseVirtualKey KeyCode) {
	switch (KeyCode)
	{
		case SVK_SHIFT:
		case SVK_CONTROL: {
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

void SenseKeyboard::ForEachKeyPressed(std::function<void(SenseVirtualKey)> func) {
	for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; ++i) {
		// For now a key press is when the state not equal zero.
		// TODO: Needs to be changed to a type rather than an int.
		if (m_KeyStates[i] != 0) {
			func(static_cast<SenseVirtualKey>(i));
		}
	}
}

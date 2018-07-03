#include "SenseGamePadController.h"

SenseGamePadController::SenseGamePadController()
{
	for (int i = 0; i < SENSE_GAMEPAD_INVALID; i++) {
		RegisterEvent((SenseGamePadEventType)(i));
	}
}

SenseGamePadController::~SenseGamePadController()
{
}

RESULT SenseGamePadController::SetGamePadState(SenseGamePadEventType eventType, GamePadState gpState) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Sense Gamepad Event x:%f y:%f T:%f",
		gpState.ptJoyStick.x(), gpState.ptJoyStick.y(), gpState.triggerRange);

	switch (eventType) {
	case(SENSE_GAMEPAD_LEFTSTICK): {
		NotifySubscribers(eventType, &SenseGamePadEvent(eventType, gpState));
	} break;

	case(SENSE_GAMEPAD_RIGHTSTICK): {
		NotifySubscribers(eventType, &SenseGamePadEvent(eventType, gpState));
	} break;

	case(SENSE_GAMEPAD_BUTTONS): {
		NotifySubscribers(eventType, &SenseGamePadEvent(eventType, gpState));
	} break;

	case(SENSE_GAMEPAD_TRIGGER_LEFT): {
		NotifySubscribers(eventType, &SenseGamePadEvent(eventType, gpState));
	} break;

	case(SENSE_GAMEPAD_TRIGGER_RIGHT): {
		NotifySubscribers(eventType, &SenseGamePadEvent(eventType, gpState));
	} break;
	
	default: {
	}

	};

//Error:
	return r;
}

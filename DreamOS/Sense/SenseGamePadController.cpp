#include "SenseGamepadController.h"

SenseGamepadController::SenseGamepadController()
{
	memset(&m_currentGamepadState, 0, sizeof(SenseGamepadState));

	for (int i = 0; i < SENSE_GAMEPAD_INVALID; i++) {
		RegisterEvent((SenseGamepadEventType)(i));
	}
}

SenseGamepadController::~SenseGamepadController() {
	//empty
}

RESULT SenseGamepadController::SetGamepadState(SenseGamepadState gpState) {
	RESULT r = R_PASS;

	if (!gpState.leftJoystick.IsZero() && !m_currentGamepadState.leftJoystick.IsZero()) {
		NotifySubscribers(SENSE_GAMEPAD_JOYSTICK_LEFT, &SenseGamepadEvent(SENSE_GAMEPAD_JOYSTICK_LEFT, gpState.leftJoystick));
	}
	if (gpState.rightJoystick != m_currentGamepadState.rightJoystick) {
		NotifySubscribers(SENSE_GAMEPAD_JOYSTICK_RIGHT, &SenseGamepadEvent(SENSE_GAMEPAD_JOYSTICK_RIGHT, gpState.rightJoystick));
	}
	if (gpState.leftTriggerRange != m_currentGamepadState.leftTriggerRange) {
		point2D triggerAsPoint;
		triggerAsPoint(0,0) = gpState.leftTriggerRange;
		NotifySubscribers(SENSE_GAMEPAD_TRIGGER_LEFT, &SenseGamepadEvent(SENSE_GAMEPAD_TRIGGER_LEFT, triggerAsPoint));
	}
	if (gpState.rightTriggerRange != m_currentGamepadState.rightTriggerRange) {
		point2D triggerAsPoint;
		triggerAsPoint(0,0) = gpState.rightTriggerRange;
		NotifySubscribers(SENSE_GAMEPAD_TRIGGER_RIGHT, &SenseGamepadEvent(SENSE_GAMEPAD_TRIGGER_RIGHT, triggerAsPoint));
	}

	if (gpState.buttonStruct.fDpadUp != m_currentGamepadState.buttonStruct.fDpadUp) {
		if (gpState.buttonStruct.fDpadUp) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_DPAD_UP));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_DPAD_UP));
		}
	}
	if (gpState.buttonStruct.fDpadDown != m_currentGamepadState.buttonStruct.fDpadDown) {
		if (gpState.buttonStruct.fDpadDown) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_DPAD_DOWN));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_DPAD_DOWN));
		}
	}
	if (gpState.buttonStruct.fDpadLeft != m_currentGamepadState.buttonStruct.fDpadLeft) {
		if (gpState.buttonStruct.fDpadLeft) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_DPAD_LEFT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_DPAD_LEFT));
		}
	}
	if (gpState.buttonStruct.fDpadRight != m_currentGamepadState.buttonStruct.fDpadRight) {
		if (gpState.buttonStruct.fDpadRight) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_DPAD_RIGHT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_DPAD_RIGHT));
		}
	}
	if (gpState.buttonStruct.fStart != m_currentGamepadState.buttonStruct.fStart) {
		if (gpState.buttonStruct.fStart) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_START));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_START));
		}
	}
	if (gpState.buttonStruct.fSelect != m_currentGamepadState.buttonStruct.fSelect) {
		if (gpState.buttonStruct.fSelect) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_SELECT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_SELECT));
		}
	}
	if (gpState.buttonStruct.fLeftThumb != m_currentGamepadState.buttonStruct.fLeftThumb) {
		if (gpState.buttonStruct.fLeftThumb) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_BUTTON_JOYSTICK_LEFT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_BUTTON_JOYSTICK_LEFT));
		}
	}
	if (gpState.buttonStruct.fRightThumb != m_currentGamepadState.buttonStruct.fRightThumb) {
		if (gpState.buttonStruct.fRightThumb) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_BUTTON_JOYSTICK_RIGHT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_BUTTON_JOYSTICK_RIGHT));
		}
	}
	if (gpState.buttonStruct.fLeftShoulder != m_currentGamepadState.buttonStruct.fLeftShoulder) {
		if (gpState.buttonStruct.fLeftShoulder) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_SHOULDER_LEFT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_SHOULDER_LEFT));
		}
	}
	if (gpState.buttonStruct.fRightShoulder != m_currentGamepadState.buttonStruct.fRightShoulder) {
		if (gpState.buttonStruct.fRightShoulder) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_SHOULDER_RIGHT));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_SHOULDER_RIGHT));
		}
	}
	if (gpState.buttonStruct.fbuttonA != m_currentGamepadState.buttonStruct.fbuttonA) {
		if (gpState.buttonStruct.fbuttonA) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_A));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_A));
		}
	}
	if (gpState.buttonStruct.fbuttonB != m_currentGamepadState.buttonStruct.fbuttonB) {
		if (gpState.buttonStruct.fbuttonB) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_B));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_B));
		}
	}
	if (gpState.buttonStruct.fbuttonX != m_currentGamepadState.buttonStruct.fbuttonX) {
		if (gpState.buttonStruct.fbuttonX) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_X));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_X));
		}
	}
	if (gpState.buttonStruct.fbuttonY != m_currentGamepadState.buttonStruct.fbuttonY) {
		if (gpState.buttonStruct.fbuttonY) {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_DOWN, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_DOWN, SENSE_GAMEPAD_Y));
		}
		else {
			NotifySubscribers(SENSE_GAMEPAD_BUTTON_UP, &SenseGamepadEvent(SENSE_GAMEPAD_BUTTON_UP, SENSE_GAMEPAD_Y));
		}
	}

	m_currentGamepadState = gpState;

//Error:
	return r;
}

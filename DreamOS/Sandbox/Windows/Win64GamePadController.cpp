#include "Win64GamepadController.h"
#include "Windows64App.h"

Win64GamepadController::Win64GamepadController(Windows64App *pWin64AppParent) :
	SenseGamepadController(),
	m_pWin64AppParent(pWin64AppParent)
{
	// empty for now
}

RESULT Win64GamepadController::UpdateGamepad() {
	RESULT r = R_PASS;
	
	CR(GetGamepadState());

Error:
	return r;
}

RESULT Win64GamepadController::GetGamepadState() {
	RESULT r = R_PASS;

	XINPUT_STATE xInputState;
	memset(&xInputState, 0, sizeof(XINPUT_STATE));

	// If the controller is not connected, the return value is ERROR_DEVICE_NOT_CONNECTED.
	CRM((RESULT)XInputGetState(0, &xInputState), "Controller not Connected");
	
	//If the dwPacketNumber member is the same in sequentially returned XINPUT_STATE structures, the controller state has not changed.
	CBR(xInputState.dwPacketNumber != m_dwPreviousPacketNumber, R_SKIPPED);
	m_dwPreviousPacketNumber = xInputState.dwPacketNumber;

	ProcessGamepadState(xInputState.Gamepad);

Error:
	return r;
}

RESULT Win64GamepadController::ProcessGamepadState(XINPUT_GAMEPAD xInputGamepad) {
	RESULT r = R_PASS;

	// joystick values are signed 32767, triggers are 0-255

	float leftStickX = xInputGamepad.sThumbLX;
	float leftStickY = xInputGamepad.sThumbLY;
	float rightStickX = xInputGamepad.sThumbRX;
	float rightStickY = xInputGamepad.sThumbRY;

	//determine how far the controller is pushed
	float leftMagnitude = sqrt(leftStickX * leftStickX + leftStickY * leftStickY);
	float rightMagnitude = sqrt(rightStickX * rightStickX + rightStickY * rightStickY);

	//determine the direction the controller is pushed (left and down are negative)
	float normalizedLX = leftStickX / leftMagnitude;
	float normalizedLY = leftStickY / leftMagnitude;
	float normalizedRX = rightStickX / rightMagnitude;
	float normalizedRY = rightStickY / rightMagnitude;

	float normalizedLeftMagnitude = 0;
	float normalizedRightMagnitude = 0;

	// check if the controller is outside a circular dead zone
	// using default deadzones for now, including trigger thresholds
	if (leftMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		
		// clip the magnitude at its expected maximum value
		if (leftMagnitude > 32767) {
			leftMagnitude = 32767;
		}
		// adjust magnitude relative to the end of the dead zone
		leftMagnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		// optionally normalize the magnitude with respect to its expected range
		// giving a magnitude value of 0.0 to 1.0
		normalizedLeftMagnitude = leftMagnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}
	else {	// if the controller is in the deadzone zero out the magnitude
		normalizedLX = 0;
		normalizedLY = 0;
	}

	// if the joystick state changed, set new values
	if (m_leftJoystick.x() != normalizedLX && m_leftJoystick(0,1) != normalizedLY) {
		m_leftJoystick.x() = normalizedLX * normalizedLeftMagnitude;
		m_leftJoystick.y() = normalizedLY * normalizedLeftMagnitude;
		m_currentGamepadState.leftJoystick = m_leftJoystick;
	}
	
	// repeat for right stick
	if (rightMagnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		if (rightMagnitude > 32767) {
			rightMagnitude = 32767;
		}

		rightMagnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		normalizedRightMagnitude = rightMagnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}
	else {
		normalizedRX = 0;
		normalizedRY = 0;
	}

	if (m_rightJoystick.x() != normalizedRX && m_rightJoystick(0,1) != normalizedRY) {
		m_rightJoystick.x() = normalizedRX * normalizedRightMagnitude;
		m_rightJoystick.y() = normalizedRY * normalizedRightMagnitude;
		m_currentGamepadState.rightJoystick = m_rightJoystick;
	}

	// Triggers
	if (xInputGamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && xInputGamepad.bLeftTrigger != m_triggerLeft) {
		m_triggerLeft = xInputGamepad.bLeftTrigger;
		m_currentGamepadState.leftTriggerRange = m_triggerLeft;
	}
	else if (m_triggerLeft != 0 && xInputGamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {	// inside of threshold but we haven't send the 0 yet
		m_triggerLeft = 0;
		m_currentGamepadState.leftTriggerRange = m_triggerLeft;
	}

	if (xInputGamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && xInputGamepad.bRightTrigger != m_triggerRight) {
		m_triggerRight = xInputGamepad.bRightTrigger;
		m_currentGamepadState.rightTriggerRange = m_triggerRight;
	}
	else if (m_triggerRight != 0 && xInputGamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		m_triggerRight = 0;
		m_currentGamepadState.rightTriggerRange = m_triggerRight;
	}

	// Buttons (including DPad)
	// XINPUT_GAMEPAD_DPAD_UP		0x0001
	// XINPUT_GAMEPAD_DPAD_DOWN		0x0002
	// XINPUT_GAMEPAD_DPAD_LEFT		0x0004
	// XINPUT_GAMEPAD_DPAD_RIGHT	0x0008
	// XINPUT_GAMEPAD_START			0x0010
	// XINPUT_GAMEPAD_BACK			0x0020
	// XINPUT_GAMEPAD_LEFT_THUMB	0x0040
	// XINPUT_GAMEPAD_RIGHT_THUMB	0x0080
	// XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
	// XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
	// XINPUT_GAMEPAD_A				0x1000
	// XINPUT_GAMEPAD_B				0x2000
	// XINPUT_GAMEPAD_X				0x4000
	// XINPUT_GAMEPAD_Y				0x8000
	if (m_previousButtonState != xInputGamepad.wButtons) {
		m_currentGamepadState.buttonStruct.fDpadUp = ((xInputGamepad.wButtons & 1 << 0) != 0);
		m_currentGamepadState.buttonStruct.fDpadDown = ((xInputGamepad.wButtons & 1 << 1) != 0);
		m_currentGamepadState.buttonStruct.fDpadLeft = ((xInputGamepad.wButtons & 1 << 2) != 0);
		m_currentGamepadState.buttonStruct.fDpadRight = ((xInputGamepad.wButtons & 1 << 3) != 0);
		m_currentGamepadState.buttonStruct.fStart = ((xInputGamepad.wButtons & 1 << 4) != 0);
		m_currentGamepadState.buttonStruct.fSelect = ((xInputGamepad.wButtons & 1 << 5) != 0);
		m_currentGamepadState.buttonStruct.fLeftThumb = ((xInputGamepad.wButtons & 1 << 6) != 0);
		m_currentGamepadState.buttonStruct.fRightThumb = ((xInputGamepad.wButtons & 1 << 7) != 0);
		m_currentGamepadState.buttonStruct.fLeftShoulder = ((xInputGamepad.wButtons & 1 << 8) != 0);
		m_currentGamepadState.buttonStruct.fRightShoulder = ((xInputGamepad.wButtons & 1 << 9) != 0);
		m_currentGamepadState.buttonStruct.fbuttonA = ((xInputGamepad.wButtons & 1 << 12) != 0);
		m_currentGamepadState.buttonStruct.fbuttonB = ((xInputGamepad.wButtons & 1 << 13) != 0);
		m_currentGamepadState.buttonStruct.fbuttonX = ((xInputGamepad.wButtons & 1 << 14) != 0);
		m_currentGamepadState.buttonStruct.fbuttonY = ((xInputGamepad.wButtons & 1 << 15) != 0);
		
		m_previousButtonState = xInputGamepad.wButtons;
	}

	SetGamepadState(m_currentGamepadState);

//Error:
	return r;
}

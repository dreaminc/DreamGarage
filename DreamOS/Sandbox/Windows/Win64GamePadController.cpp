#include "Win64GamePadController.h"
#include "Windows64App.h"

Win64GamePadController::Win64GamePadController(Windows64App *pWin64AppParent) :
	SenseGamePadController(),
	m_pWin64AppParent(pWin64AppParent)
{
	// empty for now
}

RESULT Win64GamePadController::UpdateGamePad() {
	RESULT r = R_PASS;
	
	CR(GetGamePadState());

Error:
	return r;
}

RESULT Win64GamePadController::GetGamePadState() {
	RESULT r = R_PASS;

	XINPUT_STATE xInputState;
	memset(&xInputState, 0, sizeof(XINPUT_STATE));

	// If the controller is not connected, the return value is ERROR_DEVICE_NOT_CONNECTED.
	CRM((RESULT)XInputGetState(0, &xInputState), "Controller not Connected");
	
	//If the dwPacketNumber member is the same in sequentially returned XINPUT_STATE structures, the controller state has not changed.
	CBR(xInputState.dwPacketNumber != m_dwPreviousPacketNumber, R_SKIPPED);
	m_dwPreviousPacketNumber = xInputState.dwPacketNumber;

	ProcessGamePadState(xInputState.Gamepad);

Error:
	return r;
}

RESULT Win64GamePadController::ProcessGamePadState(XINPUT_GAMEPAD xInputGamePad) {
	RESULT r = R_PASS;

	// joystick values are signed 32767, triggers are 0-255

	GamePadState gpState;

	float leftStickX = xInputGamePad.sThumbLX;
	float leftStickY = xInputGamePad.sThumbLY;
	float rightStickX = xInputGamePad.sThumbRX;
	float rightStickY = xInputGamePad.sThumbRY;

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
		
		gpState.ptJoyStick = point(normalizedLX, normalizedLY, 0);
		SetGamePadState(SenseGamePadEventType::SENSE_GAMEPAD_LEFTSTICK, gpState);
	}

	/*
	else {	// if the controller is in the deadzone zero out the magnitude
		leftMagnitude = 0.0;
		normalizedLeftMagnitude = 0.0;
	}
	//*/

	// repeat for right stick
	if (rightMagnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		if (rightMagnitude > 32767) {
			rightMagnitude = 32767;
		}

		rightMagnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		normalizedRightMagnitude = rightMagnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

		gpState.ptJoyStick = point(normalizedRX, normalizedRY, 0);
		SetGamePadState(SenseGamePadEventType::SENSE_GAMEPAD_RIGHTSTICK, gpState);
	}

	// Triggers
	if (xInputGamePad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		gpState.triggerRange = xInputGamePad.bLeftTrigger;
		SetGamePadState(SenseGamePadEventType::SENSE_GAMEPAD_TRIGGER_LEFT, gpState);
	}

	if (xInputGamePad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		gpState.triggerRange = xInputGamePad.bRightTrigger;
		SetGamePadState(SenseGamePadEventType::SENSE_GAMEPAD_TRIGGER_RIGHT, gpState);
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
	if (m_buttonState != xInputGamePad.wButtons) {
		m_buttonStruct.fDPAD_UP = ((xInputGamePad.wButtons & 1 << 0) != 0);
		m_buttonStruct.fDPAD_DOWN = ((xInputGamePad.wButtons & 1 << 1) != 0);
		m_buttonStruct.fDPAD_LEFT = ((xInputGamePad.wButtons & 1 << 2) != 0);
		m_buttonStruct.fDPAD_RIGHT = ((xInputGamePad.wButtons & 1 << 3) != 0);
		m_buttonStruct.fGAMEPAD_START = ((xInputGamePad.wButtons & 1 << 4) != 0);
		m_buttonStruct.fGAMEPAD_BACK = ((xInputGamePad.wButtons & 1 << 5) != 0);
		m_buttonStruct.fGAMEPAD_LEFT_THUMB = ((xInputGamePad.wButtons & 1 << 6) != 0);
		m_buttonStruct.fGAMEPAD_RIGHT_THUMB = ((xInputGamePad.wButtons & 1 << 7) != 0);
		m_buttonStruct.fGAMEPAD_LEFT_SHOULDER = ((xInputGamePad.wButtons & 1 << 8) != 0);
		m_buttonStruct.fGAMEPAD_RIGHT_SHOULDER = ((xInputGamePad.wButtons & 1 << 9) != 0);
		m_buttonStruct.fGAMEPAD_A = ((xInputGamePad.wButtons & 1 << 10) != 0);
		m_buttonStruct.fGAMEPAD_B = ((xInputGamePad.wButtons & 1 << 11) != 0);
		m_buttonStruct.fGAMEPAD_X = ((xInputGamePad.wButtons & 1 << 12) != 0);
		m_buttonStruct.fGAMEPAD_Y = ((xInputGamePad.wButtons & 1 << 13) != 0);
		
		m_buttonState = xInputGamePad.wButtons;
	}

//Error:
	return r;
}

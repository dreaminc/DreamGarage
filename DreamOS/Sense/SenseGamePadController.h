#ifndef SENSE_GAMEPAD_CONTROLLER_H_
#define SENSE_GAMEPAD_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS

#include "SenseDevice.h"
#include "Primitives/Publisher.h"
#include "Primitives/point.h"

typedef enum SenseGamepadEventType {
	SENSE_GAMEPAD_JOYSTICK_LEFT,
	SENSE_GAMEPAD_JOYSTICK_RIGHT,
	SENSE_GAMEPAD_TRIGGER_LEFT,
	SENSE_GAMEPAD_TRIGGER_RIGHT,
	SENSE_GAMEPAD_BUTTON_UP,
	SENSE_GAMEPAD_BUTTON_DOWN,
	SENSE_GAMEPAD_INVALID
} SENSE_GAMEPAD_EVENT_TYPE;

typedef enum SenseGamepadButtonType {
	SENSE_GAMEPAD_DPAD_UP,
	SENSE_GAMEPAD_DPAD_DOWN,
	SENSE_GAMEPAD_DPAD_LEFT,
	SENSE_GAMEPAD_DPAD_RIGHT,
	SENSE_GAMEPAD_START,
	SENSE_GAMEPAD_SELECT,
	SENSE_GAMEPAD_BUTTON_JOYSTICK_LEFT,
	SENSE_GAMEPAD_BUTTON_JOYSTICK_RIGHT,
	SENSE_GAMEPAD_SHOULDER_LEFT,
	SENSE_GAMEPAD_SHOULDER_RIGHT,
	SENSE_GAMEPAD_A,	// using Xbox orientation
	SENSE_GAMEPAD_B,
	SENSE_GAMEPAD_X,
	SENSE_GAMEPAD_Y
} SENSE_GAMEPAD_BUTTON_TYPE;

struct SenseGamepadButtonStruct {
	unsigned fDpadUp : 1;
	unsigned fDpadDown : 1;
	unsigned fDpadLeft : 1;
	unsigned fDpadRight : 1;
	unsigned fStart : 1;
	unsigned fSelect : 1;
	unsigned fLeftThumb : 1;
	unsigned fRightThumb : 1;
	unsigned fLeftShoulder : 1;
	unsigned fRightShoulder : 1;
	unsigned fbuttonA : 1;	// Based on Xbox orientation
	unsigned fbuttonB : 1;
	unsigned fbuttonX : 1;
	unsigned fbuttonY : 1;
};

typedef struct SenseGamepadState {
	int leftTriggerRange = 0;
	int rightTriggerRange = 0;
	point2D leftJoystick;
	point2D rightJoystick;
	SenseGamepadButtonStruct buttonStruct;
} SENSE_GAMEPAD_STATE;

typedef struct SenseGamepadEvent : SenseDevice::SenseDeviceEvent {
	SenseGamepadEventType gamepadEventType;
	SenseGamepadButtonType gamepadButtonType;
	point2D eventData;

	SenseGamepadEvent(SenseGamepadEventType gpEventType, point2D gpEventData) :
		SenseDeviceEvent(),
		gamepadEventType(gpEventType),
		eventData(gpEventData)
	{
		SenseEventSize = sizeof(SenseGamepadEvent);
	}

	SenseGamepadEvent(SenseGamepadEventType gpEventType, SenseGamepadButtonType gpButtonType) :
		SenseDeviceEvent(),
		gamepadEventType(gpEventType),
		gamepadButtonType(gpButtonType)
	{
		SenseEventSize = sizeof(SenseGamepadEvent);
	}

} SENSE_GAMEPAD_EVENT;

class SenseGamepadController : public SenseDevice, public Publisher<SenseGamepadEventType, SenseGamepadEvent> {

public:
	SenseGamepadController();
	~SenseGamepadController();
	
	RESULT SetGamepadState(SenseGamepadState gpState);

	virtual RESULT UpdateGamepad() = 0;

private:
	SenseGamepadState m_currentGamepadState;
};

#endif // ! SENSE_GAMEPAD_CONTROLLER_H_

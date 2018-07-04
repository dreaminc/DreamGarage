#ifndef SENSE_GAMEPAD_CONTROLLER_H_
#define SENSE_GAMEPAD_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS

#include "SenseDevice.h"
#include "Primitives/Publisher.h"
#include "Primitives/point.h"

typedef enum SenseGamePadEventType {
	SENSE_GAMEPAD_LEFTSTICK,
	SENSE_GAMEPAD_RIGHTSTICK,
	SENSE_GAMEPAD_BUTTONS,
	SENSE_GAMEPAD_TRIGGER_LEFT,
	SENSE_GAMEPAD_TRIGGER_RIGHT,
	SENSE_GAMEPAD_INVALID
} SENSE_GAMEPAD_EVENT_TYPE;

struct GamePadButtonStruct {
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

typedef struct GamePadState {
	int triggerRange = 0;
	point ptJoyStick;
	GamePadButtonStruct buttonStruct;
} GAMEPAD_STATE;

typedef struct SenseGamePadEvent : SenseDevice::SenseDeviceEvent {
	SenseGamePadEventType eventType;
	GamePadState gamepadState;

	SenseGamePadEvent(SenseGamePadEventType gpEvent, GamePadState gpState) :
		SenseDeviceEvent(),
		eventType(gpEvent),
		gamepadState(gpState)
	{
		SenseEventSize = sizeof(SenseGamePadEvent);
	}
} SENSE_GAMEPAD_EVENT;

class SenseGamePadController : public SenseDevice, public Publisher<SenseGamePadEventType, SenseGamePadEvent> {

public:
	SenseGamePadController();
	~SenseGamePadController();
	
	RESULT SetGamePadState(SenseGamePadEventType eventType, GamePadState gpState);

	virtual RESULT UpdateGamePad() = 0;
};

#endif // ! SENSE_GAMEPAD_CONTROLLER_H_

#ifndef SENSE_GAMEPAD_CONTROLLER_H_
#define SENSE_GAMEPAD_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseMouse.h
// Sense Mouse Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

typedef enum SenseGamePadEventType {
	SENSE_GAMEPAD_LEFTSTICK,
	SENSE_GAMEPAD_RIGHTSTICK,
	SENSE_GAMEPAD_BUTTONS,
	SENSE_GAMEPAD_TRIGGER_LEFT,
	SENSE_GAMEPAD_TRIGGER_RIGHT,
	SENSE_GAMEPAD_INVALID
} SENSE_GAMEPAD_EVENT_TYPE;

struct ButtonStruct {
	unsigned fDPAD_UP : 1;
	unsigned fDPAD_DOWN : 1;
	unsigned fDPAD_LEFT : 1;
	unsigned fDPAD_RIGHT : 1;
	unsigned fGAMEPAD_START : 1;
	unsigned fGAMEPAD_BACK : 1;
	unsigned fGAMEPAD_LEFT_THUMB : 1;
	unsigned fGAMEPAD_RIGHT_THUMB : 1;
	unsigned fGAMEPAD_LEFT_SHOULDER : 1;
	unsigned fGAMEPAD_RIGHT_SHOULDER : 1;
	unsigned fGAMEPAD_A : 1;
	unsigned fGAMEPAD_B : 1;
	unsigned fGAMEPAD_X : 1;
	unsigned fGAMEPAD_Y : 1;
};

typedef struct GamePadState {
	float triggerRange;
	point ptJoyStick;
	ButtonStruct buttonStruct;
} GAMEPAD_STATE;

typedef struct SenseGamePadEvent : SenseDevice::SenseDeviceEvent {
	SenseGamePadEventType eventType;
	GamePadState gamepadState;

	SenseGamePadEvent(SenseGamePadEventType gpEvent, GamePadState gpState) :
		SenseDeviceEvent(),
		eventType(eventType),
		gamepadState(gpState)
	{
		SenseEventSize = sizeof(SenseGamePadEvent);
	}
} SENSE_MOUSE_EVENT;

class SenseGamePadController : public SenseDevice, public Publisher<SenseGamePadEventType, SenseGamePadEvent> {

public:
	SenseGamePadController();
	~SenseGamePadController();
	
	RESULT SetGamePadState(SenseGamePadEventType eventType, GamePadState gpState);
}

#endif // ! SENSE_GAMEPAD_CONTROLLER_H_

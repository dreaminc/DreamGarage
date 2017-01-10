#ifndef SENSE_CONTROLLER_H_
#define SENSE_CONTROLLER_H_

#include "Primitives/valid.h"

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"
#include "Primitives/point.h"
#include "SenseDevice.h"

typedef enum SenseControllerEventType {
	SENSE_CONTROLLER_GRIP_DOWN,
	SENSE_CONTROLLER_GRIP_UP,
	SENSE_CONTROLLER_MENU_DOWN,
	SENSE_CONTROLLER_MENU_UP,
	SENSE_CONTROLLER_TRIGGER_MOVE,
	SENSE_CONTROLLER_PAD_MOVE,
	SENSE_CONTROLLER_INVALID
} SENSE_CONTROLLER_EVENT_TYPE;

typedef enum ControllerType {
	CONTROLLER_LEFT,
	CONTROLLER_RIGHT,
	CONTROLLER_INVALID
} CONTROLLER_TYPE;

typedef struct ControllerState {
	CONTROLLER_TYPE type;
	float triggerRange;
	point ptTouchpad;
	bool fGrip;
	bool fMenu;
} CONTROLLER_STATE;

typedef struct SenseControllerEvent : SenseDevice::SenseDeviceEvent {
	SenseControllerEventType type;
	ControllerState state;

	SenseControllerEvent(SenseControllerEventType eventType, ControllerState controllerState) :
		SenseDeviceEvent(),
		type(eventType),
		state(controllerState)
	{
		SenseEventSize = sizeof(SenseControllerEvent);
	}
} SENSE_CONTROLLER_EVENT;

class SenseController : public SenseDevice, public Publisher<SenseControllerEventType, SenseControllerEvent>, public valid {

public:
	SenseController();
	~SenseController();

public:
	ControllerState GetControllerState();
	RESULT SetControllerState(ControllerState controllerState);

private:

	ControllerState m_controllerState;
};

#endif // ! SENSE_CONTROLLER_H_
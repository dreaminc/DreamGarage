#ifndef SENSE_CONTROLLER_H_
#define SENSE_CONTROLLER_H_

#include "Primitives/valid.h"

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"
#include "Primitives/point.h"
#include "SenseDevice.h"

typedef enum ControllerType {
	CONTROLLER_LEFT,
	CONTROLLER_RIGHT,
	CONTROLLER_INVALID
} CONTROLLER_TYPE;

typedef struct ControllerState {
	CONTROLLER_TYPE type;
	float trigger;
	point ptTouchpad;
	bool fGrip;
	bool fMenu;
} CONTROLLER_STATE;

typedef struct SenseControllerEvent : SenseDevice::SenseDeviceEvent {
	ControllerState state;

	SenseControllerEvent(ControllerState controllerState) :
		SenseDeviceEvent(),
		state(controllerState)
	{
		SenseEventSize = sizeof(SenseControllerEvent);
	}
} SENSE_CONTROLLER_EVENT;

class SenseController : public SenseDevice, public Publisher<int, SenseControllerEvent>, public valid {

public:
	SenseController();
	~SenseController();

public:
	ControllerState GetControllerState();
	RESULT SetControllerState(ControllerState controllerState);

private:

	CONTROLLER_TYPE m_type;
	float m_trigger;
	point m_touchpad;
	bool m_grip;
	bool m_menu;
};

#endif // ! SENSE_CONTROLLER_H_
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
	SENSE_CONTROLLER_TRIGGER_DOWN,
	SENSE_CONTROLLER_TRIGGER_UP,
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
	enum class HapticCurveType {
		SQUARE,
		SAW,
		SINE,
		EXPONENTIAL,
		CONSTANT,
		INVALID
	};

public:
	SenseController();
	~SenseController();

	virtual RESULT Initialize() = 0;
	virtual RESULT SubmitHapticBuffer(ControllerType controllerType, HapticCurveType type, float amplitude, float freq, float msDuration) = 0;
	virtual RESULT SubmitHapticImpulse(ControllerType controllerType, HapticCurveType shape, float amplitude, float msDuration, int cycles = 1) = 0;

public:
	ControllerState GetControllerState(ControllerType type);
	RESULT SetControllerState(ControllerState controllerState);

private:
	std::map<ControllerType, ControllerState> m_controllerStates;
};

#endif // ! SENSE_CONTROLLER_H_
#ifndef SENSE_KEYBOARD_H_
#define SENSE_KEYBOARD_H_

#include <string.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseMouse.h
// Sense Mouse Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

typedef enum SenseMouseEventType {
	SENSE_MOUSE_POSITION,
	SENSE_MOUSE_LEFT_BUTTON,
	SENSE_MOUSE_MIDDLE_BUTTON,
	SENSE_MOUSE_RIGHT_BUTTON,
	SENSE_MOUSE_WHEEL,
	SENSE_MOUSE_INVALID
} SENSE_MOUSE_EVENT_TYPE;

typedef struct SenseMouseEvent : SenseDevice::SenseDeviceEvent {
	SenseMouseEventType EventType;
	int xPos;
	int dx;

	int yPos;
	int dy;

	int state;

	SenseMouseEvent(SenseMouseEventType eventType, int newX, int newY, int oldX, int oldY, int newState) :
		SenseDeviceEvent(),
		EventType(eventType),
		xPos(newX),
		yPos(newY),
		state(newState)
	{
		SenseEventSize = sizeof(SenseMouseEvent);
		dx = newX - oldX;
		dx = newY - oldY;
	}
} SENSE_MOUSE_EVENT;

class SenseMouse : public SenseDevice, public Publisher<SenseMouseEventType, SenseMouseEvent> {
protected:
	//uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];

	int m_LeftButtonState;
	int m_MiddleButtonState;
	int m_RightButtonState;

	int m_mouseWheel;

private:
	struct SenseMousePosition {
		int xPos;
		int yPos;
	} m_MousePosition;

public:
	SenseMouse() :
		m_LeftButtonState(0),
		m_MiddleButtonState(0),
		m_RightButtonState(0)
	{
		SetMousePosition(0, 0);

		for (int i = 0; i < SENSE_MOUSE_INVALID; i++) {
			RegisterEvent((SenseMouseEventType)(i));
		}
	}

	~SenseMouse() {
		// empty stub
	}

	RESULT SetMouseState(SenseMouseEventType eventType, int newX, int newY, int state) {
		RESULT r = R_PASS;

		SenseMouseEvent mEvent(eventType, newX, newY, m_MousePosition.xPos, m_MousePosition.yPos, state);
		SetMousePosition(newX, newY);

		CR(NotifySubscribers(eventType, &mEvent));

	Error:
		return r;
	}

private:
	int &GetMouseState(SenseMouseEventType eventType) {
		switch (eventType) {
			case SENSE_MOUSE_LEFT_BUTTON: return m_LeftButtonState; break;
			case SENSE_MOUSE_MIDDLE_BUTTON: return m_MiddleButtonState; break;
			case SENSE_MOUSE_RIGHT_BUTTON: return m_RightButtonState; break;
			case SENSE_MOUSE_WHEEL: return m_mouseWheel; break;
		}
	}

	SenseMousePostion &GetMousePosition() {
		return m_MousePosition;
	}

	RESULT SetMousePosition(int x, int y) {
		m_MousePosition.xPos = 0;
		m_MousePosition.yPos = 0;

		return R_PASS;
	}

	// The SenseMouse interface
public:
	virtual RESULT UpdateMouseStates() = 0;
	virtual RESULT CheckMouseState(SenseMouseEventType eventType) = 0;

};

#endif // ! SENSE_KEYBOARD_H_
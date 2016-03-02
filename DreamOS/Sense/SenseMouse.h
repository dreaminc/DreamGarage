#ifndef SENSE_MOUSE_H_
#define SENSE_MOUSE_H_

#include <string.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseMouse.h
// Sense Mouse Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

typedef enum SenseMouseEventType {
	SENSE_MOUSE_LEFT_BUTTON,
	SENSE_MOUSE_MIDDLE_BUTTON,
	SENSE_MOUSE_RIGHT_BUTTON,
	SENSE_MOUSE_WHEEL,
	SENSE_MOUSE_MOVE,
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
		dy = newY - oldY;
	}
} SENSE_MOUSE_EVENT;

class SenseMouse : public SenseDevice, public Publisher<SenseMouseEventType, SenseMouseEvent> {
protected:
	//uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];

	int m_LeftButtonState;
	int m_MiddleButtonState;
	int m_RightButtonState;

	int m_mouseWheel;

	bool m_fMouseCaptured;

private:
	typedef struct SenseMousePosition {
		int xPos;
		int yPos;
	} SENSE_MOUSE_POSITION;

	SenseMousePosition m_MousePosition;

public:
	SenseMouse() :
		m_LeftButtonState(0),
		m_MiddleButtonState(0),
		m_RightButtonState(0),
		m_fMouseCaptured(false)
	{
		SetMousePosition(0, 0);

		for (int i = 0; i < SENSE_MOUSE_INVALID; i++) {
			RegisterEvent((SenseMouseEventType)(i));
		}
	}

	~SenseMouse() {
		// empty stub
	}

	static const char *GetEventTypeName(SenseMouseEventType eventType) {
		switch (eventType) {
			case SENSE_MOUSE_LEFT_BUTTON: return "left button"; 
			case SENSE_MOUSE_MIDDLE_BUTTON: return "middle button";
			case SENSE_MOUSE_RIGHT_BUTTON: return "right button";
			case SENSE_MOUSE_WHEEL: return "wheel";
			case SENSE_MOUSE_MOVE: return "position";

			default:
			case SENSE_MOUSE_INVALID: return "invalid";
		}
	}

	static const char *GetEventTypeName(SenseMouseEvent event) {
		return GetEventTypeName(event.EventType);
	}

	static const char *GetEventTypeName(SenseMouseEvent *pEvent) {
		return GetEventTypeName(pEvent->EventType);
	}

	static RESULT PrintEvent(SenseMouseEvent *pEvent) {
		DEBUG_LINEOUT("Sense Mouse Event %s x:%d y:%d dx:%d dy:%d state:%d", 
			GetEventTypeName(pEvent), pEvent->xPos, pEvent->yPos, pEvent->dx, pEvent->dy, pEvent->state);
		
		return R_PASS;
	}

	RESULT SetMouseState(SenseMouseEventType eventType, int newX, int newY, int state) {
		RESULT r = R_PASS;
		static bool fFirst = true;

		SenseMouseEvent mEvent(eventType, newX, newY, m_MousePosition.xPos, m_MousePosition.yPos, state);

		// First event should be to set the mouse position
		// TODO: This is a bit of a hack - might need to further develop this in the future
		if (fFirst) {
			//SenseMouse::SetMousePosition(newX, newY);
			goto Error;
		}

		//DEBUG_LINEOUT("Event %s x:%d y:%d dx:%d dy:%d state:%d", GetEventTypeName(eventType), newX, newY, mEvent.dx, mEvent.dy, state);
		if (eventType == SENSE_MOUSE_MOVE && (mEvent.dx != 0 || mEvent.dy != 0)) {
			CR(NotifySubscribers(eventType, &mEvent));

			if (m_fMouseCaptured && !fFirst) {
				CR(CenterMousePosition());
			}
			else {
				SenseMouse::SetMousePosition(newX, newY);
			}
		}
		else if (eventType != SENSE_MOUSE_MOVE) {
			CR(NotifySubscribers(eventType, &mEvent));
		}

	Error:
		fFirst = false;
		return r;
	}

private:
	RESULT GetMouseState(SenseMouseEventType eventType, int &rvalue) {
		RESULT r = R_PASS;

		CB((eventType <= SENSE_MOUSE_RIGHT_BUTTON));

		switch (eventType) {
			case SENSE_MOUSE_LEFT_BUTTON: rvalue = m_LeftButtonState; break;
			case SENSE_MOUSE_MIDDLE_BUTTON: rvalue = m_MiddleButtonState; break;
			case SENSE_MOUSE_RIGHT_BUTTON: rvalue = m_RightButtonState; break;
			case SENSE_MOUSE_WHEEL: rvalue = m_mouseWheel; break;
		}

	Error:
		return r;
	}

	/*
	SenseMousePostion& GetMousePosition() {
		return m_MousePosition;
	}
	*/

protected:
	RESULT SetMousePosition(int x, int y) {
		m_MousePosition.xPos = x;
		m_MousePosition.yPos = y;

		return R_PASS;
	}

	RESULT CaptureMouse() {
		m_fMouseCaptured = true;
		return R_PASS;
	}

	RESULT ReleaseMouse() {
		m_fMouseCaptured = false;
		return R_PASS;
	}

	// The SenseMouse interface
public:
	virtual RESULT CenterMousePosition() = 0;
	virtual RESULT UpdateMousePosition() = 0;
	//virtual RESULT SetMousePosition(int x, int y) = 0;

};

#endif // ! SENSE_MOUSE_H_